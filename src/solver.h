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

typedef std::shared_ptr<zmq::socket_t> SocketPtr;

class Solver
{
public:
    Solver(Document* _document);
    ~Solver();

    void Solve(ElementId id, uint code_id, yutovo_service::ResultType result_type, const uint precision, 
        AngleMeasure angle_measure, Notation notation, const std::u32string& expression);
    void SetUserIdentifier(ElementId id, uint code_id, const std::u32string& expression);
    void RemoveIdentifier(ElementId id, uint code_id, const std::u32string& identifier);

private:
    void MessageLoop();
    void CreateSocket(SocketPtr& socket, zmq::context_t& context);

private:
    Document* document;

    std::queue<SolverTaskPtr> tasks;
    std::vector<yutovo_service::ResultType> result_types_seq;

    bool exit = false;

    std::mutex tasks_mutex;
    std::condition_variable_any next_circle;
    std::thread message_loop;

    std::string guid;
};

}

#endif
