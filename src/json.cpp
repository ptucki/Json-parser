#include <iostream>
#include <cctype>
#include <string>
//#include <functional>

#include "json.h"

Json::ParsingState Json::parsing_state{ Json::ParsingState::Undefined };

Json::Json(std::string name, Json* parent)
  : name_{ name }
  , parent_ { parent }
  , value_type_{ ValueType::Undefined }
  , number_{ 0.0l }
{
  std::cout << "Json created!" << std::endl;
}

Json& Json::AddNewPair()
{
  objects_.push_back(std::make_unique<Json>("", this));
  return *objects_.back();
}

void Json::SetName(std::string_view name)
{
  name_ = name;
}

void Json::SetValueType(ValueType type)
{
  value_type_ = type;
}

void Json::SetValue(const std::string& value)
{
  switch (value_type_)
  {
  case Json::ValueType::Undefined:
    break;

  case Json::ValueType::String:
    string_ = value;
    break;

  case Json::ValueType::Number:
    number_ = std::stold(value);
    break;

  case Json::ValueType::Object:
    break;

  case Json::ValueType::Array:
    break;

  //case Json::ValueType::Boolean:
    //boolean_ = value == "true" ? true : false;
    //break;

  //case Json::ValueType::Null:
    //null_ = true;
    //break;

  default:
    break;
  }

}

Json::ValueType Json::GetType() const
{
  return value_type_;
}

std::string& Json::GetName()
{
  return name_;
}

std::unique_ptr<Json> Json::Parse(const std::string& data)
{
  auto root = std::make_unique<Json>("", nullptr);
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
      current->SetValueType(ValueType::Object);
      parsing_state = ParsingState::Object;
      break;

    case '[':
      current->SetValueType(ValueType::Array);
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
        if (parsing_state == ParsingState::ObjectName) {
          current->SetName(value);
        }
        //else if (parsing_state == ParsingState::String)
        //{
        //  current->SetValue(value);
        //}
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

//void Json::ParseBoolNull(char_iterator& ch, char_iterator& end, Json* current)
//{
//
//}

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
  current = &current->AddNewPair();
  current->SetValueType(ValueType::Array);

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
      //current = current->parent_;
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
      current->SetValueType(ValueType::Object);
      GetParsingMethod(parsing_state)(ch, end, current);
      current = current->parent_;
      break;
      //parsing_state = ParsingState::Object;
      //GetParsingMethod(parsing_state)(ch, end, current);
      //current = current->parent_;
      //break;

    default:
      break;
    }

    ch++;
  }

}

void Json::ParseObject(char_iterator& ch, char_iterator& end, Json* current)
{
  current = &current->AddNewPair();
  current->SetValueType(ValueType::Object);
  std::string name = "";
  bool is_name_set = false;

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
        parsing_state = ParsingState::ObjectName;
        GetParsingMethod(parsing_state)(ch, end, current);
        is_name_set = true;
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
      if (is_name_set)
      {
        parsing_state = ParsingState::Value;
        GetParsingMethod(parsing_state)(++ch, end, current);
      }
      else
      {
        parsing_state = ParsingState::Undefined;
        // ':' should be after the declaration of name
      }
      break;

    case ',':
      current = current->parent_;
      current = &current->AddNewPair();
      current->SetValueType(ValueType::Object);
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
      current->SetValueType(ValueType::String);
      parsing_state = ParsingState::String;
      GetParsingMethod(parsing_state)(ch, end, current);
      return;
      break;

    case 't':
      if (ExpectKeyword(ch, end, "true"))
      {
        current->SetValueType(ValueType::True);
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
        current->SetValueType(ValueType::False);
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
        current->SetValueType(ValueType::Null);
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
      current->SetValueType(ValueType::Number);
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

decltype(&Json::ParseObject) Json::GetParsingMethod(ParsingState state)
{
  switch (state)
  {
  case ParsingState::Object:      return ParseObject;
  case ParsingState::Array:       return ParseArray;
  case ParsingState::ObjectName:
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
