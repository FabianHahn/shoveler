#include <string>

#include <gtest/gtest.h>

extern "C" {
#include "shoveler/component.h"
#include "shoveler/log.h"
}

class ShovelerComponentTest : public ::testing::Test {
public:
	virtual void SetUp()
	{
	}

	virtual void TearDown()
	{
		shovelerLogTrace("Tearing down test case.");
	}
};

TEST_F(ShovelerComponentTest, activateDeactivate)
{

}
