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
    logger(Logger::GetInstance(document->config.logs_path, "yutovo_editor", true, true)),
    message_loop(std::thread(&Solver::MessageLoop, this))
{
    guid = boost::uuids::to_string(boost::uuids::random_generator()());
    result_types_seq = {ResultType::REAL, ResultType::INTEGER, ResultType::RATIONAL, ResultType::COMPLEX};
    language = document->config.language;
}

Solver::~Solver()
{
    {
        std::unique_lock<std::mutex> lock(socket_mutex);
        if (socket)
            socket->Close();
    }
    exit = true;
    next_circle = true;
    message_loop.join();
}

void Solver::Solve(const ElementId id, const uint code_id, Config::AutoResultConfig& config, const std::u32string& expression, const uint delay)
{
    EraseSolveTasks(id);

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new AutoSolverTask(id, guid, code_id, ExpressionType::SOLVE, config, expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const ElementId id, const uint code_id, Config::RealResultConfig& config, const std::u32string& expression, const uint delay)
{
    EraseSolveTasks(id);

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new RealSolverTask(id, guid, code_id, ExpressionType::SOLVE, config, expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const ElementId id, const uint code_id, Config::IntegerResultConfig& config, const std::u32string& expression, const uint delay)
{
    EraseSolveTasks(id);

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new IntegerSolverTask(id, guid, code_id, ExpressionType::SOLVE, config, expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const ElementId id, const uint code_id, Config::RationalResultConfig& config, const std::u32string& expression, const uint delay)
{
    EraseSolveTasks(id);

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new RationalSolverTask(id, guid, code_id, ExpressionType::SOLVE, config, expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::Solve(const ElementId id, const uint code_id, Config::ComplexResultConfig& config, const std::u32string& expression, const uint delay)
{
    EraseSolveTasks(id);

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new ComplexSolverTask(id, guid, code_id, ExpressionType::SOLVE, config, expression, delay, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::SetIdentifier(ElementId id, uint code_id, const std::u32string& identifier, const std::u32string& expression, const uint delay)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mutex);
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
            [id](SolverTaskPtr& task)
            {
                return task && task->id == id && task->expression_type == ExpressionType::USER_SYMBOL && (dynamic_cast<AutoSolverTask*>(task.get()));
            }
            ), tasks.end());
    }

    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetIdentifierSolverTask(id, guid, code_id, document, identifier, expression, delay, logger));
    tasks.emplace_back(new ListIdentifiersSolverTask(guid, code_id, document, logger)); //for syntax highlight
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
        tasks.emplace_back(new RemoveIdentifierSolverTask(id, guid, code_id, document, id_arr[0], delay, logger));
        tasks.emplace_back(new ListIdentifiersSolverTask(guid, code_id, document, logger)); //for syntax highlight
        tasks.emplace_back(nullptr);
        next_circle = true;
    }
}

void Solver::SetLocale(const yutovo_calculator::Language _language)
{
    language = _language;
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetLocaleSolverTask(guid, language, document, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::ListIdentifiers(uint code_id)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.emplace_back(new ListIdentifiersSolverTask(guid, code_id, document, logger));
    tasks.emplace_back(nullptr);
    next_circle = true;
}

void Solver::MessageLoop()
{
    bool connected = false;
    bool connection_error = false;

    {
        std::unique_lock<std::mutex> lock(socket_mutex);
#ifdef EMSCRIPTEN
        socket.reset(new WebSocket(document->config, document->window));
#else
        try
        {
            socket.reset(new WebSocket(document->config, document->window));
        }
        catch (boost::system::system_error& ex)
        {
            LOG_ERROR("Error creating socket: {}", ex.code().value());
            throw;
        }
#endif
    }
    if (!socket->Connect() || !socket->IsOpen())
    {
        LOG_ERROR("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
    }
    else
    {
        LOG_INFO("Solver connected to the server: {}:{}, guid:{}", document->config.service_ip, document->config.service_port, guid);
        connected = true;
    }

    time_t now = time(0);
    time_t next = now;
    
    std::vector<SolverTaskPtr> temp_tasks;
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
                    if (next - now >= document->config.reconnect_timeout)
                        break;
                }
                else if (!connected)
                {
                    LOG_INFO("Solver connected to the server: {}:{}, guid:{}", document->config.service_ip, document->config.service_port, guid);
                    connected = true;
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(10ms);
        }

        if (exit)
            return;

        next = time(0);

        if (connection_error)
        {
            if (next - now >= document->config.reconnect_timeout)
            {
                now = time(0);
                {
                    std::unique_lock<std::mutex> lock(socket_mutex);
                    socket.reset(new WebSocket(document->config, document->window)); //recreate the socket
                }
                if (!socket->Connect() || !socket->IsOpen())
                {
                    LOG_ERROR("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
                    continue;
                }
                else
                {
                    LOG_INFO("Solver connected to the server: {}:{}, guid:{}", document->config.service_ip, document->config.service_port, guid);
                    document->ReSolveErrors();
                    connection_error = false;
                }
            }
            else
                continue;
        }

        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            if (temp_tasks.empty())
            {
                while (!tasks.empty() && tasks.front() == nullptr)
                    tasks.pop_front();
                if (tasks.empty())
                {
                    next_circle = false;
                    continue;
                }
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
        }

        Result result;
        int tries = 2;
        for (int i = 0; i < temp_tasks.size();)
        {
            SolverTaskPtr t = temp_tasks[i];
            if (!t->Execute(socket, result))
            {
                if (result.error.error_code == yutovo_service::ErrorCode::OPERATION_ERROR)
                {
                    {
                        std::unique_lock<std::mutex> lock(socket_mutex);
                        socket.reset(new WebSocket(document->config, document->window)); //recreate the socket
                    }
                    if (!socket->IsOpen())
                    {
                        LOG_ERROR("Error connecting to the server: {}:{}", document->config.service_ip, document->config.service_port);
                    }
                    else
                    {
                        LOG_INFO("Solver connected to the server: {}:{}, guid:{}", document->config.service_ip, document->config.service_port, guid);
                        document->ReSolveErrors();
                    }
                    if (socket->IsOpen() && tries-- > 0)
                    {
                        --i; //it's just connected, try one more
                        continue;
                    }
                }
            }

            if (t->expression_type == ExpressionType::USER_SYMBOL)
                document->window->OnIdentifierChanged(t->id);

            document->PutResult(t->id, result);
            if (result.error.error_code == yutovo_service::ErrorCode::SOLVER_RESTARTED_ERROR)
            {
                document->ReSolve(t->id); //re-solve the expression
                std::unique_lock<std::mutex> lock(tasks_mutex);
                tasks.emplace_back(new SetLocaleSolverTask(guid, language, document, logger));
                tasks.emplace_back(new ListIdentifiersSolverTask(guid, t->code_id, document, logger)); //for syntax highlight
            }

            if (result.error.error_code != yutovo_service::ErrorCode::OPERATION_ERROR)
                temp_tasks.erase(temp_tasks.begin() + i);
            else
            {
                connection_error = true;
                ++i;
            }
        }
    }
}

void Solver::EraseSolveTasks(const ElementId id)
{
    std::unique_lock<std::mutex> lock(tasks_mutex);
    tasks.erase(std::remove_if(tasks.begin(), tasks.end(), 
        [id](SolverTaskPtr& task)
        {
            return task && task->id == id && task->expression_type == ExpressionType::SOLVE && 
                (dynamic_cast<RealSolverTask*>(task.get()) || dynamic_cast<IntegerSolverTask*>(task.get()) || 
                dynamic_cast<RationalSolverTask*>(task.get()) || dynamic_cast<AutoSolverTask*>(task.get()));
        }
        ), tasks.end());
}

}
