#ifndef __RESULT_CODES_H__
#define __RESULT_CODES_H__

#include <boost/archive/archive_exception.hpp>

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

IOResult ToIOResult(boost::archive::archive_exception::exception_code code);

}

#endif
