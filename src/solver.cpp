#include "solver.h"
#include "document.h"
#include "config.h"
#include <zmq.hpp>
#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

using namespace std::chrono_literals;
using namespace yutovo_service;

Solver::Solver(Document* _document) :
    document(_document),
    message_loop(std::thread(&Solver::MessageLoop, this)),
    guid(boost::uuids::to_string(boost::uuids::random_generator()()))
{
    result_types_seq = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX};
}

Solver::~Solver()
{
    exit = true;
    message_loop.join();
}

void Solver::Solve(ElementId id, uint code_id, ExpressionType expression_type, yutovo_service::ResultType result_type, const uint precision, 
    AngleMeasure angle_measure, Notation notation, const std::string& expression)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        switch (result_type)
        {
        case yutovo_service::ResultType::AUTO:
            tasks.emplace(new RealSolverTask(id, guid, code_id, expression_type, precision, angle_measure, expression));
            tasks.emplace(new IntegerSolverTask(id, guid, code_id, expression_type, notation, expression));
            break;
        case yutovo_service::ResultType::REAL:
            tasks.emplace(new RealSolverTask(id, guid, code_id, expression_type, precision, angle_measure, expression));
            break;
        case yutovo_service::ResultType::INTEGER:
            tasks.emplace(new IntegerSolverTask(id, guid, code_id, expression_type, notation, expression));
            break;
        case yutovo_service::ResultType::RATIONAL:
            tasks.emplace(new RationalSolverTask(id, guid, code_id, expression_type, expression));
            break;
        case yutovo_service::ResultType::COMPLEX:
            tasks.emplace(new ComplexSolverTask(id, guid, code_id, expression_type, precision, angle_measure, expression));
            break;
        }

        tasks.emplace(nullptr);
    }
    next_circle.notify_one();
}

void Solver::MessageLoop()
{
    zmq::context_t context(1);
    SocketPtr socket;
    CreateSocket(socket, context);

    std::vector<SolverTaskPtr> temp_tasks;
    while (!exit)
    {
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            if (tasks.empty())
            {
                if (next_circle.wait_for(lock, 100ms) == std::cv_status::timeout) //wait for tasks
                    continue;
            }
            while (!tasks.empty() && tasks.front() == nullptr)
                tasks.pop();
            if (tasks.empty())
                continue;
            while (tasks.front() != nullptr)
            {
                temp_tasks.push_back(tasks.front());
                tasks.pop();
            }
        }

        Result result;
        for (SolverTaskPtr t : temp_tasks) //try all variants of parsers until one of them solves
        {
            if (t->Solve(*socket.get(), result))
                break;
            if (result.error.error_code == yutovo_service::ErrorCode::SOLVER_TIMEOUT_ERROR)
            {
                CreateSocket(socket, context); //recreate the socket
                break;
            }
            if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
                break;
        }
        document->PutResult(temp_tasks[0]->id, result);
        temp_tasks.clear();
    }

    socket->setsockopt(ZMQ_LINGER, 0);
    socket->close();
}

void Solver::CreateSocket(SocketPtr& socket, zmq::context_t& context)
{
    if (socket)
    {
        socket->setsockopt(ZMQ_LINGER, 0);
        socket->close();
    }
    Config& config = document->config;
    socket.reset(new zmq::socket_t(context, ZMQ_REQ));
    socket->setsockopt(ZMQ_SNDTIMEO, config.service_timeout * 1000);
    socket->setsockopt(ZMQ_RCVTIMEO, config.service_timeout * 1000);
    socket->connect("tcp://" + config.service_ip + ":" + std::to_string(config.service_port));
}

}
