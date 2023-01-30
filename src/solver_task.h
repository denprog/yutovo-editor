#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include <memory>
#include <zmq.hpp>
#include <map>
#include <yutovo_service/solver.h>

namespace yutovo
{

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

//Expression types
enum class ExpressionType
{
    NONE = 0,
	CALC = 1, //expression for solving
	STANDARD_FUNCTION, //buildin function
	STANDARD_VARIABLE, //buildin variable
	STANDARD_MEASURE, //building measure
	USER_FUNCTION, //user function
	USER_TEMP_VARIABLE, //temporary variable of a function
	USER_SYMBOL, //symbol of user variable or function
	USER_VARIABLE, //user variable
	USER_MEASURE //user measure
};

struct Error
{
    yutovo_service::ErrorCode error_code = yutovo_service::ErrorCode::NONE;
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

    virtual bool Solve(zmq::socket_t& socket, Result& result) = 0;

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

    virtual bool Solve(zmq::socket_t& socket, Result& result);

    uint precision;
    AngleMeasure angle_measure;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Notation _notation, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);

    Notation notation;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);
};

struct ComplexSolverTask : SolverTask
{
    ComplexSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const uint _precision, 
        AngleMeasure _angle_measure, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);

    uint precision;
    AngleMeasure angle_measure;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
