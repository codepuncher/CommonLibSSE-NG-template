#include <catch2/catch_test_macros.hpp>

#include "Utils.h"

TEST_CASE("FormatVersion produces correct string", "[utils]")
{
	CHECK(ExampleMod::FormatVersion(1, 2, 3) == "1.2.3");
	CHECK(ExampleMod::FormatVersion(0, 0, 1) == "0.0.1");
	CHECK(ExampleMod::FormatVersion(10, 0, 0) == "10.0.0");
}
