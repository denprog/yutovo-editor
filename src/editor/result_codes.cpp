#include "result_codes.h"

namespace yutovo
{

IOResult ToIOResult(boost::archive::archive_exception::exception_code code)
{
    using ex = boost::archive::archive_exception;
    switch (code)
    {
    case ex::no_exception:
        return IOResult::Success;
    case ex::other_exception:
        return IOResult::OtherException;
    case ex::unregistered_class:
        return IOResult::UnregisteredClass;
    case ex::invalid_signature:
        return IOResult::InvalidSignature;
    case ex::unsupported_version:
        return IOResult::UnsupportedVersion;
    case ex::pointer_conflict:
        return IOResult::PointerConflict;
    case ex::incompatible_native_format:
        return IOResult::IncompatibleNativeFormat;
    case ex::array_size_too_short:
        return IOResult::ArraySizeTooShort;
    case ex::input_stream_error:
        return IOResult::InputStreamError;
    case ex::invalid_class_name:
        return IOResult::InvalidClassName;
    case ex::unregistered_cast:
        return IOResult::UnregisteredCast;
    case ex::unsupported_class_version:
        return IOResult::UnsupportedClassVersion;
    case ex::multiple_code_instantiation:
        return IOResult::MultipleCodeInstantiation;
    case ex::output_stream_error:
        return IOResult::OutputStreamError;
    }
    return IOResult::OtherException;
}

}
