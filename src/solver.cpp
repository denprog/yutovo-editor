#include "solver.h"
#include "document.h"
#include "config.h"
#include "web_socket.h"
#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace yutovo
{

using namespace std::chrono_literals;
using namespace yutovo_service;

//Solver

Solver::Solver(Document* _document) :
    document(_document),
    message_loop(std::thread(&Solver::MessageLoop, this)),
    guid(boost::uuids::to_string(boost::uuids::random_generator()())),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true))
{
    result_types_seq = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX};
}

Solver::~Solver()
{
    exit = true;
    next_circle = true;
    message_loop.join();
}

void Solver::Solve(ElementId id, uint code_id, yutovo_service::ResultType result_type, const uint precision, 
    AngleMeasure angle_measure, Notation notation, const std::u32string& expression)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        switch (result_type)
        {
        case yutovo_service::ResultType::AUTO:
            tasks.emplace(new RealSolverTask(id, guid, code_id, ExpressionType::SOLVE, precision, angle_measure, expression));
            tasks.emplace(new IntegerSolverTask(id, guid, code_id, ExpressionType::SOLVE, notation, expression));
            tasks.emplace(new RationalSolverTask(id, guid, code_id, ExpressionType::SOLVE, expression));
            break;
        case yutovo_service::ResultType::REAL:
            tasks.emplace(new RealSolverTask(id, guid, code_id, ExpressionType::SOLVE, precision, angle_measure, expression));
            break;
        case yutovo_service::ResultType::INTEGER:
            tasks.emplace(new IntegerSolverTask(id, guid, code_id, ExpressionType::SOLVE, notation, expression));
            break;
        case yutovo_service::ResultType::RATIONAL:
            tasks.emplace(new RationalSolverTask(id, guid, code_id, ExpressionType::SOLVE, expression));
            break;
        }

        tasks.emplace(nullptr);
    }
    next_circle = true;
}

void Solver::SetUserIdentifier(ElementId id, uint code_id, const std::u32string& expression)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace(new RealSolverTask(id, guid, code_id, ExpressionType::USER_SYMBOL, 0, AngleMeasure::RADIAN, expression));
    tasks.emplace(new IntegerSolverTask(id, guid, code_id, ExpressionType::USER_SYMBOL, Notation::DECIMAL, expression));
    tasks.emplace(new RationalSolverTask(id, guid, code_id, ExpressionType::USER_SYMBOL, expression));
    tasks.emplace(nullptr);
    next_circle = true;
}

void Solver::RemoveIdentifier(ElementId id, uint code_id, const std::u32string& identifier)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace(new RemoveIdentifierSolverTask(id, guid, code_id, ResultType::REAL, identifier));
    tasks.emplace(nullptr);
    tasks.emplace(new RemoveIdentifierSolverTask(id, guid, code_id, ResultType::INTEGER, identifier));
    tasks.emplace(nullptr);
    tasks.emplace(new RemoveIdentifierSolverTask(id, guid, code_id, ResultType::RATIONAL, identifier));
    tasks.emplace(nullptr);
    next_circle = true;
}

void Solver::MessageLoop()
{
    WebSocketPtr socket(new WebSocket(document->config));
    if (!socket->Connect())
    {
        logger->Error("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
    }

    time_t now = time(0);
    time_t next = now;
    
    std::vector<SolverTaskPtr> temp_tasks;
    while (!exit)
    {
        {
            bool empty = false;
            {
                std::unique_lock<std::mutex> lock(tasks_mutex);
                empty = tasks.empty();
            }
            if (empty)
            {
                while (!next_circle) //wait for tasks
                {
                    std::this_thread::sleep_for(10ms);
                    if (!socket->IsOpen())
                    {
                        next = time(0);
                        if (next - now >= reconnect_period)
                            break;
                    }
                }
            }

            if (!socket->IsOpen())
            {
                if (next - now >= reconnect_period)
                {
                    now = time(0);
                    if (!socket->Connect())
                    {
                        logger->Error("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
                        continue;
                    }
                }
            }

            std::unique_lock<std::mutex> lock(tasks_mutex);
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
            if (t->Execute(socket, result) && t->expression_type == ExpressionType::SOLVE)
                break;
            if (result.error.error_code == yutovo_service::ErrorCode::OPERATION_ERROR)
            {
                socket.reset(new WebSocket(document->config)); //recreate the socket
                if (!socket->Connect())
                {
                    logger->Error("Error connecting to the server");
                }
                break;
            }
            if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
                break;
        }

        document->PutResult(temp_tasks[0]->id, result);

        if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
            document->ReSolve(temp_tasks[0]->id); //re-solve the expressions above and later this one
        
        temp_tasks.clear();
    }
}

}
