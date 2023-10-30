#include "mock.h"
#include <QBuffer>

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

Size DocumentTest::GetImageSizeMock(const std::vector<unsigned char>& bmp, const int width, const int height)
{
    QImage image(&bmp[0], width, height, QImage::Format_ARGB32);
    return Size{image.width(), image.height()};
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
    image.convertTo(QImage::Format_ARGB32);

    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "BMP");

    data = std::vector<unsigned char>(arr.begin(), arr.end());
}

}
