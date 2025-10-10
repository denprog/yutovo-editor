/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __GRAPH_FORMULA_H__
#define __GRAPH_FORMULA_H__

#include "middle_shape_formula.h"
#include "config.h"
#include "mgl2/mgl.h"

namespace yutovo
{

class Graph : public Formula
{
public:
    Graph(Element* _parent, bool with_init = true);
    Graph(Document* _document, bool with_init = true);
    Graph(const Graph& source);

    void Init();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);
    virtual void Resize(const int dx, const int dy);

    virtual void MovePicture(const int dx, const int dy);
    virtual void ZoomPicture(const int pixels);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual bool Depends(const std::string& identifier);

    virtual void PutError(const Error& error);

protected:
    void SetNumber(const double num, CodeRow* el);

public:
    Dependencies dependencies;
    yutovo_solver::ErrorCode last_error_code = yutovo_solver::ErrorCode::OK;
    yutovo_calculator::ParserExceptionCode last_parser_error_code = yutovo_calculator::ParserExceptionCode::None;
    GraphFormat format;

#ifdef TEST
public:
#else
protected:
#endif
    double x_left = -1, x_right = 1, y_bottom = -1, y_top = 1;

    CodeRow* GetYTop() const;
    CodeRow* GetYBottom() const;
    CodeRow* GetExpression() const;
    CodeRow* GetXLeft() const;
    CodeRow* GetVariable() const;
    CodeRow* GetXRight() const;
    Shape* GetShape() const;

    mutable mglGraph graph;

    std::string guid;

    Config::ArrayRealResultConfig config;

    ParserString last_expression;

    bool delay = false; //don't delay on the first calculation
    bool solving = false;
};

class GraphLine : public Graph
{
public:
    GraphLine(Element* _parent, bool with_init = true);
    GraphLine(Document* _document, bool with_init = true);
    GraphLine(const GraphLine& source);

    void Init();

    virtual bool AfterFromJson();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Solve();

    virtual void PutResult(Result& result);

    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

#ifdef TEST
public:
#else
protected:
#endif
    std::vector<double> x, y;
};

}

#endif
