/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "mock.h"
#include <QBuffer>
#include <QSettings>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDir>
#include <algorithm>
#ifdef Q_OS_LINUX
#include <fontconfig/fontconfig.h>
#else
#include <Windows.h>
#endif
#include "formulas/code_string.h"
#include "formulas/code_row.h"
#include "formulas/division.h"
#include "formulas/power.h"

namespace yutovo_test
{

int argc = 0;
char** argv = nullptr;

static const std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//DocumentTest

Size DocumentTest::GetTextSizeMock(const std::u32string& text, const StringFormatPtr format)
{
    QFont font(format->family.c_str());
    font.setPixelSize(round(format->size / 0.75));
    font.setBold(format->bold);
    font.setItalic(format->italic);
    font.setUnderline(format->underline);
    QFontMetrics m(font);
    QString str = QString::fromUcs4(text.c_str());
    QSize s = m.size(Qt::TextSingleLine, str);
    int cx = m.horizontalAdvance(str);
    return Size{cx > s.width() ? cx : s.width(), s.height()};
}

Size DocumentTest::GetImageSizeMock(const std::vector<unsigned char>& image)
{
    QImage picture;
    picture.loadFromData(&image[0], image.size());
    return Size{picture.width(), picture.height()};
}

std::string DocumentTest::Base64Encode(std::vector<unsigned char>& arr)
{
    std::string res;
    int val = 0, valb = -6;
    for (uchar c : arr)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            res.push_back(base[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        res.push_back(base[((val << 8) >> (valb + 8)) & 0x3F]);
    while (res.size() % 4)
        res.push_back('=');
    return res;
}

void DocumentTest::GetImageData(QImage& image, std::vector<unsigned char>& data)
{
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    data = std::vector<unsigned char>(arr.begin(), arr.end());
}

//FormulaTest

void FormulaTest::MoveToDenominatorVariable()
{
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(false));
}

void FormulaTest::FillFunctionAndVariable()
{
    document.WaitTask(document.InsertString("f", true));
    MoveToDenominatorVariable();
    document.WaitTask(document.InsertString("x", true));
}

Division* FormulaTest::CreateMixedDerivativeDivision(int order, const std::u32string& func, const std::vector<std::u32string>& vars)
{
    Division* div = new Division(&document);
    CodeRow<>* num = div->GetNumeratorRow();
    CodeRow<>* den = div->GetDenominatorRow();

    if (order == 1)
    {
        ElementPtr d(new CodeString(num, U"d"));
        d->can_merge = false;
        div->AddNumerator(d);
    }
    else
    {
        Power* p = new Power(num);
        CodeRow<>* base = p->GetBaseRow();
        CodeRow<>* exp = p->GetExponentRow();
        base->elements->Clear();
        exp->elements->Clear();

        ElementPtr d(new CodeString(base, U"d"));
        d->can_merge = false;
        base->elements->Add(d);

        std::u32string str;
        do
        {
            str = char32_t(U'0' + order % 10) + str;
            order /= 10;
        }
        while (order > 0);

        ElementPtr n(new CodeString(exp, str));
        n->can_merge = false;
        exp->elements->Add(n);

        div->AddNumerator(ElementPtr(p));
    }

    for (char32_t c : func)
    {
        ElementPtr s(new CodeString(num, std::u32string(1, c)));
        s->can_merge = false;
        div->AddNumerator(s);
    }

    for (const auto& var : vars)
    {
        ElementPtr d(new CodeString(den, U"d"));
        d->can_merge = false;
        den->elements->Add(d);
        for (char32_t c : var)
        {
            ElementPtr s(new CodeString(den, std::u32string(1, c)));
            s->can_merge = false;
            den->elements->Add(s);
        }
    }

    return div;
}

Division* FormulaTest::CreateMixedDerivativeDivision(const std::u32string& func, const std::vector<std::u32string>& vars)
{
    return CreateMixedDerivativeDivision((int)vars.size(), func, vars);
}

Division* FormulaTest::CreateStringDerivativeDivision(const std::u32string& numerator, const std::u32string& denominator)
{
    Division* div = new Division(&document);
    CodeRow<>* num = div->GetNumeratorRow();
    CodeRow<>* den = div->GetDenominatorRow();

    ElementPtr n(new CodeString(num, numerator));
    n->can_merge = false;
    div->AddNumerator(n);

    ElementPtr d(new CodeString(den, denominator));
    d->can_merge = false;
    div->AddDenomerator(d);

    return div;
}

//PdfTest

QString PdfTest::ResolveFontPath(const StringFormatPtr format)
{
#ifdef Q_OS_LINUX
    FcInit();

    FcPattern* pat = FcPatternCreate();
    FcPatternAddString(pat, FC_FAMILY, reinterpret_cast<const FcChar8*>(format->family.c_str()));
    FcPatternAddInteger(pat, FC_WEIGHT, format->bold ? FC_WEIGHT_BOLD : FC_WEIGHT_REGULAR);
    FcPatternAddInteger(pat, FC_SLANT, format->italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
    FcConfigSubstitute(nullptr, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult result;
    FcPattern* match = FcFontMatch(nullptr, pat, &result);
    QString path;
    if (match)
    {
        FcChar8* file = nullptr;
        if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch)
            path = QString::fromUtf8(reinterpret_cast<char*>(file));
        FcPatternDestroy(match);
    }
    FcPatternDestroy(pat);
    return path;
#else
    //Windows: look the font up in the fonts registry, the value data is a file name in the Windows fonts directory
    QSettings settings(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts)", QSettings::NativeFormat);

    QString family = QString::fromStdString(format->family).toLower();

    //generic families and common Linux fonts fall back to their metric-compatible Windows counterparts
    if (family == "sans-serif" || family == "sans serif" || family == "helvetica" || family == "dejavu sans" || family == "liberation sans")
        family = "arial";
    else if (family == "serif" || family == "times" || family == "dejavu serif" || family == "liberation serif")
        family = "times new roman";
    else if (family == "monospace" || family == "mono" || family == "dejavu sans mono" || family == "liberation mono")
        family = "courier new";

    QString fallback;
    const QStringList keys = settings.childKeys();
    for (const QString& key : keys)
    {
        QString name = key.toLower();
        name.remove(QRegularExpression(R"(\s*\([^)]*\)\s*$)")); //drop the "(TrueType)" style suffix

        bool bold = false;
        bool italic = false;
        if (name.endsWith("bold italic"))
        {
            bold = true;
            italic = true;
            name.chop(sizeof("bold italic") - 1);
        }
        else if (name.endsWith("italic"))
        {
            italic = true;
            name.chop(sizeof("italic") - 1);
        }
        else if (name.endsWith("bold"))
        {
            bold = true;
            name.chop(sizeof("bold") - 1);
        }
        name = name.trimmed();
        if (name != family && name != "arial")
            continue;

        QString file = settings.value(key).toString();
        if (file.isEmpty())
            continue;
        if (file.contains(';'))
            file = file.section(';', 0, 0);
        if (!QFileInfo(file).isAbsolute())
        {
            QString windir = qEnvironmentVariable("WINDIR", "C:\\Windows");
            file.replace("%SystemRoot%", windir, Qt::CaseInsensitive);
            if (!QFileInfo(file).isAbsolute())
                file = QDir(windir + "/Fonts").absoluteFilePath(file);
        }

        if (name == "arial" && family != "arial")
            fallback = file; //metric-compatible substitute is not registered, use Arial itself
        if (name != family)
            continue;
        if (bold == format->bold && italic == format->italic)
            return file;
        if (fallback.isEmpty())
            fallback = file; //no style match, use the regular file and let the renderer synthesize the style
    }
    return fallback;
#endif
}

std::string PdfTest::PdfText(std::string text)
{
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
    return text;
}

}
