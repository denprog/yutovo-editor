#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, images1)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, test_image.width(), test_image.height(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, images2)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, test_image.width(), test_image.height(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(DocumentTest, images3)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, test_image.width(), test_image.height(), true));
    document.Save("images3_1.yut");

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Load("images3_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert a large image
TEST_F(DocumentTest, images4)
{
    Start(200);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, test_image.width(), test_image.height(), true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertString("2", true));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a large image and resize
TEST_F(DocumentTest, images5)
{
    Start(200);

    int width = 200;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, test_image.width(), test_image.height(), true);
    document.InsertString("1", true);
    document.WaitTask(document.InsertString("2", true));

    document.Undo();
    document.WaitUndo();

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 2})) << document.GetEditorState().ToString();
}

//Insert a large image and resize
TEST_F(DocumentTest, images6)
{
    Start(200);

    int width = 200;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, test_image.width(), test_image.height(), true);
    document.InsertString("1", true);
    document.InsertString("2", true);
    document.WaitTask(document.DeleteElements(true, true));

    document.Undo();
    document.WaitUndo();

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Move caret on the image on click on it
TEST_F(DocumentTest, images7)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertString("123", true);
    document.WaitTask(document.InsertImage(data, test_image.width(), test_image.height(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaret(80, 40));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Paste an image before an image
TEST_F(DocumentTest, images8)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& bmp, const int width, const int height)
        {
            return GetImageSizeMock(bmp, width, height);
        });

    QImage test_image("../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, test_image.width(), test_image.height(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    ASSERT_TRUE(document.IsEditable(ElementId{0, 0, 0}));

    document.WaitTask(document.InsertImage(data, test_image.width(), test_image.height(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/bmp;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

}
