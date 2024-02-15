#ifndef JSON_H
#define JSON_H

#include <string>
#include <vector>
#include <memory>

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

  Json(std::string name, Json* parent);

  Json& AddNewPair();
  void SetName(std::string_view name);
  void SetValueType(ValueType type);
  void SetValue(const std::string& value);
  Json::ValueType GetType() const;

  std::string& GetName();

  static std::unique_ptr<Json> Parse(const std::string& data);

private:

  enum class ParsingState {
    Undefined = -1,
    Object,
    Array,
    ObjectName,
    Value,
    String,
    Number,
    BooleanNull,
    Null,
    EscapeChar
  };

  using char_iterator = std::string::const_iterator;
  static void ParseString(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseArray(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseObject(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseValue(char_iterator& ch, char_iterator& end, Json* current);
  //static void ParseString(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseNumber(char_iterator& ch, char_iterator& end, Json* current);
  //static void ParseBoolNull(char_iterator& ch, char_iterator& end, Json* current);
  static void ParseEscapeChar(char_iterator& ch, char_iterator& end, std::string& str);

  static decltype(&ParseObject) GetParsingMethod(ParsingState state);

  static bool ExpectKeyword(char_iterator& ch, char_iterator& end, std::string expected_value);




  Json* parent_;
  std::string name_;
  ValueType value_type_;

  //Values
  std::string string_;
  long double number_;
  std::vector<std::unique_ptr<Json>> objects_;

  static ParsingState parsing_state;
};

#endif // !JSON_H
