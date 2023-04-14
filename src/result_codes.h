#ifndef __RESULT_CODES_H__
#define __RESULT_CODES_H__

#include <boost/archive/archive_exception.hpp>
#include <yutovo_service/types.h>
#include <yutovo_calculator/parser_exception.h>
#include <map>
#include <vector>

namespace yutovo
{

enum class IOResult
{
    Success = 0,
    OtherException,    // any exception not listed below
    UnregisteredClass, // attempt to serialize a pointer of an unregistered class
    InvalidSignature,  // first line of archive does not contain expected string
    UnsupportedVersion,// archive created with library version subsequent to this one
    PointerConflict,   // an attempt has been made to directly serialize an object which has already been serialized through a pointer
    IncompatibleNativeFormat, // attempt to read native binary format on incompatible platform
    ArraySizeTooShort,// array being loaded doesn't fit in array allocated
    InputStreamError, // error on input stream
    InvalidClassName, // class name greater than the maximum permitted
    UnregisteredCast,   // base - derived relationship not registered with void_cast_register
    UnsupportedClassVersion, // type saved with a version # greater than the one used by the program
    MultipleCodeInstantiation, // code for implementing serialization for some type has been instantiated in more than one module.
    OutputStreamError // error on input stream
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

IOResult ToIOResult(boost::archive::archive_exception::exception_code code);

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

typedef std::vector<std::string> Dependencies;

struct Result
{
    yutovo_service::ResultType type;
    std::map<std::string, std::string> values;
    Dependencies dependencies;
    Error error;
    std::vector<Warning> warnings;
};

}

#endif
