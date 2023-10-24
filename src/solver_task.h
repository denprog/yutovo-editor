#ifndef __SOLVER_TASK_H__
#define __SOLVER_TASK_H__

#include "caret_state.h"
#include <memory>
#include "rapidjson/document.h"
#include "web_socket.h"
#include "result_codes.h"
#include <yutovo_calculator/unit.h>
#include <yutovo_calculator/math_helper.h>

namespace yutovo
{

using namespace yutovo_service;

enum class ExpressionType
{
    NONE = 0,
	SOLVE = 1, //expression for solving
	USER_SYMBOL //symbol of user variable or function
};

class Logger;

struct SolverTask
{
    SolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, const std::u32string& _expression, const uint _delay);
    SolverTask(std::string& _guid);

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

    ElementId id;
    std::string guid;
    uint code_id = 0;
    ExpressionType expression_type;
    std::u32string expression;
    int delay = 0; //in milliseconds
    uint64_t cur_time = 0;
    Logger* logger;
};

struct AutoSolverTask : SolverTask
{
    AutoSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::AutoResultConfig _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::AutoResultConfig config;
};

struct RealSolverTask : SolverTask
{
    RealSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RealResultConfig _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RealResultConfig config;
};

struct IntegerSolverTask : SolverTask
{
    IntegerSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::IntegerResultConfig _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::IntegerResultConfig config;
};

struct RationalSolverTask : SolverTask
{
    RationalSolverTask(ElementId _id, std::string& _guid, uint _code_id, ExpressionType _expression_type, Config::RationalResultConfig _config, 
        const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    Config::RationalResultConfig config;
};

struct RemoveIdentifierSolverTask : SolverTask
{
    RemoveIdentifierSolverTask(ElementId _id, std::string& _guid, uint _code_id, const std::u32string& _expression, const uint _delay);

    virtual bool Execute(WebSocketPtr socket, Result& result);
};

struct SetLanguageSolverTask : SolverTask
{
    SetLanguageSolverTask(std::string& _guid, const yutovo_calculator::Language _language);

    virtual bool Execute(WebSocketPtr socket, Result& result);

    yutovo_calculator::Language language;
};

typedef std::shared_ptr<SolverTask> SolverTaskPtr;

}

#endif
