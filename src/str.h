#ifndef __STRING_H__
#define __STRING_H__

#include <string>
#include "element.h"
#include "document.h"
#include "style.h"
#include <boost/serialization/unique_ptr.hpp>

namespace yutovo
{

class String : public Element
{
public:
    String(Element* parent);
    String(Element* parent, const std::string _str);
    String(Element* parent, const std::string _str, const StringFormatPtr _format);
    String(Element* parent, const std::u32string _str, const StringFormatPtr _format);
    String(Document* _document, const std::string _str, const StringFormatPtr _format);
    String(Document* _document, const std::u32string _str, const StringFormatPtr _format);

    virtual Element* Clone();
    virtual bool Copy(std::vector<ElementPtr>& copy);

    virtual Element* Create(Element* parent);
    virtual Element* Create(Element* parent, const std::u32string _str, const StringFormatPtr _format);

    virtual void Remake(bool with_elements, bool with_parent, bool with_undo);
    virtual void Normalize(bool with_undo);

    virtual void UpdateRect(bool with_elements = false);

    virtual bool GetElementAtCoords(const int x, const int y, ElementId& _id);

    virtual std::string ToHtml();

    virtual bool InsertElements(std::vector<ElementPtr>& _elements, bool with_undo);
    virtual bool DeleteElements(bool left, bool with_undo);
    virtual bool ChangeStringFormat(const StringFormatPtr format, bool with_undo);

    virtual bool Split(const uint max_left_width);
    virtual bool SplitAt(const uint pos);
    virtual bool Merge(const ElementPtr with_element);

    virtual bool AfterInsert(bool with_undo);

    virtual StringFormatPtr GetStringFormat() const;
    virtual void UpdateStringFormat(const StringFormatPtr base_format, const StringFormatPtr new_format);
    virtual void UpdateFormat(StringFormatPtr& _format);
    int GetFontSize(const uint size);

    virtual bool CanContinueSelection();

    virtual void UpdateDrawRect();

    virtual void UpdateLevel(uint8_t _level);

    virtual void SetEditable(bool _editable);

    virtual void GetElements(ElementType _type, std::vector<ElementId>& _elements);
    virtual void GetElementsBelow(const ElementId from_id, ElementType _type, std::vector<ElementId>& _elements);

    virtual void ReSolve();

    virtual void SubscribeOnChange(const ElementId _id);

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << format->id;
        ar << (boost::serialization::base_object<Element>(*this), elements);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> (boost::serialization::base_object<Element>(*this), elements);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    bool FindCachedSize(const std::u32string& str, Size& size);
    void AddCachedSize(const std::u32string& str, const Size& size);
    void ResetCache();

protected:
    friend class Document;
    friend class ChangeStringFormatTask;
    friend class StringElements;
    friend class InsertElementsTask;
    StringFormatPtr format;

private:
    std::map<std::u32string, std::pair<Size, time_t>> size_cache; //cache of string sizes
    static const int max_cache_size = 2;
};

class StringElements : public Elements
{
public:
    StringElements(Element* parent);
    StringElements(Element* parent, const std::u32string& _str);

    virtual Elements* Clone(Element* _parent);

    virtual void Draw() const;

    virtual ElementPtr Get(uint pos);
    virtual ElementId GetElementId(uint pos);
    
    virtual void Add(ElementPtr element);
    virtual void Insert(ElementPtr element, const uint pos);
    virtual void Remove(const ElementPtr element);
    virtual void RemoveAt(const uint pos, const int size);
    virtual void Clear();
    virtual uint Count() const;

    virtual Rect GetCaretRect(const uint pos) const;
    virtual void DrawCaret(const uint pos) const;

    virtual Rect GetRect();

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetRightCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordLeftCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetWordRightCaretState(CaretState& caret_state, Selection* select);

    virtual std::string ToHtml();
    virtual std::u32string ToText();

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << parent;
        ar << ToBasicString(str);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        std::string s;
        ar >> s;
        str = ToUtfString(s);
    }

	BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    friend class String;

    std::u32string str;
};

}

namespace boost
{
namespace serialization
{

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::String* t, const unsigned int version)
{
    ar << t->parent;
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::String* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    uint format_id;
    ar >> format_id;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    auto f = user_data.document->GetStringFormat(format_id);
    if (f)
        ::new(t)yutovo::String(p, "", f);
    else
        ::new(t)yutovo::String(p);
}

template<class Archive>
void save_construct_data(Archive& ar, const yutovo::StringElements* t, const unsigned int version)
{
}

template<class Archive>
void load_construct_data(Archive& ar, yutovo::StringElements* t, const unsigned int version)
{
    yutovo::Element* p;
    ar >> p;
    yutovo::DocumentUserData& user_data = yutovo::GetUserData<yutovo::DocumentUserData>(ar);
    ::new(t)yutovo::StringElements(p);
}

}
}

#endif
