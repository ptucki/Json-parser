#include <iostream>
#include "file.h"
#include "json.h"

int main()
{
  File my_file("testFiles/temp.txt");

  if (my_file.Load())
  {
    std::cout << my_file.GetContent() << std::endl;

    auto ptr = Json::Parse(my_file.GetContent());

    Json first("first1", nullptr);
    auto temp = first.AddChild(true, "wow");
    if (first.AddChild(1.3l, "da") == nullptr) {
      std::cout << "nullptr :<" << std::endl;
    }

    ptr->AddChild(first);
  }
}
