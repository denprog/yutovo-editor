#include "solver_task.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "logger.h"

namespace yutovo
{

using namespace yutovo_service;

//SolverTask

SolverTask::SolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::string& _expression) :
    id(_id),
    guid(_guid),
    code_id(_code_id),
    expression_type(_expression_type),
    expression(_expression),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true))
{
}

bool SolverTask::SendRequest(const rapidjson::Document& json, Result& result, zmq::socket_t& socket)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    std::string str = buffer.GetString();

    logger->Info("Sending request:\n{}", str);
    zmq::message_t request(str.size());
    std::memcpy(request.data(), str.data(), str.size());
    if (socket.send(request) == 0)
    {
        logger->Error("Send timeout");
        result.error.error_code = ErrorCode::SOLVER_TIMEOUT_ERROR;
        return false;
    }
    return true;
}

//RealSolverTask

RealSolverTask::RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const uint _precision, 
    AngleMeasure _angle_measure, const std::string& _expression) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression),
    precision(_precision),
    angle_measure(_angle_measure)
{
}

bool RealSolverTask::Execute(zmq::socket_t& socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::REAL, alloc);
    doc.AddMember("expression", rapidjson::StringRef(expression.c_str()), alloc);
    doc.AddMember("precision", precision, alloc);
    doc.AddMember("angle_measure", (int)angle_measure, alloc);
    doc.AddMember("accuracy_size", 6, alloc);
    doc.AddMember("exponent_size", 3, alloc);

    if (!SendRequest(doc, result, socket))
        return false;

    //reply
    zmq::message_t reply;
    if (socket.recv(&reply) == 0)
    {
        logger->Error("Receive timeout");
        result.error.error_code = ErrorCode::SOLVER_TIMEOUT_ERROR;
        return false;
    }

    std::string json = std::string((const char*)reply.data(), reply.size());
    logger->Info("Got reply:\n{}", json);
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        logger->Error("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.type = ResultType::REAL;

    if (doc.HasMember("error"))
    {
        logger->Error("Solver error");
        if (doc["error"].IsObject())
        {
            rapidjson::Value error = doc["error"].GetObject();
            if (error.HasMember("error_code") && error["error_code"].IsInt())
            {
                result.error.error_code = (ErrorCode)error["error_code"].GetInt();
                return false;
            }
        }
        result.error.error_code = ErrorCode::EXPRESSION_ERROR;
        return false;
    }
    if (!doc.HasMember("mantissa") || !doc["mantissa"].IsString())
    {
        logger->Error("mantissa error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.values["mantissa"] = doc["mantissa"].GetString();
    if (doc.HasMember("exponent") && doc["exponent"].IsString())
        result.values["exponent"] = doc["exponent"].GetString();

    return true;
}

//IntegerSolverTask

IntegerSolverTask::IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, 
    Notation _notation, const std::string& _expression) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression),
    notation(_notation)
{
}

bool IntegerSolverTask::Execute(zmq::socket_t& socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::INTEGER, alloc);
    doc.AddMember("expression", rapidjson::StringRef(expression.c_str()), alloc);

    if (!SendRequest(doc, result, socket))
        return false;

    //reply
    zmq::message_t reply;
    if (socket.recv(&reply) == 0)
    {
        logger->Error("Receive timeout");
        result.error.error_code = ErrorCode::SOLVER_TIMEOUT_ERROR;
        return false;
    }
    
    std::string json = std::string((const char*)reply.data(), reply.size());
    logger->Info("Got reply:\n{}", json);
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        logger->Error("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.type = ResultType::INTEGER;

    if (doc.HasMember("error"))
    {
        logger->Error("Solver error");
        if (doc["error"].IsObject())
        {
            rapidjson::Value error = doc["error"].GetObject();
            if (error.HasMember("error_code") && error["error_code"].IsInt())
            {
                result.error.error_code = (ErrorCode)error["error_code"].GetInt();
                return false;
            }
        }
        result.error.error_code = ErrorCode::EXPRESSION_ERROR;
        return false;
    }
    if (!doc.HasMember("value") || !doc["value"].IsString())
    {
        logger->Error("value error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.values["value"] = doc["value"].GetString();

    return true;
}

//RationalSolverTask

RationalSolverTask::RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::string& _expression) :
    SolverTask(_id, _guid, _code_id, _expression_type, _expression)
{
}

bool RationalSolverTask::Execute(zmq::socket_t& socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "SOLVE_CODE", alloc);
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("result_type", (int)ResultType::RATIONAL, alloc);
    doc.AddMember("expression", rapidjson::StringRef(expression.c_str()), alloc);

    if (!SendRequest(doc, result, socket))
        return false;

    //reply
    zmq::message_t reply;
    if (socket.recv(&reply) == 0)
    {
        logger->Error("Receive timeout");
        result.error.error_code = ErrorCode::SOLVER_TIMEOUT_ERROR;
        return false;
    }
    
    std::string json = std::string((const char*)reply.data(), reply.size());
    logger->Info("Got reply:\n{}", json);
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        logger->Error("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.type = ResultType::RATIONAL;

    if (doc.HasMember("error"))
    {
        logger->Error("Solver error");
        if (doc["error"].IsObject())
        {
            rapidjson::Value error = doc["error"].GetObject();
            if (error.HasMember("error_code") && error["error_code"].IsInt())
            {
                result.error.error_code = (ErrorCode)error["error_code"].GetInt();
                return false;
            }
        }
        result.error.error_code = ErrorCode::EXPRESSION_ERROR;
        return false;
    }
    if (!doc.HasMember("numerator") || !doc["numerator"].IsString() || !doc.HasMember("denomerator") || !doc["denomerator"].IsString())
    {
        logger->Error("value error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    result.values["numerator"] = doc["numerator"].GetString();
    result.values["denomerator"] = doc["denomerator"].GetString();

    return true;
}

//RemoveIdentifierSolverTask

RemoveIdentifierSolverTask::RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, const ResultType _result_type, 
    const std::string& _expression) :
    SolverTask(_id, _guid, _code_id, ExpressionType::USER_SYMBOL, _expression),
    result_type(_result_type)
{
}

bool RemoveIdentifierSolverTask::Execute(zmq::socket_t& socket, Result& result)
{
    //request
    rapidjson::Document doc;
    auto& alloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("command", "REMOVE_IDENTIFIER", alloc);
    doc.AddMember("guid", rapidjson::StringRef(guid.c_str()), alloc);
    doc.AddMember("code_id", code_id, alloc);
    doc.AddMember("solver_type", (int)SolverType::CALCULATOR, alloc);
    doc.AddMember("expression", rapidjson::StringRef(expression.c_str()), alloc);

    if (!SendRequest(doc, result, socket))
        return false;
    
    //reply
    zmq::message_t reply;
    if (socket.recv(&reply) == 0)
    {
        logger->Error("Receive timeout");
        result.error.error_code = ErrorCode::SOLVER_TIMEOUT_ERROR;
        return false;
    }
    
    std::string json = std::string((const char*)reply.data(), reply.size());
    logger->Info("Got reply:\n{}", json);
    doc.Parse<0>(json.c_str());
    if (doc.HasParseError())
    {
        logger->Error("Json error");
        result.error.error_code = ErrorCode::JSON_ERROR;
        return false;
    }

    if (doc.HasMember("error"))
    {
        logger->Error("Solver error");
        if (doc["error"].IsObject())
        {
            rapidjson::Value error = doc["error"].GetObject();
            if (error.HasMember("error_code") && error["error_code"].IsInt())
            {
                result.error.error_code = (ErrorCode)error["error_code"].GetInt();
                return false;
            }
        }
        result.error.error_code = ErrorCode::EXPRESSION_ERROR;
        return false;
    }

    return true;
}

}
