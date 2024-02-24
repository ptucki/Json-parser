#include <gtest/gtest.h>
#include "test-suites.h"


int main(int argc, char* argv[])
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}