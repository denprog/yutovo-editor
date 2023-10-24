#ifndef __SOLVER_H__
#define __SOLVER_H__

#include "solver_task.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <yutovo_service/types.h>
#include <yutovo_calculator/math_helper.h>

namespace yutovo
{

class Document;

class Solver
{
public:
    Solver(Document* _document);
    ~Solver();

    void Solve(const ElementId id, const uint code_id, Config::AutoResultConfig& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::RealResultConfig& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::IntegerResultConfig& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::RationalResultConfig& config, const std::u32string& expression, const uint delay);
    void Solve(const ElementId id, const uint code_id, Config::ComplexResultConfig& config, const std::u32string& expression, const uint delay);

    void SetUserIdentifier(ElementId id, uint code_id, const std::u32string& expression, const uint delay);
    void RemoveIdentifier(ElementId id, uint code_id, const std::u32string& identifier, const uint delay);
    void SetLanguage(const yutovo_calculator::Language language);

private:
    void MessageLoop();

    void EraseSolveTasks(const ElementId id);

public:
    std::string guid;
    
private:
    Document* document;

    std::deque<SolverTaskPtr> tasks;
    std::vector<yutovo_service::ResultType> result_types_seq;

    Logger* logger;

    bool exit = false;

    std::mutex tasks_mutex;
    std::atomic_bool next_circle;
    std::thread message_loop;
};

}

#endif
