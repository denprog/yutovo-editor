/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "middle_shape_formula.h"
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

    virtual void UpdateLevel(uint8_t _level);

    virtual void Solve();
    virtual void ReSolve(bool if_error = false, bool force = false);

    virtual void ToParserString(ParserString& str);

public:
    Dependencies dependencies;
    ParserString last_func_expr, last_arg_expr, last_x_left_expr, last_x_right_expr, 
        last_y_down_expr, last_y_up_expr;

protected:
    CodeRow* GetXLeft() const;
    CodeRow* GetXRight() const;
    CodeRow* GetYDown() const;
    CodeRow* GetYUp() const;

    mutable mglGraph graph;
    bool empty = true;
};

}

#endif
