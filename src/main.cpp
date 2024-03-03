#include <iostream>
#include "file.h"
#include "json.h"

int main()
{

  std::variant<std::string, double> v;

  auto json = Json::Parse(R"(   
  {
    "key" : "value",
    "key1" : [[ 13.2, "dasd"], true]
  }   
)");

  if (!json->IsValid())
  {
    std::cout << "Json is invalid" << std::endl;
  }

  auto test = (*json)[0];

  const std::string temp1 = "asdf";

  test->SetValue(temp1);

  std::string_view asd = "asdas";

  auto coto = json->AddChild(14.5, "wow");
  coto->AddValue(true);
  auto item = (*coto)[1]->Detach();

  return 0;
}
