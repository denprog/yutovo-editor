#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include <memory>
#include "rapidjson/document.h"
#include "web_socket.h"
#include "result_codes.h"

namespace yutovo
{

using namespace yutovo_service;

enum class ExpressionType
{
    NONE = 0,
	SOLVE = 1, //expression for solving
	USER_SYMBOL //symbol of user variable or function
};

class Logger;

struct SolverTask
{
    SolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result) = 0;

    bool SendRequest(const rapidjson::Document& json, Result& result, WebSocketPtr& socket);

    void GetResultType(const rapidjson::Document& json, Result& result);
    void GetDependencies(const rapidjson::Document& json, Result& result);

    void FillId(rapidjson::Document& doc);
    void FillUnit(rapidjson::Document& doc, Result& result);
    void FillError(rapidjson::Document& doc, Result& result);

    bool FillRealResult(rapidjson::Document& doc, Result& result);
    bool FillIntegerResult(rapidjson::Document& doc, Result& result);
    bool FillRationalResult(rapidjson::Document& doc, Result& result);

    ElementId id;
    std::string guid;
    uint code_id;
    ExpressionType expression_type;
    std::u32string expression;
    int delay; //in milliseconds
    uint64_t cur_time;
    Logger* logger;
};

struct AutoSolverTask : SolverTask
{
    AutoSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::AutoResult _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::AutoResult config;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RealResult _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RealResult config;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::IntegerResult _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::IntegerResult config;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RationalResult _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RationalResult config;
};

struct RemoveIdentifierSolverTask : SolverTask
{
    RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, const ResultType _result_type, const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    ResultType result_type;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
