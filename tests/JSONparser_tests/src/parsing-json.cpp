#include "parsing-json.h"

TEST_F(ParsingTests, EmptyObject) {
  auto json = Json::Parse("{}");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");
  EXPECT_EQ(json->GetChildren().size(), 0);
}

TEST_F(ParsingTests, EmptyArray) {
  auto json = Json::Parse("[]");

  EXPECT_EQ(json->GetType(), Json::ValueType::Array);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");
  EXPECT_EQ(json->GetChildren().size(), 0);
}

TEST_F(ParsingTests, ObjectWithValidNumberElement) {
  auto json = Json::Parse(R"({ "key" : 14 })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");
  
  auto& children = json->GetChildren();
  ASSERT_EQ(children.size(), 1);

  auto& number_element = children.front();
  EXPECT_STREQ(number_element->GetKey().c_str(), "key");
  EXPECT_EQ(number_element->GetChildren().size(), 0);
  EXPECT_EQ(number_element->GetParent(), json.get());
  EXPECT_EQ(number_element->GetType(), Json::ValueType::Number);
  EXPECT_EQ(number_element->GetNumberValue(), 14);
}


TEST_F(ParsingTests, ObjectWithValidStringElement) {
  auto json = Json::Parse(R"({ "key" : "value" })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& children = json->GetChildren();
  ASSERT_EQ(children.size(), 1);

  auto& number_element = children.front();
  EXPECT_STREQ(number_element->GetKey().c_str(), "key");
  EXPECT_EQ(number_element->GetChildren().size(), 0);
  EXPECT_EQ(number_element->GetParent(), json.get());
  EXPECT_EQ(number_element->GetType(), Json::ValueType::String);
  EXPECT_STREQ(number_element->GetStringValue().c_str(), "value");
}

TEST_F(ParsingTests, ObjectWithValidTrueElement) {
  auto json = Json::Parse(R"({ "key" : true })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& children = json->GetChildren();
  ASSERT_EQ(children.size(), 1);

  auto& number_element = children.front();
  EXPECT_STREQ(number_element->GetKey().c_str(), "key");
  EXPECT_EQ(number_element->GetChildren().size(), 0);
  EXPECT_EQ(number_element->GetParent(), json.get());
  EXPECT_EQ(number_element->GetType(), Json::ValueType::True);
  EXPECT_EQ(number_element->GetBoolValue(), true);
}

TEST_F(ParsingTests, ObjectWithValidFalseElement) {
  auto json = Json::Parse(R"({ "key" : false })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& children = json->GetChildren();
  ASSERT_EQ(children.size(), 1);

  auto& number_element = children.front();
  EXPECT_STREQ(number_element->GetKey().c_str(), "key");
  EXPECT_EQ(number_element->GetChildren().size(), 0);
  EXPECT_EQ(number_element->GetParent(), json.get());
  EXPECT_EQ(number_element->GetType(), Json::ValueType::False);
  EXPECT_EQ(number_element->GetBoolValue(), false);
}

TEST_F(ParsingTests, ObjectWithValidNullElement) {
  auto json = Json::Parse(R"({ "key" : false })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& children = json->GetChildren();
  ASSERT_EQ(children.size(), 1);

  auto& number_element = children.front();
  EXPECT_STREQ(number_element->GetKey().c_str(), "key");
  EXPECT_EQ(number_element->GetChildren().size(), 0);
  EXPECT_EQ(number_element->GetParent(), json.get());
  EXPECT_EQ(number_element->GetType(), Json::ValueType::False);
  EXPECT_EQ(number_element->GetBoolValue(), false);
}