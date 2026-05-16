#pragma once

#include <vector>

namespace memory
{
	constexpr uintptr_t preferred_image_base = 0x140000000;

	uintptr_t get_base();

	inline uintptr_t relocate(const uintptr_t val)
	{
		if (!val) return 0;
		return get_base() + (val - preferred_image_base);
	}

	inline uintptr_t derelocate(const uintptr_t val)
	{
		if (!val) return 0;
		return (val - get_base()) + preferred_image_base;
	}

	inline uintptr_t derelocate(const void* val)
	{
		return derelocate(reinterpret_cast<uintptr_t>(val));
	}

	void patch(void* address, const void* data, size_t size);
	void patch(uintptr_t address, const void* data, size_t size);

	void nop(void* address, size_t size);
	void nop(uintptr_t address, size_t size);

	void ret(void* address);
	void ret(uintptr_t address);

	void relative_jmp(void* from, void* to, size_t nop_count = 0);
	void relative_jmp(uintptr_t from, uintptr_t to, size_t nop_count = 0);

	template <typename T, typename... Args>
	T invoke(const uintptr_t address, Args... args)
	{
		return reinterpret_cast<T(*)(Args...)>(address)(args...);
	}
	template <typename T, typename... Args>
	T invoke(const void* address, Args... args)
	{
		return reinterpret_cast<T(*)(Args...)>(address)(args...);
	}

	uintptr_t find_signature(uintptr_t start_address, size_t search_size, const char* pattern, std::vector<uintptr_t>* addresses = nullptr);
	uintptr_t find_signature(const char* module_name, const char* section_name, const char* pattern, std::vector<uintptr_t>* addresses = nullptr);
}

inline uintptr_t operator""_g(const uintptr_t val)
{
	return memory::relocate(static_cast<uintptr_t>(val));
}
