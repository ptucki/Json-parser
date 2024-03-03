#ifndef JSON_H
#define JSON_H

#include <string>
#include <vector>
#include <concepts>
#include <memory>
#include <variant>
#include <any>
#include <functional>
#include <utility>
#include <type_traits>

class Json;

template<typename T, typename... U>
concept any_of = std::disjunction_v<std::is_same<T, U>...>;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

using ChildrenList = std::vector<std::unique_ptr<Json>>;
using Bool = bool;
using Number = double;
using String = std::string;
using Array = std::vector<std::unique_ptr<Json>>;

using JsonValue = std::variant<String, Number, Bool, ChildrenList>;

template<class T>
concept StringLike = std::is_convertible_v<T, std::string_view>;

class Json {

  using char_iterator = std::string::const_iterator;
  using ParsingMethodType = void(*)(char_iterator&, char_iterator&, Json*);

public:

  enum class ValueType {
    Undefined = -1,
    String,
    Number,
    Bool,
    Object,
    Array,
    Null,
  };

  Json();
  Json(std::string key, Json* parent);
  Json(const Json& obj);
  Json(Json&& obj) noexcept;
  Json& operator=(const Json& obj);
  Json& operator=(Json&& obj) noexcept;
  ~Json() = default;

  /* Accessors and mutators */
  Json::ValueType GetType() const;
  const std::string& GetKey() const;
  Json* GetParent() const;
  const JsonValue& GetValue() const;

  void SetKey(std::string name);
  void SetParent(Json* parent);

  template<StringLike T>
  void SetValue(T&& data);

  template<Arithmetic T>
  void SetValue(T&& data);

  void SetValue(bool data);
  void ClearValue();
  std::unique_ptr<Json> Detach();
  bool RemoveChild(int index);

  bool IsValid() const;
  bool IsRoot() const;
  bool IsArrayElement() const;
  bool IsLastChild() const;

  Json* operator[](std::string_view key);
  Json* operator[](int index);

  /* Object maniputaion methods */
  /* - Add child elements */
  template<typename T>
  Json* AddChild(T&& data, std::string key);

  template<typename T>
  Json* AddValue(T&& data);

  void ForEachChild(const std::function<void(const Json&)>& function) const;

  /* Json conversion to string */
  std::string ToString() const;

  /* Parsing methods */
  static std::unique_ptr<Json> Parse(const std::string& data);

private:

  enum class ParsingState {
    Undefined = -1,
    Object,
    Array,
    Key,
    Value,
    String,
    Number,
    BooleanNull,
    Null,
    EscapeChar,
    Started,
    Finished
  };

  /* Accessors and mutators */
  void SetType(ValueType type);

  /* Parsing methods */
  static void ParseString(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseArray(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseObject(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseValue(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseNumber(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseEscapeChar(char_iterator& ch, char_iterator& end, std::string& str);
  static ParsingMethodType GetParsingMethod(ParsingState state);
  static bool ExpectKeyword(char_iterator& ch, char_iterator& end, std::string expected_value);
  void SetParsedValue(const std::string& value);

  /* Maniputaion methods */
  Json& AddNewPair();
  Json& AddNewPair(ValueType value_type);
  
  void ConvertToArray();

  /* Json conversion to string */
  void ToString(std::string& str) const;

  Json* parent_;
  std::string key_;

  //Values
  ValueType value_type_;
  JsonValue value_;

  static ParsingState parsing_state;
};


template<StringLike T>
void Json::SetValue(T&& data) {
  value_ = std::string(std::forward<T>(data));
  value_type_ = ValueType::String;
}

template<Arithmetic T>
void Json::SetValue(T&& data) {
  if (std::is_same_v<T, bool>)
  {
    value_ = static_cast<bool>(std::forward<T>(data));
    value_type_ = ValueType::Bool;
  }
  else {
    value_ = static_cast<double>(std::forward<T>(data));
    value_type_ = ValueType::Number;
  }
}


template<typename T>
Json* Json::AddChild(T&& data, std::string key)
{
  if (value_type_ != ValueType::Object && value_type_ != ValueType::Null) return nullptr;

  if (!std::holds_alternative<ChildrenList>(value_))
  {
    value_type_ = ValueType::Object;
    value_ = ChildrenList();
  }

  auto& children = std::get<ChildrenList>(value_);
  children.push_back(std::make_unique<Json>());

  auto& newObj = *children.back();

  newObj.parent_ = this;
  newObj.key_ = key;
  
  newObj.SetValue(std::forward<T>(data));

  return &newObj;
}

template<typename T>
Json* Json::AddValue(T&& data)
{
  if (value_type_ == ValueType::Null)
  {
    value_type_ = ValueType::Array;
    value_ = ChildrenList();
  }

  if (value_type_ != ValueType::Array) ConvertToArray();

  if (std::holds_alternative<ChildrenList>(value_))
  {
    auto& children = std::get<ChildrenList>(value_);
    children.push_back(std::make_unique<Json>());
    children.back()->SetValue(std::forward<T>(data));
    children.back()->SetParent(this);

    return children.back().get();
  }

  return nullptr;
}

#endif // !JSON_H
