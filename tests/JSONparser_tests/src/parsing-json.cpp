#include "parsing-json.h"

TEST_F(ParsingTests, EmptyObject) {
  auto json = Json::Parse("{}");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& children = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(children));
  EXPECT_EQ(std::get<ChildrenList>(children).size(), 0);
}

TEST_F(ParsingTests, EmptyArray) {
  auto json = Json::Parse("[]");

  EXPECT_EQ(json->GetType(), Json::ValueType::Array);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& children = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(children));
  EXPECT_EQ(std::get<ChildrenList>(children).size(), 0);
}

TEST_F(ParsingTests, FrontTrailingWhitespaces) {
  auto json = Json::Parse(
    R"(   
 [])"
  );

  EXPECT_EQ(json->GetType(), Json::ValueType::Array);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));
  EXPECT_EQ(std::get<ChildrenList>(value).size(), 0);
}

TEST_F(ParsingTests, BackTrailingWhitespaces) {
  auto json = Json::Parse(
    R"([]  
  )"
  );

  EXPECT_EQ(json->GetType(), Json::ValueType::Array);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));
  EXPECT_EQ(std::get<ChildrenList>(value).size(), 0);
}

TEST_F(ParsingTests, TrailingWhitespaces) {
  auto json = Json::Parse(
    R"(  
[]  
  )"
  );

  EXPECT_EQ(json->GetType(), Json::ValueType::Array);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));
  EXPECT_EQ(std::get<ChildrenList>(value).size(), 0);
}

TEST_F(ParsingTests, ObjectWithValidNumberElement) {
  auto json = Json::Parse(R"({ "key" : 14 })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Number);
  EXPECT_EQ(std::get<Number>(child_element->GetValue()), 14);
}

TEST_F(ParsingTests, ObjectWithValidStringElement) {
  auto json = Json::Parse(R"({ "key" : "value" })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::String);
  EXPECT_EQ(std::get<String>(child_element->GetValue()), "value");
}

TEST_F(ParsingTests, ObjectWithValidTrueElement) {
  auto json = Json::Parse(R"({ "key" : true })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Bool);
  EXPECT_EQ(std::get<Bool>(child_element->GetValue()), true);
}

TEST_F(ParsingTests, ObjectWithValidFalseElement) {
  auto json = Json::Parse(R"({ "key" : false })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Bool);
  EXPECT_EQ(std::get<Bool>(child_element->GetValue()), false);
}

TEST_F(ParsingTests, ObjectWithValidNullElement) {
  auto json = Json::Parse(R"({ "key" : null })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Null);
}

TEST_F(ParsingTests, ObjectWithValidEmptyArrayElement) {
  auto json = Json::Parse(R"({ "key" : [] })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Array);
  EXPECT_EQ(std::get<Array>(child_element->GetValue()).size(), 0);
}

TEST_F(ParsingTests, ObjectWithValidEmptyObjectElement) {
  auto json = Json::Parse(R"({ "key" : {} })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Object);
  EXPECT_EQ(std::get<ChildrenList>(child_element->GetValue()).size(), 0);
}

TEST_F(ParsingTests, ObjectWithValidMultipleElements) {
  auto json = Json::Parse(R"({ "key1" : {}, "key2" : 13.4 })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 2);

  // First element
  auto& child_element1 = json_children.at(0);
  EXPECT_STREQ(child_element1->GetKey().c_str(), "key1");
  EXPECT_EQ(child_element1->GetParent(), json.get());
  EXPECT_EQ(child_element1->GetType(), Json::ValueType::Object);
  EXPECT_EQ(std::get<ChildrenList>(child_element1->GetValue()).size(), 0);

  // Second element
  auto& child_element2 = json_children.at(1);
  EXPECT_STREQ(child_element2->GetKey().c_str(), "key2");
  EXPECT_EQ(child_element2->GetParent(), json.get());
  EXPECT_EQ(child_element2->GetType(), Json::ValueType::Number);
  EXPECT_EQ(std::get<Number>(child_element2->GetValue()), 13.4);
}

TEST_F(ParsingTests, ObjectWithNestedObjects) {
  auto json = Json::Parse(R"({ "key1" : { "key2" : 3.14 } })");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key1");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Object);

  auto& nested_children = std::get<ChildrenList>(child_element->GetValue());
  EXPECT_EQ(nested_children.size(), 1);

  auto& nested = nested_children.front();
  EXPECT_STREQ(nested->GetKey().c_str(), "key2");
  EXPECT_EQ(nested->GetParent(), child_element.get());
  EXPECT_EQ(nested->GetType(), Json::ValueType::Number);
  EXPECT_EQ(std::get<Number>(nested->GetValue()), 3.14);
}

TEST_F(ParsingTests, ObjectWithNestedArrays) {
  auto json = Json::Parse(R"({ "key1" : ["value1", 3.14, [{ "key" : "value"}, true]]})");

  EXPECT_EQ(json->GetType(), Json::ValueType::Object);
  EXPECT_EQ(json->GetParent(), nullptr);
  EXPECT_EQ(json->GetKey(), "");

  auto& value = json->GetValue();
  ASSERT_TRUE(std::holds_alternative<ChildrenList>(value));

  auto& json_children = std::get<ChildrenList>(value);
  ASSERT_EQ(json_children.size(), 1);

  auto& child_element = json_children.front();
  EXPECT_STREQ(child_element->GetKey().c_str(), "key1");
  EXPECT_EQ(child_element->GetParent(), json.get());
  EXPECT_EQ(child_element->GetType(), Json::ValueType::Array);

  auto& array_values = std::get<ChildrenList>(child_element->GetValue());
  EXPECT_EQ(array_values.size(), 3);

  auto& element1 = array_values.at(0);
  EXPECT_STREQ(element1->GetKey().c_str(), "");
  EXPECT_EQ(element1->GetParent(), child_element.get());
  EXPECT_EQ(element1->GetType(), Json::ValueType::String);
  EXPECT_EQ(std::get<String>(element1->GetValue()), "value1");

  auto& element2 = array_values.at(1);
  EXPECT_STREQ(element2->GetKey().c_str(), "");
  EXPECT_EQ(element2->GetParent(), child_element.get());
  EXPECT_EQ(element2->GetType(), Json::ValueType::Number);
  EXPECT_EQ(std::get<Number>(element2->GetValue()), 3.14);

  auto& element3 = array_values.at(2);
  EXPECT_STREQ(element3->GetKey().c_str(), "");
  EXPECT_EQ(element3->GetParent(), child_element.get());
  EXPECT_EQ(element3->GetType(), Json::ValueType::Array);

  ASSERT_TRUE(std::holds_alternative<ChildrenList>(element3->GetValue()));

  auto& element3_values = std::get<ChildrenList>(element3->GetValue());
  ASSERT_EQ(element3_values.size(), 2);

  auto& nested1 = element3_values.at(0);
  EXPECT_STREQ(nested1->GetKey().c_str(), "");
  EXPECT_EQ(nested1->GetParent(), element3.get());
  EXPECT_EQ(nested1->GetType(), Json::ValueType::Object);
  EXPECT_EQ(std::get<ChildrenList>(nested1->GetValue()).size(), 1);

  auto& nested2 = element3_values.at(1);
  EXPECT_STREQ(nested2->GetKey().c_str(), "");
  EXPECT_EQ(nested2->GetParent(), element3.get());
  EXPECT_EQ(nested2->GetType(), Json::ValueType::Bool);
  EXPECT_EQ(std::get<Bool>(nested2->GetValue()), true);
}