#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace yutovo_service;
using namespace std::chrono_literals;

//Present Auto result as Real result
TEST_F(SolverRealTest, solver1)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("1.23456789", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.23456789</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.235</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetResult({0, 0, 0, 0, 0, 0, 0, 2, 0, 0}, ResultType::REAL));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>1.23456789</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>1.235</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

}
