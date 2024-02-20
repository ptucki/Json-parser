#ifndef JSON_H
#define JSON_H

#include <string>
#include <vector>
#include <concepts>
#include <memory>
#include <tuple>

class Json {
public:

  enum class ValueType {
    Undefined = -1,
    String,
    Number,
    Object,
    Array,
    True,
    False,
    Null,
  };

  Json(std::string key, Json* parent);
  Json(const Json& obj);
  Json(Json&& obj) noexcept;
  Json& operator=(const Json& obj);
  Json& operator=(Json&& obj) noexcept;
  ~Json() {}

  /* Accessors and mutators */
  Json::ValueType GetType() const;
  const std::string& GetKey() const;
  Json* GetParent() const;

  void SetKey(std::string_view name);
  void SetType(ValueType type);
  void SetParent(Json* parent);

  /* Object maniputaion methods */
  /* - Add child elements */
  Json* AddChild(long double data, std::string_view key);
  Json* AddChild(long data, std::string_view key);
  Json* AddChild(std::string_view data, std::string_view key);
  Json* AddChild(bool data, std::string_view key);
  Json* AddChild(std::string_view key);

  template<typename T>
  inline typename std::enable_if<std::is_same<std::remove_reference_t<T>, Json>::value, Json*>::type
    AddChild(T&& json_object);

  static std::unique_ptr<Json> Parse(const std::string& data);

private:

  using char_iterator     = std::string::const_iterator;
  using ParsingMethodType = void(*)(char_iterator&, char_iterator&, Json*);

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
    EscapeChar
  };

  /* Parsing methods */
  static void ParseString(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseArray(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseObject(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseValue(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseNumber(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseEscapeChar(char_iterator& ch, char_iterator& end, std::string& str);
  static ParsingMethodType GetParsingMethod(ParsingState state);
  static bool ExpectKeyword(char_iterator& ch, char_iterator& end, std::string expected_value);

  /* Maniputaion methods */
  Json& AddNewPair();
  Json& AddNewPair(ValueType value_type);
  void SetValue(const std::string& value);

  Json* parent_;
  std::string key_;

  //Values
  ValueType value_type_;
  std::string string_;
  long double number_;
  std::vector<std::unique_ptr<Json>> objects_;

  static ParsingState parsing_state;
};

template<typename T>
inline typename std::enable_if<std::is_same<std::remove_reference_t<T>, Json>::value, Json*>::type
Json::AddChild(T&& json_object)
{

  if (this->value_type_ != ValueType::Object) return nullptr;

  auto obj = std::make_unique<Json>(std::forward<T>(json_object));
  obj->SetParent(this);
  objects_.push_back(std::move(obj));
  
  return objects_.back().get();
}

#endif // !JSON_H
