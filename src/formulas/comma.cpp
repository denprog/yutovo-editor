/*
 * Yutovo Editor
 * Copyright (C) 2022-2025 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "comma.h"
#include "../document.h"

namespace yutovo
{

//Comma

Comma::Comma(Element* _parent) :
    OnlyShapeFormula(_parent, ',')
{
    type = ElementType::COMMA;
}

Comma::Comma(Document* _document) :
    OnlyShapeFormula(_document, ',')
{
    type = ElementType::COMMA;
}

Comma::Comma(const Comma& source) :
    OnlyShapeFormula(source)
{
}

Element* Comma::Clone()
{
    return new Comma(*this);
}

Element* Comma::Create(Element* _parent)
{
    return new Comma(_parent);
}

Element* Comma::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    Comma* p = nullptr;
    if (parent)
        p = new Comma(parent);
    else
        p = new Comma(document);
    if (!p->SymbolFromJson(value, alloc))
    {
        delete p;
        return nullptr;
    }
    return p;
}

std::string Comma::ToHtml() const
{
    return "<mo>,</mo>";
}

}
