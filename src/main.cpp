#include <iostream>
#include "file.h"
#include "json.h"

int main()
{
  File my_file("testFiles/temp.txt");

  //if (my_file.Load())
  //{
  //  std::cout << my_file.GetContent() << std::endl;

  //  auto ptr = Json::Parse(my_file.GetContent());

  //  Json first("first1", nullptr);
  //  first.SetValue(5);
  //  //auto temp = first.AddChild(true, "wow");
  //  if (first.AddChild(1.3l, "da") == nullptr) {
  //    std::cout << "nullptr :<" << std::endl;
  //  }

  //  ptr->AddChild(first);
  //}

  auto root = std::make_unique<Json>();

  root->AddChild(true, "bool");
  root->AddChild(5l, "number");
  auto child = root->AddChild(Json("temp", nullptr));
  child->AddChild(true, "bool");
  child->AddChild(5.0l, "number");
  child->AddValue(12.3l);

  return 0;
}
