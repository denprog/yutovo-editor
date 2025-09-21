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

class Graph : public MiddleShapeFormula
{
public:
    Graph(Element* _parent, bool with_init = true);
    Graph(Document* _document, bool with_init = true);
    Graph(const Graph& source);

    void Init();

    virtual bool AfterFromJson();

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual bool DeleteElements(bool left, bool with_undo, ElementId& changed_element);

    virtual void UpdateLevel(uint8_t _level);

    virtual void Solve();
    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual void PutResult(Result& _result);

    virtual bool Depends(const std::string& identifier);

    virtual void ToParserString(ParserString& str);

public:
    Dependencies dependencies;
    yutovo_solver::ErrorCode last_error_code = yutovo_solver::ErrorCode::OK;

protected:
    CodeRow* GetXLeft() const;
    CodeRow* GetXRight() const;
    CodeRow* GetYDown() const;
    CodeRow* GetYUp() const;

    mutable mglGraph graph;

    std::string guid;

    Config::ArrayRealResultConfig config;

    ParserString last_expression;

    std::vector<double> x, y;

    bool delay = false; //don't delay on the first calculation
    bool solving = false;

    int x_pos = 0;
    int x_inc = 100;
};

}

#endif
