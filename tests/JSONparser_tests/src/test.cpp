#include <gtest/gtest.h>
#include "json.h"

TEST(TestCaseName, TestName) {
  auto json = Json::Parse("{}");

  EXPECT_TRUE(json->GetType() == Json::ValueType::Object);
}