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

    void Solve(ElementId id, ExpressionType expression_type, ResultType result_type, const uint precision, AngleMeasure angle_measure, 
        Notation notation, const std::string& expression);

    void OnResult(Result result);

private:
    void MessageLoop();

private:
    Document* document;

    std::queue<SolverTaskPtr> tasks;
    std::vector<ResultType> result_types_seq;

    bool exit = false;

    std::mutex tasks_mutex;
    std::condition_variable_any next_circle;
    std::thread message_loop;
};

}

#endif
