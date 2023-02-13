#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include <memory>
#include <zmq.hpp>
#include <map>
#include <yutovo_service/solver.h>

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

struct Error
{
    yutovo_service::ErrorCode error_code = yutovo_service::ErrorCode::OK;
    yutovo_calculator::ParserExceptionCode parser_error_code = yutovo_calculator::ParserExceptionCode::None;
    int solver_error_code = -1;
    int pos = -1;
    int line = -1;
};

enum class WarningCode
{
    NONE = 0
};

struct Warning
{
    WarningCode code;
    int pos;
};

struct Result
{
    yutovo_service::ResultType type;
    std::map<std::string, std::string> values;
    Error error;
    std::vector<Warning> warnings;
};

class Logger;

struct SolverTask
{
    SolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::string& _expression);

    virtual bool Execute(zmq::socket_t& socket, Result& result) = 0;

    bool SendRequest(const rapidjson::Document& json, Result& result, zmq::socket_t& socket);

    ElementId id;
    std::string guid;
    uint code_id;
    ExpressionType expression_type;
    std::string expression;
    Logger* logger;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const uint _precision, 
        AngleMeasure _angle_measure, const std::string& _expression);

    virtual bool Execute(zmq::socket_t& socket, Result& result);

    uint precision;
    AngleMeasure angle_measure;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Notation _notation, const std::string& _expression);

    virtual bool Execute(zmq::socket_t& socket, Result& result);

    Notation notation;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::string& _expression);

    virtual bool Execute(zmq::socket_t& socket, Result& result);
};

struct RemoveIdentifierSolverTask : SolverTask
{
    RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, const ResultType _result_type, const std::string& _expression);

    virtual bool Execute(zmq::socket_t& socket, Result& result);

    ResultType result_type;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
