#include <iostream>
#include <cctype>
#include <cassert>
#include <string>
#include <format>

#include "json.h"

Json::ParsingState Json::parsing_state{ Json::ParsingState::Undefined };

Json::Json()
  : key_{ "" }
  , parent_{ nullptr }
  , value_type_{ ValueType::Null }
{
}

Json::Json(std::string key, Json* parent)
  : key_{ key }
  , parent_ { parent }
  , value_type_{ ValueType::Null }
{

}

Json::Json(const Json& obj)
: parent_{ nullptr }
, key_{obj.key_}
, value_type_{obj.value_type_}
{
  if (std::holds_alternative<String>(obj.value_)) value_ = std::get<String>(obj.value_);
  if (std::holds_alternative<Number>(obj.value_)) value_ = std::get<Number>(obj.value_);
  if (std::holds_alternative<Bool>(obj.value_)) value_ = std::get<Bool>(obj.value_);

  if (std::holds_alternative<ChildrenList>(obj.value_))
  {
    auto& objChildren = std::get<ChildrenList>(obj.value_);
    
    value_ = ChildrenList();
    auto& copyChildren = std::get<ChildrenList>(value_);
    copyChildren.reserve(objChildren.size());


    for (auto& json : objChildren)
    {
      copyChildren.push_back(std::make_unique<Json>(*json));
      copyChildren.back()->parent_ = this;
    }
  }
}

Json::Json(Json&& obj) noexcept
  : parent_{ nullptr }
  , key_{ "" }
  , value_type_{ ValueType::Undefined }
{
  std::swap(parent_, obj.parent_);
  std::swap(key_, obj.key_);
  std::swap(value_type_, obj.value_type_);
  std::swap(value_, obj.value_);
}

Json& Json::operator=(const Json& obj)
{
  if (std::holds_alternative<String>(obj.value_)) value_ = std::get<String>(obj.value_);
  if (std::holds_alternative<Number>(obj.value_)) value_ = std::get<Number>(obj.value_);
  if (std::holds_alternative<Bool>(obj.value_)) value_ = std::get<Bool>(obj.value_);

  if (std::holds_alternative<ChildrenList>(obj.value_))
  {
    auto& objChildren = std::get<ChildrenList>(obj.value_);

    value_ = ChildrenList();
    auto& copyChildren = std::get<ChildrenList>(value_);
    copyChildren.reserve(objChildren.size());

    for (auto& json : objChildren)
    {
      copyChildren.push_back(std::make_unique<Json>(*json));
      copyChildren.back()->parent_ = this;
    }
  }

  return *this;
}

Json& Json::operator=(Json&& obj) noexcept
{
  parent_ = obj.parent_;
  key_ = obj.key_;
  value_type_ = obj.value_type_;

  obj.parent_ = nullptr;
  obj.key_ = "";
  obj.value_type_ = ValueType::Undefined;
  value_ = std::move(obj.value_);

  return *this;
}

Json& Json::AddNewPair()
{
  if (!std::holds_alternative<ChildrenList>(value_))
  {
    value_ = ChildrenList();
  }
  auto& list = std::get<ChildrenList>(value_);
  list.push_back(std::make_unique<Json>("", this));
  return *list.back();
}

Json& Json::AddNewPair(ValueType value_type)
{
  auto& list = std::get<ChildrenList>(value_);
  list.push_back(std::make_unique<Json>("", this));
  auto& added_object = *list.back();
  added_object.SetType(value_type);
  return added_object;
}

void Json::SetKey(std::string key)
{
  key_ = key;
}

void Json::SetType(ValueType type)
{
  value_type_ = type;
}

void Json::SetParsedValue(const std::string& value)
{
  switch (value_type_)
  {
  case Json::ValueType::String:
    value_ = value;
    break;
  case Json::ValueType::Number:
    value_ = std::stod(value);
    break;
  default:
    break;
  }

}

void Json::ConvertToArray()
{
  std::unique_ptr<Json> copy = std::make_unique<Json>(std::move(*this));
  this->SetType(ValueType::Array);
  this->key_ = copy->key_;
  this->parent_ = copy->parent_;

  copy->key_ = "";
  copy->SetParent(this);
  this->value_ = ChildrenList();
  auto& children = std::get<ChildrenList>(value_);

  children.push_back(std::move(copy));
}

void Json::ToString(std::string& str) const
{

  switch (value_type_)
  {
  case Json::ValueType::String:
    {
      auto& value = std::get<String>(value_);

      if (IsArrayElement())
      {
        str += std::format("\"{}\"", value);
      }
      else
      {
        str += std::format("\"{}\" : \"{}\"", key_, value);
      }
    }
    break;

  case Json::ValueType::Number:
    {
      auto& value = std::get<Number>(value_);

      if (IsArrayElement())
      {
        str += std::to_string(value);
      }
      else
      {
        str += std::format("\"{}\" : {}", key_, std::to_string(value));
      }
    }
    break;

  case Json::ValueType::Bool:
    {
      auto& value = std::get<Bool>(value_);

      if (IsArrayElement())
      {
        str += (value == true ? "true" : "false");
      }
      else
      {
        std::string bool_value = value == true ? "true" : "false";
        str += std::format("\"{}\" : {}", key_, bool_value);
      }
    }
    break;

  case Json::ValueType::Object:
    {
      if (IsArrayElement() || IsRoot())
      {
        str.append(1, '{');
      }
      else
      {
        str += std::format("\"{}\" : {{", key_);
      }
      
      ForEachChild([&str](const Json& child) -> void {
        child.ToString(str);
        if (!child.IsLastChild()) str.append(1, ',');
        });
      str.append(1, '}');
    }
    break;

  case Json::ValueType::Array:

    if (IsArrayElement())
    {
      str.append(1, '[');
    }
    else
    {
      str += std::format("\"{}\" : [", key_);
    }

    ForEachChild([&str](const Json& child) -> void {
      child.ToString(str);
      if (!child.IsLastChild()) str.append(1, ',');
      });
    str.append(1, ']');
    break;

  case Json::ValueType::Null:
  {
    if (IsArrayElement())
    {
      str += "null";
    }
    else
    {
      str += std::format("\"{}\" : null", key_);
    }
  }
    break;

  default:
    str.clear();
    return;
  }
}

Json::ValueType Json::GetType() const
{
  return value_type_;
}

const std::string& Json::GetKey() const
{
  return key_;
}

Json* Json::GetParent() const
{
  return parent_;
}

const JsonValue& Json::GetValue() const
{
  return value_;
}

void Json::SetParent(Json* parent)
{
  parent_ = parent;
}

void Json::SetValue(bool data)
{
  value_ = data;
  value_type_ = ValueType::Bool;
}

void Json::ClearValue()
{
  value_ = JsonValue();
  value_type_ = ValueType::Null;
}

std::unique_ptr<Json> Json::Detach()
{
  if (this->IsRoot()) return std::unique_ptr<Json>(nullptr);

  std::unique_ptr<Json> json;

  auto parent = this->GetParent();

  auto& value = parent->value_;

  if (std::holds_alternative<ChildrenList>(value))
  {
    auto& children = std::get<ChildrenList>(value);

    for (auto it = std::begin(children); it != std::end(children); it++)
    {
      if (it->get() == this)
      {
        json = std::unique_ptr<Json>(it->release());
        children.erase(it);
        return json;
      }
    }
  }

  return std::unique_ptr<Json>(nullptr);
}

bool Json::RemoveChild(int index)
{
  if (!std::holds_alternative<ChildrenList>(value_)) return false;

  auto& children = std::get<ChildrenList>(value_);
  children.erase(std::begin(children) + index);

  return true;
}

bool Json::IsValid() const
{
  return value_type_ == ValueType::Undefined ? false : true;
}

bool Json::IsRoot() const
{
  return parent_ == nullptr ? true : false;
}

bool Json::IsArrayElement() const
{
  if (parent_ == nullptr) return false;
  return parent_->GetType() == ValueType::Array;
}

bool Json::IsLastChild() const
{
  auto& parents_value = std::get<ChildrenList>(parent_->value_);
  return parents_value.back().get() == this;
}

Json* Json::operator[](std::string_view key)
{
  if (value_type_ != ValueType::Object) return nullptr;
  if (!std::holds_alternative<ChildrenList>(value_)) return nullptr;

  for (auto& child : std::get<ChildrenList>(value_))
  {
    if (child->GetKey() == key) return child.get();
  }

  return nullptr;
}

Json* Json::operator[](int index)
{
  if (value_type_ != ValueType::Object && value_type_ != ValueType::Array) return nullptr;
  if (!std::holds_alternative<ChildrenList>(value_)) return nullptr;



  return std::get<ChildrenList>(value_)[index].get();
}

void Json::ForEachChild(const std::function<void(const Json&)>& function) const
{
  if (std::holds_alternative<ChildrenList>(value_))
  {
    auto& children = std::get<ChildrenList>(value_);

    for (auto& element : children)
    {
      function(*element);
    }
  }
}

std::string Json::ToString() const
{
  std::string json_string;

  this->ToString(json_string);

  return json_string;
}

std::unique_ptr<Json> Json::Parse(const std::string& data)
{
  auto root = std::make_unique<Json>("", nullptr);
  root->SetType(ValueType::Undefined);
  auto current = root.get();

  auto it = std::begin(data);
  auto end = std::end(data);

  parsing_state = ParsingState::Started;

  for (; it != end; it++)
  {
    switch (*it)
    {
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
    case ' ':
      //ignore trailing whitespaces
      while (it != end && std::isspace(*it)) it++;
      it--;
      break;

    case '{':
      current->SetType(ValueType::Object);
      parsing_state = ParsingState::Object;
      break;

    case '[':
      current->SetType(ValueType::Array);
      current->value_ = ChildrenList();
      parsing_state = ParsingState::Array;
      break;

    default:
      parsing_state = ParsingState::Undefined;
      break;
    }

    if ( parsing_state != ParsingState::Undefined
      && parsing_state != ParsingState::Finished
      && parsing_state != ParsingState::Started )
    {
      if ((it + 1) != end)
      {
        GetParsingMethod(parsing_state)(++it, end, current);
        parsing_state = current->IsRoot() ? ParsingState::Finished : parsing_state;
      }
      else
      {
        parsing_state = ParsingState::Undefined;
      }
    }

    if (parsing_state == ParsingState::Undefined)
    {
      root = std::make_unique<Json>();
      root->SetType(ValueType::Undefined);
      break;
    }
  }

  return root;
}

void Json::ParseString(char_iterator& ch, char_iterator& end, Json* current)
{
  std::string value = "";
  ++ch;

  while (ch != end)
  {
    switch (*ch)
    {
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
      parsing_state = ParsingState::Undefined;
      return;

    case '\\':
      parsing_state = ParsingState::EscapeChar;
      ParseEscapeChar(ch, end, value);
      break;

    case '\"':
      if (current->GetType() == ValueType::Object)
      {
        if (parsing_state == ParsingState::Key) {
          current->SetKey(value);
        }
        parsing_state = ParsingState::Object;
        return;
      }
      else if (current->GetType() == ValueType::String)
      {
        current->SetParsedValue(value);
        parsing_state = ParsingState::Object;
        return;
      }
      
      else if (current->GetType() == ValueType::Array) parsing_state = ParsingState::Array;
      return;
    default:
      value.append(1, *ch);

      break;
    }
    ch++;
  }
}

void Json::ParseNumber(char_iterator& ch, char_iterator& end, Json* current)
{
  std::string value = "";
  bool decimal_point = false;
  value.append(1, *ch);
  ++ch;

  while (ch != end)
  {
    switch (*ch)
    {
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
    case ' ':
    case ',':
    case '}':
    case ']':
      if (value.back() >= '0' || value.back() <= '9')
      {
        current->SetParsedValue(value);
        ch--;
        parsing_state = ParsingState::Object;
      }
      else
      {
        parsing_state = ParsingState::Undefined;
      }
      return;

    case '.':
      if (value.size() == 1 && value[0] == '-' || decimal_point == true)
      {
        parsing_state = ParsingState::Undefined;
      }
      else
      {
        decimal_point = true;
        value.append(1, *ch);
      }
      break;

    case 'e':
    case 'E':
      if (value.back() >= '0' || value.back() <= '9')
      {
        value.append(1, *ch);
      }
      else
      {
        parsing_state = ParsingState::Undefined;
        return;
      }
      break;

    case '-':
      if (value.back() >= 'E' || value.back() <= 'e')
      {
        parsing_state = ParsingState::Undefined;
        return;
      }
      else
      {
        value.append(1, *ch);
      }
      break;

    default:
      if (*ch >= '0' || *ch <= '9')
      {
        value.append(1, *ch);
      }
      
      break;
    }
    ch++;
  }
}

void Json::ParseEscapeChar(char_iterator& ch, char_iterator& end, std::string& str)
{
  ++ch;

  switch (*ch)
  {
  case 'n':
    str.append(1, '\n');
    break;

  case 't':
    str.append(1, '\t');
    break;

  case 'f':
    str.append(1, '\f');
    break;

  case 'v':
    str.append(1, '\v');
    break;

  case 'b':
    str.append(1, '\b');
    break;

  case 'r':
    str.append(1, '\r');
    break;

  case '\\':
    str.append(1, '\\');
    break;
  case '\"':
    str.append(1, '\"');
    break;

  case '\'':
    str.append(1, '\'');
    break;

  case '\?':
    str.append(1, '\?');
    break;

  case '\a':
    str.append(1, '\a');
    break;

  default:
    parsing_state = ParsingState::Undefined;
    break;
  }
  parsing_state = ParsingState::String;
}

void Json::ParseArray(char_iterator& ch, char_iterator& end, Json* current)
{
  while (ch != end)
  {
    switch (*ch)
    {
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
    case ' ':
      break;
    case ']':
      {
        auto& list = std::get<ChildrenList>(current->value_);
        if (list.size() == 1 &&
          (*list.back()).GetType() == ValueType::Undefined)
        {
          list.pop_back();
        }
      }
      return;

    case ',':
      break;

    case '\"':
    case '[':
    case 't':
    case 'f':
    case 'n':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
    case '-':
      current = &current->AddNewPair();
      parsing_state = ParsingState::Value;
      GetParsingMethod(parsing_state)(ch, end, current);
      current = current->parent_;
      break;

    case '{':
      parsing_state = ParsingState::Value;
      current = &current->AddNewPair();
      current->SetType(ValueType::Object);
      GetParsingMethod(parsing_state)(ch, end, current);
      current = current->parent_;
      break;

    default:
      break;
    }

    ch++;
  }

}

void Json::ParseObject(char_iterator& ch, char_iterator& end, Json* current)
{
  current = &current->AddNewPair();

  current->SetType(ValueType::Object);
  std::string name = "";
  bool is_key_set = false;

  while (ch != end)
  {
    switch (*ch)
    {
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
    case ' ':
      break;

    case '\"':
      if (parsing_state == ParsingState::Object)
      {
        parsing_state = ParsingState::Key;
        GetParsingMethod(parsing_state)(ch, end, current);
        is_key_set = true;
      }
      else {
        parsing_state = ParsingState::Undefined;
        return;
      }
      break;

    case '}':
      {
        current = current->parent_;
        auto& list = std::get<ChildrenList>(current->value_);
        if (list.size() > 0 && list[0]->key_ == "")
        {
          list.pop_back();
        }
      }
      return;

    case ':':
      if (is_key_set)
      {
        parsing_state = ParsingState::Value;
        GetParsingMethod(parsing_state)(++ch, end, current);
      }
      else
      {
        parsing_state = ParsingState::Undefined;
      }
      break;

    case ',':
      current = current->parent_;
      current = &current->AddNewPair();
      current->SetType(ValueType::Object);
      break;

    default:
      name.append(1, *ch);
      break;
    }

    ch++;
  }

}

void Json::ParseValue(char_iterator& ch, char_iterator& end, Json* current)
{
  std::string value = "";

  while (ch != end)
  {
    switch (*ch)
    {
    case '\n':
    case '\f':
    case '\r':
    case '\t':
    case '\v':
    case ' ':
      break;

    case '\"':
      current->SetType(ValueType::String);
      parsing_state = ParsingState::String;
      GetParsingMethod(parsing_state)(ch, end, current);
      return;
      break;

    case 't':
      if (ExpectKeyword(ch, end, "true"))
      {
        current->SetType(ValueType::Bool);
        current->value_ = true;
        parsing_state = ParsingState::Object;
        ch += 3;
      }
      else
      {
        parsing_state = ParsingState::Undefined;
      }
      return;

    case 'f':
      if (ExpectKeyword(ch, end, "false"))
      {
        current->SetType(ValueType::Bool);
        current->value_ = false;
        parsing_state = ParsingState::Object;
        ch += 4;
      }
      else
      {
        parsing_state = ParsingState::Undefined;
      }
      return;

    case 'n':
      if (ExpectKeyword(ch, end, "null"))
      {
        current->SetType(ValueType::Null);
        parsing_state = ParsingState::Object;
        ch += 3;
      }
      else
      {
        parsing_state = ParsingState::Undefined;
      }
      return;
      
    case '{':
      parsing_state = ParsingState::Object;
      GetParsingMethod(parsing_state)(ch, end, current);
      return;

    case '[':
      current->SetType(ValueType::Array);
      parsing_state = ParsingState::Array;
      current->value_ = ChildrenList();
      GetParsingMethod(parsing_state)(++ch, end, current);
      return;

    case '-':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
      current->SetType(ValueType::Number);
      parsing_state = ParsingState::Number;
      GetParsingMethod(parsing_state)(ch, end, current);
      return;

    default:
      value.append(1, *ch);
      break;
    }

    ch++;
  }
}

Json::ParsingMethodType Json::GetParsingMethod(ParsingState state)
{
  switch (state)
  {
  case ParsingState::Object:      return ParseObject;
  case ParsingState::Array:       return ParseArray;
  case ParsingState::Key:
  case ParsingState::String:      return ParseString;
  case ParsingState::Number:      return ParseNumber;
  case ParsingState::Value:       return ParseValue;
  }
  return nullptr;
}

bool Json::ExpectKeyword(char_iterator& ch, char_iterator& end, std::string keyword)
{
  std::string temp = "";

  if (static_cast<size_t>(end - ch) > keyword.size())
  {
    temp.append(ch, ch + keyword.size());
    if (keyword == temp)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  return false;
}

