#include "memory.hpp"

#include <Windows.h>

namespace memory
{
	namespace
	{
		constexpr size_t relative_jmp_size = 5;
	}

	uintptr_t get_base()
	{
		static const uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));
		return base;
	}

	void patch(void* address, const void* data, const size_t size)
	{
		if (address == nullptr || data == nullptr) return;

		unsigned long old_protect{};
		VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old_protect);

		memcpy(address, data, size);

		VirtualProtect(address, size, old_protect, &old_protect);
		FlushInstructionCache(GetCurrentProcess(), address, size);
	}

	void patch(const uintptr_t address, const void* data, const size_t size)
	{
		patch(reinterpret_cast<void*>(address), data, size);
	}

	void nop(void* address, const size_t size)
	{
		if (address == nullptr) return;

		unsigned long old_protect{};
		VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &old_protect);

		memset(address, 0x90, size);

		VirtualProtect(address, size, old_protect, &old_protect);
		FlushInstructionCache(GetCurrentProcess(), address, size);
	}

	void nop(const uintptr_t address, const size_t size)
	{
		nop(reinterpret_cast<void*>(address), size);
	}

	void ret(void* address)
	{
		patch(address, "\xC3", 1);
	}

	void ret(const uintptr_t address)
	{
		ret(reinterpret_cast<void*>(address));
	}

	void relative_jmp(void* from, void* to, const size_t nop_count)
	{
		if (from == nullptr || to == nullptr) return;

		const uintptr_t from_address = reinterpret_cast<uintptr_t>(from);
		const uintptr_t to_address = reinterpret_cast<uintptr_t>(to);
		const int64_t delta = static_cast<int64_t>(to_address) - static_cast<int64_t>(from_address + relative_jmp_size);
		if (delta < INT32_MIN || delta > INT32_MAX) return;

		uint8_t bytes[relative_jmp_size] = { 0xE9 };
		const int32_t offset = static_cast<int32_t>(delta);
		memcpy(&bytes[1], &offset, sizeof(offset));

		patch(from, bytes, sizeof(bytes));
		if (nop_count != 0) nop(from_address + relative_jmp_size, nop_count);
	}

	void relative_jmp(const uintptr_t from, const uintptr_t to, const size_t nop_count)
	{
		relative_jmp(reinterpret_cast<void*>(from), reinterpret_cast<void*>(to), nop_count);
	}

	uintptr_t find_signature(const uintptr_t start_address, const size_t search_size, const char* pattern, std::vector<uintptr_t>* addresses)
	{
		const size_t pattern_size = strlen(pattern) + 1;
		char* const pattern_copy = static_cast<char*>(alloca(pattern_size));
		memset(pattern_copy, 0, pattern_size);
		strncpy_s(pattern_copy, pattern_size, pattern, pattern_size);

		unsigned char* const pattern_full = static_cast<unsigned char*>(alloca(pattern_size));
		memset(pattern_full, 0, pattern_size);

		bool* const mask = static_cast<bool*>(alloca(pattern_size));
		memset(mask, 0, pattern_size);

		size_t pattern_length = 0;
		char* next_token = nullptr;
		const char* token = strtok_s(pattern_copy, " ", &next_token);
		while (token != nullptr) {
			if (token[0] != '?') {
				unsigned char pattern_value = 0;
				sscanf_s(token, "%02X", &pattern_value);
				pattern_full[pattern_length] = pattern_value;
				mask[pattern_length] = true;
			}
			else {
				mask[pattern_length] = false;
			}

			pattern_length++;
			if (pattern_length >= pattern_size) break;
			token = strtok_s(nullptr, " ", &next_token);
		}
		if (pattern_length == 0) return 0;
		if (search_size < pattern_length) return 0;

		uintptr_t found_address = 0;
		const uintptr_t last_address = start_address + search_size - pattern_length;
		for (uintptr_t address = start_address; address <= last_address; address++) {
			bool found = true;
			for (size_t i = 0; i < pattern_length; i++) {
				if (*reinterpret_cast<unsigned char*>(address + i) != pattern_full[i] && mask[i]) {
					found = false;
					break;
				}
			}
			if (!found) continue;

			found_address = address;
			if (addresses != nullptr) {
				addresses->push_back(found_address);
			}
			else break;
		}

		return found_address;
	}

	uintptr_t find_signature(const char* module_name, const char* section_name, const char* pattern, std::vector<uintptr_t>* addresses)
	{
		const HMODULE h_module = GetModuleHandleA(module_name);
		const uintptr_t module_base = reinterpret_cast<uintptr_t>(h_module);
		if (module_base == 0) return 0;

		uintptr_t found_address = 0;
		const PIMAGE_DOS_HEADER dos_headers = reinterpret_cast<PIMAGE_DOS_HEADER>(module_base);
		const PIMAGE_NT_HEADERS nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + dos_headers->e_lfanew);
		PIMAGE_SECTION_HEADER section_header = IMAGE_FIRST_SECTION(nt_headers);
		for (size_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i) {
			if (memcmp(section_header->Name, section_name, IMAGE_SIZEOF_SHORT_NAME) == 0) {
				const uintptr_t section_start = module_base + section_header->VirtualAddress;
				const size_t section_size = section_header->Misc.VirtualSize;

				found_address = find_signature(section_start, section_size, pattern, addresses);
			}
			section_header++;
		}

		return found_address;
	}
}
