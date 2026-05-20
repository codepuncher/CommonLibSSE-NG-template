#pragma once

#include <format>
#include <string>

namespace ExampleMod
{

	[[nodiscard]] inline std::string FormatVersion(int major, int minor, int patch)
	{
		return std::format("{}.{}.{}", major, minor, patch);
	}

}  // namespace ExampleMod
