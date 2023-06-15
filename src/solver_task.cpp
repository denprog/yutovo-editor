#include "solver_task.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "logger.h"
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
    if (result.error.error_code != ErrorCode::SOLVER_RESTARTED_ERROR)
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
    return true;
}

//AutoSolverTask

AutoSolverTask::AutoSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::AutoResult _config, 
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
    rapidjson::Value d(rapidjson::kArrayType);
    for (auto t : config.results_order)
        d.PushBack((int)t, alloc);
    doc.AddMember("results_order", d, alloc);

    //real config
    doc.AddMember("precision", config.real_result.precision, alloc);
    doc.AddMember("default_angle_measure", (int)config.real_result.default_angle_measure, alloc);
    doc.AddMember("result_angle_measure", (int)config.real_result.result_angle_measure, alloc);
    doc.AddMember("exponent_size", config.real_result.exp, alloc);

    //integer config
    doc.AddMember("result_notation", (int)config.integer_result.result_notation, alloc);

    //rational config
    doc.AddMember("fraction_form", (int)config.rational_result.fraction_form, alloc);

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

RealSolverTask::RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RealResult _config, 
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
    Config::IntegerResult _config, const std::u32string& _expression, const uint _delay) :
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

RationalSolverTask::RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RationalResult _config, 
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

RemoveIdentifierSolverTask::RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, const ResultType _result_type, 
    const std::u32string& _expression, const uint _delay) :
    SolverTask(_id, _guid, _code_id, ExpressionType::USER_SYMBOL, _expression, _delay),
    result_type(_result_type)
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
