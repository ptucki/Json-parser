#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <string>
#include <memory>
#include "json.h"

class JsonParser
{
  using char_iterator = std::string::const_iterator;
  using ParsingMethodType = void(JsonParser::*)(char_iterator&, char_iterator&, Json*);

public:
  JsonParser();
  std::unique_ptr<Json> Parse(const std::string& data);

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

  /* Parsing methods */
  void ParseString(char_iterator& ch, char_iterator& end, Json* current);
  void ParseArray(char_iterator& ch, char_iterator& end, Json* current);
  void ParseObject(char_iterator& ch, char_iterator& end, Json* current);
  void ParseValue(char_iterator& ch, char_iterator& end, Json* current);
  void ParseNumber(char_iterator& ch, char_iterator& end, Json* current);
  void ParseEscapeChar(char_iterator& ch, char_iterator& end, std::string& str);
  ParsingMethodType GetParsingMethod(ParsingState state);
  bool ExpectKeyword(char_iterator& ch, char_iterator& end, std::string expected_value);

  /* Maniputaion methods */
  void SetParsedValue(const std::string& value, Json* current);
  Json* AddNewPair(Json* current);

  ParsingState parsing_state_;
};

#endif // !JSON_PARSER_H
