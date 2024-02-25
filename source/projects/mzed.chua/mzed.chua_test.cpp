/// @file
///	@ingroup 	mzed
///	@copyright	Copyright 2002-2021 Michael Zbyszyński  All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min_unittest.h"     // required unit test header
#include "mzed.chua.cpp"    // need the source of our object so that we can access it

SCENARIO("object produces correct output")
{
  ext_main(nullptr);
  
  GIVEN("An instance of chua")
  {
    test_wrapper<mzed_chua> an_instance;
    mzed_chua&              my_object = an_instance;
    
    // check that default attr values are correct
    REQUIRE(my_object.c_a == 14.5);
    REQUIRE(my_object.c_b == 1.0);
    REQUIRE(my_object.c_c == 25.58);
    REQUIRE(my_object.c_d == -1.0);
    REQUIRE(my_object.c_e == 0.0);
    REQUIRE(my_object.c_h == 0.01);
    
    // test bang
    WHEN("a 'bang' is received")
    {
      my_object.bang();
      THEN("z is calculated")
      {
        auto& output = *c74::max::object_getoutput(my_object, 2);
        REQUIRE(output.size() == 1);
        REQUIRE(output[0].size() == 2);
        REQUIRE(output[0][1] == 0.7442);
      }
      THEN("y is calculated")
      {
        auto& output = *c74::max::object_getoutput(my_object, 1);
        REQUIRE(output.size() == 1);
        REQUIRE(output[0].size() == 2);
        REQUIRE(output[0][1] == 1.01);
      }
      THEN("x is calculated")
      {
        auto& output = *c74::max::object_getoutput(my_object, 0);
        REQUIRE(output.size() == 1);
        REQUIRE(output[0].size() == 2);
        REQUIRE(output[0][1] == 1.29);
      }
    }
  }
}
