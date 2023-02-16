#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mock.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
	yutovo_test::argc = argc;
	yutovo_test::argv = argv;
	return RUN_ALL_TESTS();
}
