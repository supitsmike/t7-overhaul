#pragma once

namespace utils
{
	void debug_print(const char* fmt, ...);

	[[noreturn]] void fatal_error(const char* fmt, ...);

	const char* va(const char* fmt, ...);

	void* get_return_address(unsigned short index = 0);
}
