#include "solver.h"
#include "document.h"
#include "config.h"
#include "web_socket.h"
#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

namespace yutovo
{

using namespace std::chrono_literals;
using namespace std::chrono;
using namespace yutovo_service;

//Solver

Solver::Solver(Document* _document) :
    document(_document),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true)),
    message_loop(std::thread(&Solver::MessageLoop, this)),
    guid(boost::uuids::to_string(boost::uuids::random_generator()()))
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
    AngleMeasure angle_measure, Notation notation, const std::u32string& expression, const uint delay)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
            [id](SolverTaskPtr& task)
            {
                return task && task->id == id && task->expression_type == ExpressionType::SOLVE && 
                    (dynamic_cast<RealSolverTask*>(task.get()) || dynamic_cast<IntegerSolverTask*>(task.get()) || dynamic_cast<RationalSolverTask*>(task.get()));
            }
            ), tasks.end());
    }

    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        switch (result_type)
        {
        case yutovo_service::ResultType::AUTO:
            tasks.emplace_back(new RealSolverTask(id, guid, code_id, ExpressionType::SOLVE, precision, angle_measure, expression, delay));
            tasks.emplace_back(new IntegerSolverTask(id, guid, code_id, ExpressionType::SOLVE, notation, expression, delay));
            tasks.emplace_back(new RationalSolverTask(id, guid, code_id, ExpressionType::SOLVE, expression, delay));
            break;
        case yutovo_service::ResultType::REAL:
            tasks.emplace_back(new RealSolverTask(id, guid, code_id, ExpressionType::SOLVE, precision, angle_measure, expression, delay));
            break;
        case yutovo_service::ResultType::INTEGER:
            tasks.emplace_back(new IntegerSolverTask(id, guid, code_id, ExpressionType::SOLVE, notation, expression, delay));
            break;
        case yutovo_service::ResultType::RATIONAL:
            tasks.emplace_back(new RationalSolverTask(id, guid, code_id, ExpressionType::SOLVE, expression, delay));
            break;
        default:
            break;
        }

        tasks.emplace_back(nullptr);
    }
    next_circle = true;
}

void Solver::SetUserIdentifier(ElementId id, uint code_id, const std::u32string& expression, const uint delay)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
            [id](SolverTaskPtr& task)
            {
                return task && task->id == id && task->expression_type == ExpressionType::USER_SYMBOL && 
                    (dynamic_cast<RealSolverTask*>(task.get()) || dynamic_cast<IntegerSolverTask*>(task.get()) || dynamic_cast<RationalSolverTask*>(task.get()));
            }
            ), tasks.end());
    }

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new RealSolverTask(id, guid, code_id, ExpressionType::USER_SYMBOL, 0, AngleMeasure::RADIAN, expression, delay));
    tasks.emplace_back(new IntegerSolverTask(id, guid, code_id, ExpressionType::USER_SYMBOL, Notation::DECIMAL, expression, delay));
    tasks.emplace_back(new RationalSolverTask(id, guid, code_id, ExpressionType::USER_SYMBOL, expression, delay));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::RemoveIdentifier(ElementId id, uint code_id, const std::u32string& identifier, const uint delay)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
            [id](SolverTaskPtr& task)
            {
                return task && task->id == id && dynamic_cast<RemoveIdentifierSolverTask*>(task.get());
            }
            ), tasks.end());
    }

    std::vector<std::u32string> id_arr;
    boost::split(id_arr, identifier, boost::is_any_of("()"));

    if (!id_arr.empty())
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.emplace_back(new RemoveIdentifierSolverTask(id, guid, code_id, ResultType::REAL, id_arr[0], delay));
        tasks.emplace_back(nullptr);
        tasks.emplace_back(new RemoveIdentifierSolverTask(id, guid, code_id, ResultType::INTEGER, id_arr[0], delay));
        tasks.emplace_back(nullptr);
        tasks.emplace_back(new RemoveIdentifierSolverTask(id, guid, code_id, ResultType::RATIONAL, id_arr[0], delay));
        tasks.emplace_back(nullptr);
        next_circle = true;
    }
}

void Solver::MessageLoop()
{
    WebSocketPtr socket(new WebSocket(document->config, document->window));
    if (!socket->Connect())
        logger->Error("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
    else
        logger->Info("Solver connected to the server: {}:{}", document->config.service_ip, document->config.service_port);

    time_t now = time(0);
    time_t next = now;
    
    std::vector<SolverTaskPtr> temp_tasks;
    int tries = 0;
    while (!exit)
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
        else
        {
            std::this_thread::sleep_for(10ms);
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
                else
                {
                    logger->Info("Solver connected to the server: {}:{}", document->config.service_ip, document->config.service_port);
                    document->ReSolveErrors();
                }
            }
        }

        next = time(0);

        std::unique_lock<std::mutex> lock(tasks_mutex);
        if (temp_tasks.empty())
        {
            while (!tasks.empty() && tasks.front() == nullptr)
                tasks.pop_front();
            if (tasks.empty())
                continue;
            while (!tasks.empty() && tasks.front() != nullptr)
            {
                SolverTaskPtr& t = tasks.front();
                uint64_t now_m = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
                t->delay -= now_m - t->cur_time;
                if (t->delay <= 0)
                {
                    temp_tasks.push_back(t);
                    tasks.pop_front();
                }
                else
                {
                    t->cur_time = now_m;
                    break;
                }
            }
        }

        Result result, cur_result;
        for (SolverTaskPtr t : temp_tasks) //try all variants of parsers until one of them solves
        {
            if (t->Execute(socket, cur_result) && t->expression_type == ExpressionType::SOLVE)
            {
                result = cur_result;
                break;
            }
            if (cur_result.error.error_code == yutovo_service::ErrorCode::OPERATION_ERROR)
            {
                socket.reset(new WebSocket(document->config, document->window)); //recreate the socket
                if (!socket->Connect())
                    logger->Error("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
                else
                {
                    logger->Info("Solver connected to the server: {}:{}", document->config.service_ip, document->config.service_port);
                    document->ReSolveErrors();
                }
                result = cur_result;
                break;
            }
            if (cur_result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR)
            {
                result = cur_result;
                break;
            }
            if (result.error.error_code == ErrorCode::OK && cur_result.error.error_code != ErrorCode::OK)
                result = cur_result;
        }

        if (!result.error.id.empty())
            document->PutResult(result.error.id, result);
        
        if (result.error.error_code != ErrorCode::OPERATION_ERROR && !temp_tasks.empty())
            document->PutResult(temp_tasks[0]->id, result);
        else
        {
            for (SolverTaskPtr t : temp_tasks)
                document->PutResult(t->id, result);
            if (socket->IsOpen() && tries < 1)
            {
                ++tries; //it just connected, try one more
                continue;
            }
        }

        if (result.error.error_code == ErrorCode::SOLVER_RESTARTED_ERROR && !temp_tasks.empty())
            document->ReSolve(temp_tasks[0]->id); //re-solve the expressions above and later this one
        
        temp_tasks.clear();
        
        tries = 0;
    }
}

}
