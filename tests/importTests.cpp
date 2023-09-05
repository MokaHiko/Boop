#include <gtest/gtest.h>
#include <filesystem>

#include <boop.h>
#include <glm/glm.hpp>

struct ImportTest : public ::testing::Test
{
	int GetX()
	{
		return _x;
	}

	virtual void SetUp() override
	{
		_x = 10;
	}

	virtual void TearDown() override
	{
	}

private:
	int _x = 0;
};

TEST_F(ImportTest, ImportPNG)
{
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = glm::vec3(1, 0, 0);
	EXPECT_EQ(glm::dot(up, right), 0);
}

TEST_F(ImportTest, ImportOBJ)
{
	EXPECT_TRUE(true);
}

TEST_F(ImportTest, ImportFbx)
{
	EXPECT_TRUE(true);
}
