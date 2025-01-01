#include "solver_task.h"
#include "rapidjson/writer.h"
#include <yutovo_logger/logger.h>
#include "util.h"
#include "document.h"

namespace yutovo
{

using namespace yutovo_solver;
using namespace std::chrono;

//SolverTask

SolverTask::SolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    ExpressionType _expression_type, const std::u32string& _expression, const uint _delay, Logger* _logger) :
    id(_id),
    solver_guid(_solver_guid),
    task_guid(_task_guid),
    code_id(_code_id),
    expression_type(_expression_type),
    expression(_expression),
    delay(_delay),
    logger(_logger)
{
    cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

SolverTask::SolverTask(const LogicalId& _id, const std::string& _solver_guid, uint _code_id, 
    ExpressionType _expression_type, const std::u32string& _expression, const uint _delay, Logger* _logger) : 
    id(_id),
    solver_guid(_solver_guid),
    code_id(_code_id),
    expression_type(_expression_type),
    expression(_expression),
    delay(_delay),
    logger(_logger)
{
    cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

SolverTask::SolverTask(const std::string& _solver_guid, Logger* _logger) :
    solver_guid(_solver_guid),
    logger(_logger)
{
    cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

SolverTask::SolverTask(const LogicalId& _id, const std::string& _solver_guid, uint _code_id, Logger* _logger) :
    id(_id),
    solver_guid(_solver_guid),
    code_id(_code_id),
    logger(_logger)
{
    cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

bool SolverTask::SendRequest(const rapidjson::Document& json, Result& result, WebSocketPtr& socket)
{
    socket->Reset();

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    std::string str = buffer.GetString();
    return socket->Send(str, result);
}

void SolverTask::AddUnit(rapidjson::Document& json, const yutovo_calculator::Unit& unit)
{
    if (unit.IsEmpty())
        return;
    
    auto& alloc = json.GetAllocator();
    rapidjson::Value _unit;
    _unit.SetObject();
    rapidjson::Value d(rapidjson::kArrayType);
    for (auto& u : unit.unit)
    {
        rapidjson::Value _u;
        _u.SetObject();
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(u.first)).c_str(), alloc);
        _u.AddMember("name", s, alloc);
        if (u.second != 1)
            _u.AddMember("power", u.second, alloc);
        d.PushBack(_u, alloc);
    }
    if (unit.system != U"")
    {
        rapidjson::Value s((boost::locale::conv::utf_to_utf<char>(unit.system)).c_str(), alloc);
        _unit.AddMember("system", s, alloc);
    }
    _unit.AddMember("value", d, alloc);
    json.AddMember("unit", _unit, alloc);
}

void SolverTask::GetResultType(const rapidjson::Document& json, Result& result)
{
    if (json.HasMember("result_type") && json["result_type"].IsInt())
        result.type = (yutovo_solver::ResultType)json["result_type"].GetInt();
}

void SolverTask::GetDependencies(const rapidjson::Document& json, Result& result)
{
    if (json.IsObject() && json.HasMember("dependencies") && json["dependencies"].IsArray())
    {
        rapidjson::GenericArray d = json["dependencies"].GetArray();
        for (rapidjson::SizeType i = 0; i < d.Size(); ++i)
        {
            if (d[i].IsString())
                result.dependencies.push_back(d[i].GetString());
        }
    }
}

void SolverTask::FillId(rapidjson::Document& doc)
{
    auto& alloc = doc.GetAllocator();
    rapidjson::Value d(rapidjson::kArrayType);
    for (int i : id)
        d.PushBack(i, alloc);
    doc.AddMember("id", d, alloc);
}

void SolverTask::FillUnit(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("unit") || !doc["unit"].IsObject())
        return;
    
    yutovo_calculator::Unit unit;
    rapidjson::Value _unit = doc["unit"].GetObject();
    if (!_unit.HasMember("value") || !_unit["value"].IsArray())
        return;
    if (_unit.HasMember("system"))
        unit.system = ToUtfString(_unit["system"].GetString());
    rapidjson::GenericArray arr = _unit["value"].GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return;
        rapidjson::Value u = arr[i].GetObject();
        std::u32string name;
        int power = 1;
        if (!u.HasMember("name") && !u["name"].IsString())
            return;
        name = ToUtfString(u["name"].GetString());
        if (u.HasMember("power") && u["power"].IsInt())
            power = u["power"].GetInt();
        unit.unit.push_back(std::make_pair(name, power));
    }
    result.unit = unit;
}

void SolverTask::FillCastUnits(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("cast_units") || !doc["cast_units"].IsArray())
        return;

    rapidjson::GenericArray cast_units = doc["cast_units"].GetArray();
    for (rapidjson::SizeType i = 0; i < cast_units.Size(); ++i)
    {
        if (!cast_units[i].IsObject())
            return;
        
        rapidjson::Value s_arr = cast_units[i].GetObject();
        if (!s_arr.HasMember("system") || !s_arr["system"].IsString())
            return;
        auto s = ToUtfString(s_arr["system"].GetString());
        if (!s_arr.HasMember("units") || !s_arr["units"].IsArray())
            return;
        rapidjson::GenericArray units_arr = s_arr["units"].GetArray();
        for (rapidjson::SizeType j = 0; j < units_arr.Size(); ++j)
        {
            if (!units_arr[j].IsArray())
                return;
            
            yutovo_calculator::Unit unit;
            rapidjson::Value u_arr = units_arr[j].GetArray();
            for (rapidjson::SizeType k = 0; k < u_arr.Size(); ++k)
            {
                if (!u_arr[k].IsObject())
                    return;
                rapidjson::Value u = u_arr[k].GetObject();

                std::u32string name;
                int power = 1;
                if (u.HasMember("name") && u["name"].IsString())
                    name = ToUtfString(u["name"].GetString());
                if (u.HasMember("power") && u["power"].IsInt())
                    power = u["power"].GetInt();
                unit.unit.push_back(std::make_pair(name, power));
                unit.system = s;
            }
            result.cast_units.push_back(unit);
        }
    }
}

void SolverTask::FillError(rapidjson::Document& doc, Result& result)
{
    if (doc["error"].IsObject())
    {
        rapidjson::Value error = doc["error"].GetObject();
        if (error.HasMember("error_code") && error["error_code"].IsInt())
            result.error.error_code = (ErrorCode)error["error_code"].GetInt();
        if (error.HasMember("parser_error_code") && error["parser_error_code"].IsInt())
            result.error.parser_error_code = (yutovo_calculator::ParserExceptionCode)error["parser_error_code"].GetInt();
        if (result.error.error_code != ErrorCode::SOLVER_RESTARTED_ERROR && result.error.error_code != ErrorCode::OK)
            LOG_ERROR("Solver error: {}", ErrorCodeToString(result.error.error_code));
        if (error.HasMember("pos") && error["pos"].IsInt())
            result.error.pos = error["pos"].GetInt();
        if (error.HasMember("size") && error["size"].IsInt())
            result.error.size = error["size"].GetInt();
        if (error.HasMember("line") && error["line"].IsInt())
            result.error.line = error["line"].GetInt();
        if (error.HasMember("description") && error["description"].IsString())
            result.error.description = ToUtfString(error["description"].GetString());

        if (error.HasMember("id") && error["id"].IsArray())
        {
            rapidjson::GenericArray arr = error["id"].GetArray();
            result.error.id.clear();
            for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
            {
                if (!arr[i].IsInt())
                    return;
                result.error.id.push_back(arr[i].GetInt());
            }
        }
        return;
    }
    if (result.error.error_code != ErrorCode::SOLVER_RESTARTED_ERROR && result.error.error_code != ErrorCode::OK)
        LOG_ERROR("Solver error: {}", (int)result.error.error_code);
    result.error.error_code = ErrorCode::PARSER_ERROR;
}

bool SolverTask::FillRealResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("mantissa") || !doc["mantissa"].IsString())
    {
        LOG_ERROR("mantissa error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    Value value;
    value["mantissa"] = doc["mantissa"].GetString();
    if (doc.HasMember("exponent") && doc["exponent"].IsString())
        value["exponent"] = doc["exponent"].GetString();
    result.values.push_back(value);
    if (doc.HasMember("angle_measure") && doc["angle_measure"].IsInt())
        result.angle_measure = (AngleMeasure)doc["angle_measure"].GetInt();

    FillUnit(doc, result);
    FillCastUnits(doc, result);
    return true;
}

bool SolverTask::FillIntegerResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("value") || !doc["value"].IsString())
    {
        LOG_ERROR("value error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    Value value;
    value["value"] = doc["value"].GetString();
    result.values.push_back(value);
    if (doc.HasMember("notation") && doc["notation"].IsInt())
        result.notation = (Notation)doc["notation"].GetInt();
    return true;
}

bool SolverTask::FillRationalResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("numerator") || !doc["numerator"].IsString() || !doc.HasMember("denomerator") || !doc["denomerator"].IsString())
    {
        LOG_ERROR("value error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    Value value;
    if (doc.HasMember("integer") && doc["integer"].IsString())
        value["integer"] = doc["integer"].GetString();
    value["numerator"] = doc["numerator"].GetString();
    value["denomerator"] = doc["denomerator"].GetString();
    result.values.push_back(value);

    FillUnit(doc, result);
    FillCastUnits(doc, result);
    return true;
}

bool SolverTask::FillComplexResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("results"))
    {
        LOG_ERROR("Result error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    auto fill_exp = [&](rapidjson::Value& val, const std::string& param, const std::string& mantissa, const std::string& exponent)
    {
        if (!val[param.c_str()].IsObject())
        {
            LOG_ERROR("Result error");
            result.error.error_code = ErrorCode::JSON_ERROR;
            return false;
        }
        rapidjson::Value p = val[param.c_str()].GetObject();
        if (!p.HasMember("mantissa") || !p["mantissa"].IsString())
        {
            LOG_ERROR("Mantissa error");
            result.error.error_code = ErrorCode::JSON_ERROR;
            return false;
        }

        Value& value = result.values[result.values.size() - 1];
        value[mantissa] = p["mantissa"].GetString();
        if (p.HasMember("exponent") && p["exponent"].IsString())
            value[exponent] = p["exponent"].GetString();
        return true;
    };

    rapidjson::GenericArray arr = doc["results"].GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
        {
            LOG_ERROR("Result error");
            result.error.error_code = ErrorCode::JSON_ERROR;
            return false;
        }
        auto pos = std::to_string(i);
        result.values.push_back(Value{});
        rapidjson::Value r = arr[i].GetObject();
        if (r.HasMember("module"))
        {
            if (!fill_exp(r, "module", "module_mantissa", "module_exponent"))
                return false;
            if (!fill_exp(r, "argument", "argument_mantissa", "argument_exponent"))
                return false;
        }
        else
        {
            if (r.HasMember("re"))
            {
                if (!fill_exp(r, "re", "re_mantissa", "re_exponent"))
                    return false;
            }
            if (r.HasMember("im"))
            {
                if (!fill_exp(r, "im", "im_mantissa", "im_exponent"))
                    return false;
            }
        }

        if (r.HasMember("angle_measure") && r["angle_measure"].IsInt())
            result.angle_measure = (AngleMeasure)r["angle_measure"].GetInt();
    }

    return true;
}

//AutoSolverTask

AutoSolverTask::AutoSolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    ExpressionType _expression_type, Config::AutoResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger) :
    SolverTask(_id, _solver_guid, _task_guid, _code_id, _expression_type, _expression, _delay, _logger),
    config(_config)
{
}

bool AutoSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("timestamp", cur_time, alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::AUTO, alloc);
    std::string s = ToBasicString(expression);
    doc.AddMember("expression", rapidjson::StringRef(s.c_str()), alloc);

    //auto config
    if (expression_type != ExpressionType::USER_SYMBOL) //for user symbols solve with all the result types
    {
        rapidjson::Value d(rapidjson::kArrayType);
        for (auto t : config.results_order)
            d.PushBack((int)t, alloc);
        doc.AddMember("results_order", d, alloc);
    }

    //real config
    doc.AddMember("real_precision", config.real_result.precision, alloc);
    doc.AddMember("real_default_angle_measure", (int)config.real_result.default_angle_measure, alloc);
    doc.AddMember("real_result_angle_measure", (int)config.real_result.result_angle_measure, alloc);
    doc.AddMember("real_exponent_size", config.real_result.exp, alloc);

    //integer config
    doc.AddMember("result_notation", (int)config.integer_result.result_notation, alloc);
    doc.AddMember("default_notation", (int)config.integer_result.default_notation, alloc);

    //rational config
    doc.AddMember("fraction_form", (int)config.rational_result.fraction_form, alloc);

    //complex config
    doc.AddMember("complex_precision", config.complex_result.precision, alloc);
    doc.AddMember("complex_default_angle_measure", (int)config.complex_result.default_angle_measure, alloc);
    doc.AddMember("complex_result_angle_measure", (int)config.complex_result.result_angle_measure, alloc);
    doc.AddMember("complex_exponent_size", config.complex_result.exp, alloc);
    doc.AddMember("form", (int)config.complex_result.form, alloc);
    doc.AddMember("max_count", config.complex_result.max_count, alloc);

    AddUnit(doc, config.real_result.unit);

    if (!SendRequest(doc, result, socket))
        return false;

    std::string json;
    if (!socket->Receive(json, result))
        return false;

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    GetDependencies(doc, result);

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    GetResultType(doc, result);
    switch (result.type)
    {
    case ResultType::REAL:
        return FillRealResult(doc, result);
    case ResultType::INTEGER:
        return FillIntegerResult(doc, result);
    case ResultType::RATIONAL:
        return FillRationalResult(doc, result);
    case ResultType::COMPLEX:
        return FillComplexResult(doc, result);
    default:
        return false;
    }
}

//RealSolverTask

RealSolverTask::RealSolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    ExpressionType _expression_type, Config::RealResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger) :
    SolverTask(_id, _solver_guid, _task_guid, _code_id, _expression_type, _expression, _delay, _logger),
    config(_config)
{
}

bool RealSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("timestamp", cur_time, alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::REAL, alloc);
    std::string s = ToBasicString(expression);
    doc.AddMember("expression", rapidjson::StringRef(s.c_str()), alloc);
    doc.AddMember("real_precision", config.precision, alloc);
    doc.AddMember("real_default_angle_measure", (int)config.default_angle_measure, alloc);
    doc.AddMember("real_result_angle_measure", (int)config.result_angle_measure, alloc);
    doc.AddMember("real_exponent_size", config.exp, alloc);

    AddUnit(doc, config.unit);

    if (!SendRequest(doc, result, socket))
        return false;

    std::string json;
    if (!socket->Receive(json, result))
        return false;

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    GetDependencies(doc, result);

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    GetResultType(doc, result);
    if (result.type != ResultType::REAL)
        return false;

    return FillRealResult(doc, result);
}

//IntegerSolverTask

IntegerSolverTask::IntegerSolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    ExpressionType _expression_type, Config::IntegerResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger) :
    SolverTask(_id, _solver_guid, _task_guid, _code_id, _expression_type, _expression, _delay, _logger),
    config(_config)
{
}

bool IntegerSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("timestamp", cur_time, alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::INTEGER, alloc);
    doc.AddMember("result_notation", (int)config.result_notation, alloc);
    doc.AddMember("default_notation", (int)config.default_notation, alloc);
    std::string s = ToBasicString(expression);
    doc.AddMember("expression", rapidjson::StringRef(s.c_str()), alloc);

    if (!SendRequest(doc, result, socket))
        return false;

    //reply
    std::string json;
    if (!socket->Receive(json, result))
        return false;
    
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    GetDependencies(doc, result);

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    GetResultType(doc, result);
    if (result.type != ResultType::INTEGER)
        return false;

    return FillIntegerResult(doc, result);
}

//RationalSolverTask

RationalSolverTask::RationalSolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    ExpressionType _expression_type, Config::RationalResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger) :
    SolverTask(_id, _solver_guid, _task_guid, _code_id, _expression_type, _expression, _delay, _logger),
    config(_config)
{
}

bool RationalSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("timestamp", cur_time, alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::RATIONAL, alloc);
    doc.AddMember("fraction_form", (int)config.fraction_form, alloc);
    std::string s = ToBasicString(expression);
    doc.AddMember("expression", rapidjson::StringRef(s.c_str()), alloc);

    AddUnit(doc, config.unit);

    if (!SendRequest(doc, result, socket))
        return false;

    //reply
    std::string json;
    if (!socket->Receive(json, result))
        return false;
    
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    GetDependencies(doc, result);

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    GetResultType(doc, result);
    if (result.type != ResultType::RATIONAL)
        return false;

    return FillRationalResult(doc, result);
}

//ComplexSolverTask

ComplexSolverTask::ComplexSolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    ExpressionType _expression_type, Config::ComplexResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger) :
    SolverTask(_id, _solver_guid, _task_guid, _code_id, _expression_type, _expression, _delay, _logger),
    config(_config)
{
}

bool ComplexSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("timestamp", cur_time, alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::COMPLEX, alloc);
    std::string s = ToBasicString(expression);
    doc.AddMember("expression", rapidjson::StringRef(s.c_str()), alloc);
    doc.AddMember("complex_precision", config.precision, alloc);
    doc.AddMember("complex_default_angle_measure", (int)config.default_angle_measure, alloc);
    doc.AddMember("complex_result_angle_measure", (int)config.result_angle_measure, alloc);
    doc.AddMember("complex_exponent_size", config.exp, alloc);
    doc.AddMember("form", (int)config.form, alloc);
    doc.AddMember("max_count", config.max_count, alloc);

    if (!SendRequest(doc, result, socket))
        return false;

    std::string json;
    if (!socket->Receive(json, result))
        return false;

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    GetDependencies(doc, result);

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    GetResultType(doc, result);
    if (result.type != ResultType::COMPLEX)
        return false;

    return FillComplexResult(doc, result);
}

//BreakSolverTask

BreakSolverTask::BreakSolverTask(const LogicalId& _id, const std::string& _solver_guid, uint _code_id, Logger* _logger) :
    SolverTask(_id, _solver_guid, _code_id, _logger)
{
    delay = 0;
}

bool BreakSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "BREAK_SOLVING", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("timestamp", cur_time, alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);

    if (!SendRequest(doc, result, socket))
        return false;

    std::string json;
    if (!socket->Receive(json, result))
        return false;

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    return true;
}

//SetIdentifierSolverTask

SetIdentifierSolverTask::SetIdentifierSolverTask(const LogicalId& _id, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
    Document* _document, const std::u32string& _identifier, const std::u32string& _expression, const uint _delay, Logger* _logger) : 
    AutoSolverTask(_id, _solver_guid, _task_guid, _code_id, ExpressionType::USER_SYMBOL, Config::AutoResultConfig{}, _expression, _delay, _logger),
    document(_document),
    identifier(_identifier)
{
}

bool SetIdentifierSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    if (!AutoSolverTask::Execute(socket, result))
        return false;
    document->ReSolveDependencies(id, identifier);
    return true;
}

//RemoveIdentifierSolverTask

RemoveIdentifierSolverTask::RemoveIdentifierSolverTask(const LogicalId& _id, const std::string& _solver_guid, uint _code_id, Document* _document, 
    const std::u32string& _expression, const uint _delay, Logger* _logger) :
    SolverTask(_id, _solver_guid, _code_id, ExpressionType::USER_SYMBOL, _expression, _delay, _logger),
    document(_document)
{
}

bool RemoveIdentifierSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "REMOVE_IDENTIFIER", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("expression", rapidjson::StringRef(ToBasicString(expression).c_str()), alloc);

    if (!SendRequest(doc, result, socket))
        return false;
    
    //reply
    std::string json;
    if (!socket->Receive(json, result))
        return false;
    
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    document->ReSolveDependencies(id, expression);

    return true;
}

//RemoveUserIdentifiersSolverTask

RemoveUserIdentifiersSolverTask::RemoveUserIdentifiersSolverTask(const std::string& _solver_guid, Logger* _logger) :
    SolverTask(_solver_guid, _logger)
{
}

bool RemoveUserIdentifiersSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "REMOVE_USER_IDENTIFIERS", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);

    if (!SendRequest(doc, result, socket))
        return false;
    
    //reply
    std::string json;
    if (!socket->Receive(json, result))
        return false;
    
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("error"))
    {
        FillError(doc, result);
        return false;
    }

    return true;
}

//SetLocaleSolverTask

SetLocaleSolverTask::SetLocaleSolverTask(const std::string& _solver_guid, const yutovo_calculator::Language _language, Document* _document, Logger* _logger) :
    SolverTask(_solver_guid, _logger),
    language(_language),
    document(_document)
{
}

bool SetLocaleSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SET_LOCALE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    doc.AddMember("language", (int)language, alloc);

    if (!SendRequest(doc, result, socket))
        return false;
    
    //reply
    std::string json;
    if (!socket->Receive(json, result))
        return false;
    
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("error"))
    {
        LOG_ERROR("Error setting language");
        return false;
    }

    document->window->OnLanguageChanged(language);

    return true;
}

//ListIdentifiersSolverTask

ListIdentifiersSolverTask::ListIdentifiersSolverTask(const std::string& _solver_guid, const uint _code_id, Document* _document, Logger* _logger) :
    SolverTask(_solver_guid, _logger),
    code_id(_code_id),
    document(_document)
{
}

bool ListIdentifiersSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "LIST_IDENTIFIERS", alloc);
    doc.AddMember("guid", rapidjson::StringRef(solver_guid.c_str()), alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);

    if (!SendRequest(doc, result, socket))
        return false;
    
    //reply
    std::string json;
    if (!socket->Receive(json, result))
        return false;
    
#ifndef REMOTE_SOLVER
    document->window->OnIdentifiersReceived(json);
#endif

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        LOG_ERROR("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("error"))
    {
        LOG_ERROR("Error getting identifiers");
        return false;
    }

    std::vector<std::string> variables;
    std::vector<std::string> functions;
    std::vector<std::string> units;

    if (doc.HasMember("Functions") && doc["Functions"].IsArray())
    {
        rapidjson::GenericArray arr = doc["Functions"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            if (arr[i].IsObject())
            {
                rapidjson::Value obj = arr[i].GetObject();
                std::u32string name;
                if (obj.HasMember("name") && obj["name"].IsString())
                    functions.push_back(obj["name"].GetString());
            }
        }
    }

    if (doc.HasMember("Variables") && doc["Variables"].IsArray())
    {
        rapidjson::GenericArray arr = doc["Variables"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            if (arr[i].IsObject())
            {
                rapidjson::Value obj = arr[i].GetObject();
                std::u32string name;
                if (obj.HasMember("name") && obj["name"].IsString())
                    variables.push_back(obj["name"].GetString());
            }
        }
    }

    if (doc.HasMember("Units") && doc["Units"].IsArray())
    {
        rapidjson::GenericArray arr = doc["Units"].GetArray();
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
        {
            if (arr[i].IsObject())
            {
                for (auto& system : arr[i].GetObject())
                {
                    if (system.value.IsArray())
                    {
                        for (rapidjson::SizeType j = 0; j < system.value.Size(); ++j)
                        {
                            if (system.value[j].IsObject())
                            {
                                for (auto& category : system.value[j].GetObject())
                                {
                                    if (category.value.IsArray())
                                    {
                                        for (rapidjson::SizeType k = 0; k < category.value.Size(); ++k)
                                        {
                                            rapidjson::Value obj = category.value[k].GetObject();
                                            std::u32string name;
                                            if (obj.HasMember("name") && obj["name"].IsString())
                                                units.push_back(obj["name"].GetString());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    document->SetIdentifiers(code_id, variables, functions, units);

    return true;
}

}
