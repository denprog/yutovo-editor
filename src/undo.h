#ifndef __UNDO_H__
#define __UNDO_H__

#include <any>
#include <map>
#include "caret_state.h"
#include "style.h"
#include "config.h"
#include "element.h"

namespace yutovo
{

struct UndoElement;
class Element;
class String;
class Paragraph;
class Image;
class Text;
class CodeRow;
class CodeParagraph;
class Formula;
class CodeBlock;
class Equation;

typedef std::shared_ptr<UndoElement> UndoElementPtr;

struct UndoElement
{
    UndoElement(ElementType _type);

    virtual bool operator==(const UndoElement& el) const;
    virtual bool operator==(const Element& el) const;

    virtual Element* Restore(Document* document, Element* parent) = 0;

    ElementType type;
    std::vector<UndoElementPtr> elements;
};

struct UndoString : UndoElement
{
    UndoString(std::u32string _str, StringFormatPtr _format, bool _can_merge);

    virtual bool operator==(const UndoString& el) const;
    virtual bool operator==(const Element& el) const;
    virtual bool operator==(const String& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    std::u32string str;
    StringFormatPtr format;
    bool can_merge;
};

struct UndoParagraph : UndoElement
{
    UndoParagraph(ParagraphFormatPtr _format);

    virtual bool operator==(const UndoParagraph& el) const;
    virtual bool operator==(const Paragraph& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    ParagraphFormatPtr format;
};

struct UndoImage : UndoElement
{
    UndoImage(const std::vector<unsigned char>& _picture, const int _width, const int _height);

    virtual bool operator==(const UndoImage& el) const;
    virtual bool operator==(const Image& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    std::vector<unsigned char> picture;
    int width, height;
};

struct UndoFormula : UndoElement
{
    UndoFormula(ElementType _type, FormulaFormatPtr _formula_format);

    virtual bool operator==(const UndoFormula& el) const;
    virtual bool operator==(const Formula& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    FormulaFormatPtr formula_format;
};

struct UndoCodeRow : UndoElement
{
    UndoCodeRow();

    virtual bool operator==(const UndoCodeRow& el) const;
    virtual bool operator==(const CodeRow& el) const;

    virtual Element* Restore(Document* document, Element* parent);
};

struct UndoCodeParagraph : UndoParagraph
{
    UndoCodeParagraph(ParagraphFormatPtr _format);

    virtual bool operator==(const UndoCodeParagraph& el) const;
    virtual bool operator==(const CodeParagraph& el) const;

    virtual Element* Restore(Document* document, Element* parent);
};

struct UndoCodeBlock : UndoElement
{
    UndoCodeBlock(uint _code_id, CodeFormatPtr _code_format, ParagraphFormatPtr _paragraph_format, FormulaFormatPtr _formula_format);

    virtual bool operator==(const UndoCodeBlock& el) const;
    virtual bool operator==(const CodeBlock& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    uint code_id;
    CodeFormatPtr code_format;
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

struct UndoCodeString : UndoString
{
    UndoCodeString(std::u32string _str, StringFormatPtr _format, bool _can_merge);

    virtual Element* Restore(Document* document, Element* parent);
};

struct UndoEquation : UndoFormula
{
    UndoEquation(Equation* equation);

    virtual bool operator==(const UndoEquation& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    yutovo_solver::ResultType result_type;
    std::any config;
};

class ConfigElement : public Element
{
public:
    ConfigElement(const Config& _config);

    virtual Element* Clone()
    {
        return nullptr;
    }

    virtual Element* Create(Element* parent)
    {
        return nullptr;
    }

    Config config;
};

struct UndoConfig : UndoElement
{
    UndoConfig(const Config& _config);

    virtual Element* Restore(Document* document, Element* parent);

    Config config;
};

class UndoBase
{
public:
    UndoBase(Document* _document);

    int Store(const ElementId& id);
    int Store(const ElementId& parent_id, const int pos, const int size);
    int Store(const Config& config);
    bool Restore(int undo_id, std::vector<ElementPtr>& elements);
    bool Restore(int undo_id, Config& config);

private:
    int Store(const int undo_id, const LogicalId& id);
    UndoElementPtr StoreElement(const LogicalId id, ElementPtr el);

private:
    Document* document;

    int next_undo_id = 1;

    struct UndoItem
    {
        LogicalId id;
        int refs = 0;
        std::vector<UndoElementPtr> undo_elements; //all saved variants of this element
    };

    std::vector<UndoItem> undo_store; //cached elements are stored here
    std::map<int, std::vector<UndoElementPtr>> undo_items; //undo items by undo_id
};

}

#endif
