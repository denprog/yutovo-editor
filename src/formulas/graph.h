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

class GraphLine : public Formula
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

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);
    virtual void Resize(const int dx, const int dy);

    virtual void MovePicture(const int dx, const int dy);
    virtual void ZoomPicture(const int pixels);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual void Solve();
    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual void PutResult(Result& result);

    virtual bool Depends(const std::string& identifier);

    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

protected:
    void SetNumber(const double num, CodeRow* el);

public:
    Dependencies dependencies;
    yutovo_solver::ErrorCode last_error_code = yutovo_solver::ErrorCode::OK;
    GraphFormat format;

#ifdef TEST
public:
#else
protected:
#endif
    double x_left = -1, x_right = 1, y_bottom = -1, y_top = 1;
    std::vector<double> x, y;

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

    int x_pos = 0;
    int x_inc = 100;
};

}

#endif
