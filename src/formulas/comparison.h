/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __COMPARISON_FORMULA_H__
#define __COMPARISON_FORMULA_H__

#include "middle_shape_formula.h"

namespace yutovo
{

class Comparison : public MiddleShapeFormula
{
protected:
    Comparison(Element* _parent, const std::u32string& _sign, const std::u32string& _parser_sign, ElementType _type, bool with_init = true);
    Comparison(Document* _document, const std::u32string& _sign, const std::u32string& _parser_sign, ElementType _type, bool with_init = true);
    Comparison(const Comparison& source) = default;

public:
    virtual Element* Clone() = 0;

    virtual Element* Create(Element* _parent) = 0;

    virtual void ToJson(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    virtual bool AfterFromJson();

    virtual void Draw() const;

    virtual void UpdateRect(bool with_elements = false);

    virtual bool Remake(bool with_elements = false);

    virtual bool AfterInsert(bool with_undo);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

    std::u32string sign;
    std::u32string parser_sign;
};

class Equal : public Comparison
{
public:
    Equal(Element* _parent, bool with_init = true);
    Equal(Document* _document, bool with_init = true);
    Equal(const Equal& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    virtual void Rescale() const;

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

class Greater : public Comparison
{
public:
    Greater(Element* _parent, bool with_init = true);
    Greater(Document* _document, bool with_init = true);
    Greater(const Greater& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

class Less : public Comparison
{
public:
    Less(Element* _parent, bool with_init = true);
    Less(Document* _document, bool with_init = true);
    Less(const Less& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

class GreaterEqual : public Comparison
{
public:
    GreaterEqual(Element* _parent, bool with_init = true);
    GreaterEqual(Document* _document, bool with_init = true);
    GreaterEqual(const GreaterEqual& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

class LessEqual : public Comparison
{
public:
    LessEqual(Element* _parent, bool with_init = true);
    LessEqual(Document* _document, bool with_init = true);
    LessEqual(const LessEqual& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

class NotEqual : public Comparison
{
public:
    NotEqual(Element* _parent, bool with_init = true);
    NotEqual(Document* _document, bool with_init = true);
    NotEqual(const NotEqual& source) = default;

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value,
        rapidjson::Document::AllocatorType& alloc);
};

}

#endif
