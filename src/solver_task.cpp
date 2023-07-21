#include "solver_task.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <yutovo_logger/logger.h>
#include "util.h"

namespace yutovo
{

using namespace yutovo_service;
using namespace std::chrono;

//SolverTask

SolverTask::SolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::u32string& _expression, const uint _delay) :
    id(_id),
    guid(_guid),
    code_id(_code_id),
    expression_type(_expression_type),
    expression(_expression),
    delay(_delay),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true))
{
    cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

bool SolverTask::SendRequest(const rapidjson::Document& json, Result& result, WebSocketPtr& socket)
{
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
        result.type = (yutovo_service::ResultType)json["result_type"].GetInt();
}

void SolverTask::GetDependencies(const rapidjson::Document& json, Result& result)
{
    if (json.HasMember("dependencies"))
    {
        if (json["dependencies"].IsArray())
        {
            rapidjson::GenericArray d = json["dependencies"].GetArray();
            for (rapidjson::SizeType i = 0; i < d.Size(); ++i)
            {
                if (d[i].IsString())
                    result.dependencies.push_back(d[i].GetString());
            }
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
            logger->Error("Solver error: {}", ErrorCodeToString(result.error.error_code));
        if (error.HasMember("pos") && error["pos"].IsInt())
            result.error.pos = error["pos"].GetInt();
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
        logger->Error("Solver error: {}", (int)result.error.error_code);
    result.error.error_code = ErrorCode::PARSER_ERROR;
}

bool SolverTask::FillRealResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("mantissa") || !doc["mantissa"].IsString())
    {
        logger->Error("mantissa error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.values["mantissa"] = doc["mantissa"].GetString();
    if (doc.HasMember("exponent") && doc["exponent"].IsString())
        result.values["exponent"] = doc["exponent"].GetString();
    if (doc.HasMember("angle_measure") && doc["angle_measure"].IsInt())
        result.values["angle_measure"] = AngleMeasureToString((AngleMeasure)doc["angle_measure"].GetInt());
    
    FillUnit(doc, result);
    FillCastUnits(doc, result);
    return true;
}

bool SolverTask::FillIntegerResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("value") || !doc["value"].IsString())
    {
        logger->Error("value error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.values["value"] = doc["value"].GetString();
    if (doc.HasMember("notation") && doc["notation"].IsInt())
        result.values["notation"] = NotationToString((Notation)doc["notation"].GetInt());
    return true;
}

bool SolverTask::FillRationalResult(rapidjson::Document& doc, Result& result)
{
    if (!doc.HasMember("numerator") || !doc["numerator"].IsString() || !doc.HasMember("denomerator") || !doc["denomerator"].IsString())
    {
        logger->Error("value error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("integer") && doc["integer"].IsString())
        result.values["integer"] = doc["integer"].GetString();
    result.values["numerator"] = doc["numerator"].GetString();
    result.values["denomerator"] = doc["denomerator"].GetString();

    FillUnit(doc, result);
    FillCastUnits(doc, result);
    return true;
}

//AutoSolverTask

AutoSolverTask::AutoSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::AutoResultConfig _config, 
    const std::u32string& _expression, const uint _delay) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression, _delay),
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
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    FillId(doc);
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
    doc.AddMember("precision", config.real_result.precision, alloc);
    doc.AddMember("default_angle_measure", (int)config.real_result.default_angle_measure, alloc);
    doc.AddMember("result_angle_measure", (int)config.real_result.result_angle_measure, alloc);
    doc.AddMember("exponent_size", config.real_result.exp, alloc);

    //integer config
    doc.AddMember("result_notation", (int)config.integer_result.result_notation, alloc);

    //rational config
    doc.AddMember("fraction_form", (int)config.rational_result.fraction_form, alloc);

    AddUnit(doc, config.real_result.unit);

    if (!SendRequest(doc, result, socket))
        return false;

    std::string json;
    if (!socket->Receive(json, result))
        return false;

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        logger->Error("Json error");
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
    default:
        return false;
    }
}

//RealSolverTask

RealSolverTask::RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RealResultConfig _config, 
    const std::u32string& _expression, const uint _delay) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression, _delay),
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
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::REAL, alloc);
    std::string s = ToBasicString(expression);
    doc.AddMember("expression", rapidjson::StringRef(s.c_str()), alloc);
    doc.AddMember("precision", config.precision, alloc);
    doc.AddMember("default_angle_measure", (int)config.default_angle_measure, alloc);
    doc.AddMember("result_angle_measure", (int)config.result_angle_measure, alloc);
    doc.AddMember("exponent_size", config.exp, alloc);

    AddUnit(doc, config.unit);

    if (!SendRequest(doc, result, socket))
        return false;

    std::string json;
    if (!socket->Receive(json, result))
        return false;

    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        logger->Error("Json error");
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

IntegerSolverTask::IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, 
    Config::IntegerResultConfig _config, const std::u32string& _expression, const uint _delay) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression, _delay),
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
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    FillId(doc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::INTEGER, alloc);
    doc.AddMember("result_notation", (int)config.result_notation, alloc);
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
        logger->Error("Json error");
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

RationalSolverTask::RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RationalResultConfig _config, 
    const std::u32string& _expression, const uint _delay) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression, _delay),
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
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    FillId(doc);
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
        logger->Error("Json error");
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

//RemoveIdentifierSolverTask

RemoveIdentifierSolverTask::RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, 
    const std::u32string& _expression, const uint _delay) :
    SolverTask(_id, _guid, _code_id, ExpressionType::USER_SYMBOL, _expression, _delay)
{
}

bool RemoveIdentifierSolverTask::Execute(WebSocketPtr socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "REMOVE_IDENTIFIER", alloc);
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
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
        logger->Error("Json error");
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

}
