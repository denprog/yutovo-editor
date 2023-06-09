#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "solver_task.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace yutovo
{

class Document;

class Solver
{
public:
    Solver(Document* _document);
    ~Solver();

    void Solve(const ElementId id, const uint code_id, Config::AutoResult& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::RealResult& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::IntegerResult& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::RationalResult& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::ComplexResult& config, const std::u32string& expression, const uint delay);

    void SetUserIdentifier(ElementId id, uint code_id, const std::u32string& expression, const uint delay);
    void RemoveIdentifier(ElementId id, uint code_id, const std::u32string& identifier, const uint delay);

private:
    void MessageLoop();

    void EraseSolveTasks(const ElementId id);

private:
    Document* document;

    std::deque<SolverTaskPtr> tasks;
    std::vector<yutovo_service::ResultType> result_types_seq;

    Logger* logger;

    bool exit = false;

    std::mutex tasks_mutex;
    std::atomic_bool next_circle;
    std::thread message_loop;

    std::string guid;

    const int reconnect_period = 2; //seconds
};

}

#endif
