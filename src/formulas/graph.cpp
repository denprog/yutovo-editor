/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "graph.h"
#include "document.h"

namespace yutovo
{

//Graph

Graph::Graph(Element* _parent) : 
    MiddleShapeFormula(_parent)
{
    type = ElementType::GRAPH_2D;
}

Graph::Graph(Document* _document) : 
    MiddleShapeFormula(_document)
{
    type = ElementType::GRAPH_2D;
}

Graph::Graph(const Graph& source) : 
    MiddleShapeFormula(source)
{
}

Element* Graph::Clone()
{
    return new Graph(*this);
}

Element* Graph::Create(Element* _parent)
{
    return new Graph(_parent);
}

void Graph::ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
}

Element* Graph::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    return nullptr;
}

void Graph::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
        };

    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    MiddleShapeFormula::Draw();
}

bool Graph::Remake(bool with_elements)
{
    bool changed = MiddleShapeFormula::Remake(with_elements);

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void Graph::Solve()
{
}

}
