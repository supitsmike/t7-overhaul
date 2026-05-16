#include "utils.hpp"

#include <stdio.h>
#include <Windows.h>

namespace utils
{
	namespace
	{
		constexpr size_t va_buffer_count = 4;
		constexpr size_t va_buffer_size = 0x400;

		constexpr size_t max_stack_frames = 64;
	}

	void debug_print(const char* fmt, ...)
	{
        char buffer[va_buffer_size + 5] = {'[', 't', '7', ']', ' ', '\0'};

		va_list args;
		va_start(args, fmt);
		_vsnprintf_s(&buffer[5], va_buffer_size, _TRUNCATE, fmt, args);
		va_end(args);

		OutputDebugStringA(buffer);
	}

	[[noreturn]] void fatal_error(const char* fmt, ...)
	{
		char buffer[va_buffer_size]{};

		va_list args;
		va_start(args, fmt);
		_vsnprintf_s(buffer, va_buffer_size, _TRUNCATE, fmt, args);
		va_end(args);

		debug_print("Fatal: %s", buffer);
		MessageBoxA(nullptr, buffer, "t7-overhaul", MB_OK | MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 1);
	}

	const char* va(const char* fmt, ...)
	{
		thread_local char buffers[va_buffer_count][va_buffer_size]{};
		thread_local size_t index = 0;

		char* buffer = buffers[index];
		index = (index + 1) % va_buffer_count;

		va_list ap;
		va_start(ap, fmt);
		_vsnprintf_s(buffer, va_buffer_size, _TRUNCATE, fmt, ap);
		va_end(ap);

		return buffer;
	}

	void* get_return_address(const unsigned short index)
	{
		if (index >= max_stack_frames) return nullptr;
		void* stack[max_stack_frames]{};

		const unsigned short captured = RtlCaptureStackBackTrace(1, index + 1, stack, nullptr);
		if (index >= captured) return nullptr;

		return stack[index];
	}
}
