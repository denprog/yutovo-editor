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
    Graph(Element* _parent);
    Graph(Document* _document);
    Graph(const Graph& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void Solve();

protected:
    mglGraph graph;
};

}

#endif
