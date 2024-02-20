#include <iostream>
#include <cctype>
#include <string>

#include "json.h"

Json::ParsingState Json::parsing_state{ Json::ParsingState::Undefined };

Json::Json(std::string key, Json* parent)
  : key_{ key }
  , parent_ { parent }
  , value_type_{ ValueType::Null }
  , number_{ 0.0l }
{

}

Json::Json(const Json& obj)
: parent_{ nullptr }
, key_{obj.key_}
, value_type_{obj.value_type_}
, string_{obj.string_}
, number_{obj.number_}
{
  objects_.reserve(obj.objects_.size());

  for (auto& json : obj.objects_)
  {
    objects_.push_back(std::make_unique<Json>(*json));
  }
}

Json::Json(Json&& obj) noexcept
  : parent_{ nullptr }
  , key_{ "" }
  , value_type_{ ValueType::Undefined }
  , string_{ "" }
  , number_{ 0.0 }
{
  std::swap(parent_, obj.parent_);
  std::swap(key_, obj.key_);
  std::swap(value_type_, obj.value_type_);
  std::swap(string_, obj.string_);
  std::swap(number_, obj.number_);
}

Json& Json::operator=(const Json& obj)
{
  objects_.reserve(obj.objects_.size());

  for (auto& json : obj.objects_)
  {
    objects_.push_back(std::make_unique<Json>(*json));
  }
  return *this;
}

Json& Json::operator=(Json&& obj) noexcept
{
  parent_ = obj.parent_;
  key_ = obj.key_;
  value_type_ = obj.value_type_;
  string_ = obj.string_;
  number_ = obj.number_;

  obj.parent_ = nullptr;
  obj.key_ = "";
  obj.value_type_ = ValueType::Undefined;
  obj.string_ = "";
  obj.number_ = 0;

  return *this;
}

Json& Json::AddNewPair()
{
  objects_.push_back(std::make_unique<Json>("", this));
  return *objects_.back();
}

Json& Json::AddNewPair(ValueType value_type)
{
  objects_.push_back(std::make_unique<Json>("", this));
  auto& added_object = *objects_.back();
  added_object.SetType(value_type);
  return added_object;
}

void Json::SetKey(std::string_view key)
{
  key_ = key;
}

void Json::SetType(ValueType type)
{
  value_type_ = type;
}

void Json::SetValue(const std::string& value)
{
  switch (value_type_)
  {
  //case Json::ValueType::Undefined:
  //  break;

  case Json::ValueType::String:
    string_ = value;
    break;

  case Json::ValueType::Number:
    number_ = std::stold(value);
    break;

  //case Json::ValueType::Object:
  //  break;

  //case Json::ValueType::Array:
  //  break;

  default:
    break;
  }

}

void Json::ConvertToArray()
{
  if (value_type_ == ValueType::Array) return;

  std::unique_ptr<Json> copy = std::make_unique<Json>(std::move(*this));
  this->SetType(ValueType::Array);
  this->key_ = copy->key_;

  copy->key_ = "";
  copy->SetParent(this);
  this->objects_.push_back(std::move(copy));
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

void Json::SetParent(Json* parent)
{
  parent_ = parent;
}

std::unique_ptr<Json> Json::Parse(const std::string& data)
{
  auto root = std::make_unique<Json>("", nullptr);
  root->SetType(ValueType::Undefined);
  auto current = root.get();

  auto it = std::begin(data);
  auto end = std::end(data);

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
      break;

    case '{':
      current->SetType(ValueType::Object);
      parsing_state = ParsingState::Object;
      break;

    case '[':
      current->SetType(ValueType::Array);
      parsing_state = ParsingState::Array;
      break;

    default:
      parsing_state = ParsingState::Undefined;
      break;
    }

    if (parsing_state != ParsingState::Undefined)
    {
      GetParsingMethod(parsing_state)(++it, end, current);
    }
    else
    {
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
        current->SetValue(value);
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
        current->SetValue(value);
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
      if (current->objects_.size() == 1 &&
        (*current->objects_.back()).GetType() == ValueType::Undefined)
      {
        current->objects_.pop_back();
      }
      return;

    case ',':
      break;

    case '\"':
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
      current = current->parent_;
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
        current->SetType(ValueType::True);
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
        current->SetType(ValueType::False);
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
      GetParsingMethod(parsing_state)(ch, end, current);
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

Json* Json::AddChild(std::string_view data, std::string_view key)
{
  if (key.empty()) return nullptr;
  if (this->value_type_ != ValueType::Object) return nullptr;

  this->AddNewPair(ValueType::String);
  auto& new_object = objects_.back();
  new_object->key_ = key;
  new_object->string_ = data;

  return new_object.get();
}

Json* Json::AddChild(bool data, std::string_view key)
{
  if (key.empty()) return nullptr;
  if (this->value_type_ != ValueType::Object) return nullptr;

  ValueType value_type = data == true ? ValueType::True : ValueType::False;
  this->AddNewPair(value_type);
  auto& new_object = objects_.back();
  new_object->key_ = key;

  return new_object.get();
}

Json* Json::AddChild(std::string_view key)
{
  if (key.empty()) return nullptr;
  if (this->value_type_ != ValueType::Object) return nullptr;

  this->AddNewPair(ValueType::Null);
  auto& new_object = objects_.back();
  new_object->key_ = key;

  return new_object.get();
}

Json* Json::AddChild(long data, std::string_view key)
{
  if (key.empty()) return nullptr;
  if (this->value_type_ != ValueType::Object) return nullptr;

  this->AddNewPair(ValueType::Number);
  auto& new_object = objects_.back();
  new_object->key_ = key;
  new_object->number_ = data;

  return new_object.get();
}

Json* Json::AddChild(long double data, std::string_view key)
{
  if (key.empty()) return nullptr;
  if (this->value_type_ != ValueType::Object) return nullptr;

  this->AddNewPair(ValueType::Number);
  auto& new_object = objects_.back();
  new_object->key_ = key;
  new_object->number_ = data;

  return new_object.get();
}
