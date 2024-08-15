#ifndef __RESULT_CODES_H__
#define __RESULT_CODES_H__

#include <yutovo_solver/types.h>
#include <yutovo_calculator/parser_exception.h>
#include <yutovo_calculator/unit.h>
#include <yutovo_calculator/math_helper.h>
#include <map>
#include <vector>
#include "caret_state.h"

namespace yutovo
{

enum class IOResult
{
    Success = 0,
    OtherException,    //any exception not listed below
    UnsupportedVersion, //version is unsupported
    InputStreamError, //error on input stream
    OutputStreamError, //error on output stream
    ConnectionError //error of connecting
};

enum class CopyResult
{
    Success = 0,
    EmptySelection,
    CopyError
};

enum class PasteResult
{
    Success = 0,
    EmptyBuffer,
    PasteError
};

struct Error
{
    yutovo_solver::ErrorCode error_code = yutovo_solver::ErrorCode::OK;
    yutovo_calculator::ParserExceptionCode parser_error_code = yutovo_calculator::ParserExceptionCode::None;
    int solver_error_code = -1;
    ElementId id;
    int pos = -1;
    int line = -1;
    std::u32string description;
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

typedef std::vector<std::string> Dependencies;
typedef std::map<std::string, std::string> Value;

struct Result
{
    yutovo_solver::ResultType type = yutovo_solver::ResultType::NONE;
    std::vector<Value> values;
    yutovo_calculator::AngleMeasure angle_measure = yutovo_calculator::AngleMeasure::None;
    yutovo_calculator::Notation notation = yutovo_calculator::Notation::None;
    yutovo_calculator::Unit unit;
    std::vector<yutovo_calculator::Unit> cast_units;
    Dependencies dependencies;
    Error error;
    std::vector<Warning> warnings;
};

}

#endif
