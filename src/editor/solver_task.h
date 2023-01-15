#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include "logger.h"
#include <memory>
#include <zmq.hpp>
#include <map>

namespace yutovo
{

//Returned result type
enum class ResultType
{
	NONE = 0, 
	REAL, 
	INTEGER, 
	RATIONAL, 
	COMPLEX, 
	AUTO
};

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

enum class ErrorCode
{
    NONE = 0,
    JSON_ERROR,
    SOLVER_ERROR
};

struct Error
{
    ErrorCode error_code = ErrorCode::NONE;
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
    ResultType type;
    std::map<std::string, std::string> values;
    Error error;
    std::vector<Warning> warnings;
};

struct SolverTask
{
    SolverTask(ElementId _id, ExpressionType _expression_type, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result) = 0;

    ElementId id;
    ExpressionType expression_type;
    std::string expression;
    Logger* logger;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(ElementId _id, ExpressionType _expression_type, const uint _precision, AngleMeasure _angle_measure, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);

    uint precision;
    AngleMeasure angle_measure;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, ExpressionType _expression_type, Notation _notation, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);

    Notation notation;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, ExpressionType _expression_type, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);
};

struct ComplexSolverTask : SolverTask
{
    ComplexSolverTask(ElementId _id, ExpressionType _expression_type, const uint _precision, AngleMeasure _angle_measure, const std::string& _expression);

    virtual bool Solve(zmq::socket_t& socket, Result& result);

    uint precision;
    AngleMeasure angle_measure;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
