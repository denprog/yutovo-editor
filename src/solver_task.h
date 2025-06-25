#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include <memory>
#include <rapidjson/document.h>
#include "web_socket.h"
#include "result_codes.h"
#include <yutovo_calculator/unit.h>
#include <yutovo_calculator/math_helper.h>

namespace yutovo
{

using namespace yutovo_solver;

class Logger;

struct SolverTask
{
    SolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, const std::u32string& _expression, const uint _delay, Logger* _logger);
    SolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, uint _code_id, 
        ExpressionType _expression_type, const std::u32string& _expression, const uint _delay, Logger* _logger);
    SolverTask(const std::string& _document_guid, const std::string& _solver_guid, Logger* _logger);
    SolverTask(const std::string& _document_guid, Logger* _logger);
    SolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, uint _code_id, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result) = 0;

    bool SendRequest(const rapidjson::Document& json, Result& result, WebSocketPtr& socket);

    void AddUnit(rapidjson::Document& json, const yutovo_calculator::Unit& unit);

    void GetResultType(const rapidjson::Document& json, Result& result);
    void GetDependencies(const rapidjson::Document& json, Result& result);

    void FillId(rapidjson::Document& doc);
    void FillUnit(rapidjson::Document& doc, Result& result);
    void FillCastUnits(rapidjson::Document& doc, Result& result);
    void FillError(rapidjson::Document& doc, Result& result);

    bool FillRealResult(rapidjson::Document& doc, Result& result);
    bool FillIntegerResult(rapidjson::Document& doc, Result& result);
    bool FillRationalResult(rapidjson::Document& doc, Result& result);
    bool FillComplexResult(rapidjson::Document& doc, Result& result);

    LogicalId id;
    std::string document_guid;
    std::string solver_guid;
    std::string task_guid;
    uint code_id = 0;
    ExpressionType expression_type = ExpressionType::NONE;
    std::u32string expression;
    int delay = 0; //in milliseconds
    uint64_t cur_time = 0;
    Logger* logger;
};

struct AutoSolverTask : SolverTask
{
    AutoSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::AutoResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::AutoResultConfig config;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::RealResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RealResultConfig config;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::IntegerResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::IntegerResultConfig config;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::RationalResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RationalResultConfig config;
};

struct ComplexSolverTask : SolverTask
{
    ComplexSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, uint _code_id, 
        ExpressionType _expression_type, Config::ComplexResultConfig _config, const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::ComplexResultConfig config;
};

struct BreakSolverTask : SolverTask
{
    BreakSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, uint _code_id, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct SetIdentifierSolverTask : AutoSolverTask
{
    SetIdentifierSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, const std::string& _task_guid, 
        uint _code_id, Document* _document, Config::AutoResultConfig _config, const std::u32string& _identifier, const std::u32string& _expression, 
        const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Document* document;
    const std::u32string identifier;
};

struct RemoveIdentifierSolverTask : SolverTask
{
    RemoveIdentifierSolverTask(const LogicalId& _id, const std::string& _document_guid, const std::string& _solver_guid, uint _code_id, 
        Document* _document, const std::u32string& _expression, const uint _delay, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Document* document;
};

struct RemoveUserIdentifiersSolverTask : SolverTask
{
    RemoveUserIdentifiersSolverTask(const std::string& _document_guid, const std::string& _solver_guid, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct ClearExportSolverTask : SolverTask
{
    ClearExportSolverTask(const std::string& _document_guid, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct SetLocaleSolverTask : SolverTask
{
    SetLocaleSolverTask(const std::string& _document_guid, const std::string& _solver_guid, const yutovo_calculator::Language _language, 
        Document* _document, Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    yutovo_calculator::Language language;
    Document* document;
};

struct ListIdentifiersSolverTask : SolverTask
{
    ListIdentifiersSolverTask(const std::string& _document_guid, const std::string& _solver_guid, const uint _code_id, Document* _document, 
        Logger* _logger);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    const uint code_id;
    Document* document;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
