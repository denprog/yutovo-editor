#include "result_codes.h"

namespace yutovo
{

std::string Result::ToString()
{
    std::string r = "result_type:" + ResultTypeToString(type) + ",";
    size_t i = 0;
    r += "values:{";
    for (auto& v : values)
    {
        if (i++ > 0)
            r += ",";
        r += v["mantissa"] + "E" + v["exponent"];
    }
    r += "},";
    r += "angle_measure:" + AngleMeasureToString(angle_measure) + ",";
    r += "notation:" + NotationToString(notation) + ",";
    if (!unit.IsEmpty())
        r += "unit:" + ToBasicString(unit.ToString()) + ",";
    if (error.error_code != yutovo_solver::ErrorCode::OK)
        r += "error:" + ErrorCodeToString(error.error_code) + ",";
    if (error.parser_error_code != yutovo_calculator::ParserExceptionCode::None)
    {
        r += "parser_error:" + ErrorCodeToString(error.parser_error_code) + ",pos:" + std::to_string(error.pos) + ",size:" + std::to_string(error.size) + 
            ",line:" + std::to_string(error.line) + ",";
    }
    if (!error.description.empty())
        r += "description:" + ToBasicString(error.description);

    return r;
}

}
