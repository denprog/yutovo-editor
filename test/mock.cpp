#include "mock.h"
#include <QBuffer>

namespace yutovo_test
{

int argc = 0;
char** argv = nullptr;

static const std::string base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
