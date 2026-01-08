/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "mock.h"
#include <QBuffer>
#ifdef Q_OS_LINUX
#include <fontconfig/fontconfig.h>
#else
#include <Windows.h>
#endif

namespace yutovo_test
{

int argc = 0;
char** argv = nullptr;

static const std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
    WCHAR filePath[MAX_PATH];
    DWORD size = MAX_PATH;
    BOOL ok = GetFontResourceInfoW((LPCWSTR)family.toStdWString().c_str(), &size, filePath, GFRI_FONTFILENAME);
    if (ok)
        return QString::fromWCharArray(filePath);
    return {};
#endif
}

}
