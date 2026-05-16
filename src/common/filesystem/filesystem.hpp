#pragma once

namespace filesystem
{
	unsigned long get_file_size(const char* file_name);

    bool read_file(const char* file_name, void* buffer, unsigned long bytes_to_read = 0);

    bool write_file(const char* file_name, const void* buffer, unsigned long bytes_to_write);
}
