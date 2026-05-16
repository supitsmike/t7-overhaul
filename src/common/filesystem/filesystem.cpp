#include "filesystem.hpp"

#include <Windows.h>

namespace filesystem
{
	unsigned long get_file_size(const char* file_name)
	{
		const HANDLE h_file = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h_file == INVALID_HANDLE_VALUE) return INVALID_FILE_SIZE;

		const unsigned long file_size = GetFileSize(h_file, nullptr);

		CloseHandle(h_file);
		return file_size;
	}

	bool read_file(const char* file_name, void* buffer, unsigned long bytes_to_read)
	{
		const HANDLE h_file = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h_file == INVALID_HANDLE_VALUE) return false;

		if (bytes_to_read == 0) bytes_to_read = GetFileSize(h_file, nullptr);

		unsigned long bytes_read = 0;
		const bool result = ReadFile(h_file, buffer, bytes_to_read, &bytes_read, nullptr) == TRUE && bytes_read == bytes_to_read;

		CloseHandle(h_file);
		return result;
	}

	bool write_file(const char* file_name, const void* buffer, const unsigned long bytes_to_write)
	{
		const HANDLE h_file = CreateFileA(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h_file == INVALID_HANDLE_VALUE) return false;

		unsigned long bytes_written = 0;
		const bool result = WriteFile(h_file, buffer, bytes_to_write, &bytes_written, nullptr) == TRUE && bytes_written == bytes_to_write;

		CloseHandle(h_file);
		return result;
	}
}
