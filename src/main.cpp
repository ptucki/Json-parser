#include <iostream>
#include "file.h"
#include "json.h"

int main()
{
  File my_file("testFiles/temp.txt");

  if (my_file.Load())
  {
    auto ptr = Json::Parse(my_file.GetContent(), [](auto progress) {
      std::cout << progress << std::endl;
      });

  }

  return 0;
}
