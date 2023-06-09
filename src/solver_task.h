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

    void GetDependencies(const rapidjson::Document& json, Result& result);

    void FillId(rapidjson::Document& doc);
    void FillError(rapidjson::Document& doc, Result& result);

    ElementId id;
    std::string guid;
    uint code_id;
    ExpressionType expression_type;
    std::u32string expression;
    int delay; //in milliseconds
    uint64_t cur_time;
    Logger* logger;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const uint _precision, const uint _exp, 
        AngleMeasure _default_angle_measure, AngleMeasure _result_angle_measure, const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    uint precision;
    uint exp;
    AngleMeasure default_angle_measure;
    AngleMeasure result_angle_measure;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Notation _result_notation, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Notation result_notation;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, FractionForm _fraction_form, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    FractionForm fraction_form;
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
