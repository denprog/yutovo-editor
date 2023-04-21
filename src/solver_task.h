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

enum class AngleMeasure
{
    RADIAN = 1,
    DEGREE,
    GRAD
};

enum class Notation
{
    BINARY = 1,
    OCTAL,
    DECIMAL,
    HEXADECIMAL
};

enum class ExpressionType
{
    NONE = 0,
	SOLVE = 1, //expression for solving
	USER_SYMBOL //symbol of user variable or function
};

class Logger;

struct SolverTask
{
    SolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::u32string& _expression);

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
    Logger* logger;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const uint _precision, 
        AngleMeasure _angle_measure, const std::u32string& _expression);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    uint precision;
    AngleMeasure angle_measure;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Notation _notation, 
        const std::u32string& _expression);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Notation notation;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::u32string& _expression);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct RemoveIdentifierSolverTask : SolverTask
{
    RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, const ResultType _result_type, const std::u32string& _expression);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    ResultType result_type;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
