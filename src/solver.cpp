#include "solver.h"
#include "document.h"
#include <zmq.hpp>
#include <chrono>

namespace yutovo
{

using namespace std::chrono_literals;

Solver::Solver(Document* _document) :
    document(_document),
    message_loop(std::thread(&Solver::MessageLoop, this))
{
    result_types_seq = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX};
}

Solver::~Solver()
{
    exit = true;
    message_loop.join();
}

void Solver::Solve(ElementId id, ExpressionType expression_type, ResultType result_type, const uint precision, AngleMeasure angle_measure, 
    Notation notation, const std::string& expression)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        switch (result_type)
        {
        case ResultType::AUTO:
            tasks.emplace(new RealSolverTask(id, expression_type, precision, angle_measure, expression));
            tasks.emplace(new IntegerSolverTask(id, expression_type, notation, expression));
            break;
        case ResultType::REAL:
            tasks.emplace(new RealSolverTask(id, expression_type, precision, angle_measure, expression));
            break;
        case ResultType::INTEGER:
            tasks.emplace(new IntegerSolverTask(id, expression_type, notation, expression));
            break;
        case ResultType::RATIONAL:
            tasks.emplace(new RationalSolverTask(id, expression_type, expression));
            break;
        case ResultType::COMPLEX:
            tasks.emplace(new ComplexSolverTask(id, expression_type, precision, angle_measure, expression));
            break;
        }

        tasks.emplace(nullptr);
    }
    next_circle.notify_one();
}

void Solver::OnResult(Result result)
{
    document->PutResult(ElementId{}, result);
}

void Solver::MessageLoop()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:8010");

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
            if (t->Solve(socket, result))
                break;
        }
        document->PutResult(temp_tasks[0]->id, result);
        temp_tasks.clear();
    }
}

}
