#pragma once

#include <stdint.h>

class detour
{
	void* target_{};
	void* original_{};

	void** iat_target_{};
	void* iat_detour_{};

public:
	detour() = default;
	~detour();

	detour(const detour&) = delete;
	detour& operator=(const detour&) = delete;
	detour(detour&&) = delete;
	detour& operator=(detour&&) = delete;

	static void initialize();
	static void uninitialize();

	void create(void* target, void* hook);
	void create(uintptr_t address, void* hook);
	void create(const wchar_t* module_name, const char* proc_name, void* hook);

	void remove();
	void enable() const;
	void disable() const;

	void create_iat(const char* module_name, const char* import_module_name, const char* proc_name, void* hook);

	void remove_iat();
	void enable_iat() const;
	void disable_iat() const;

	[[nodiscard]] void* get_target() const;
	void* get_target(const char* module_name, const char* proc_name);
	void* get_target(const char* module_name, const char* import_module_name, const char* proc_name);

	template <typename T = void, typename... Args>
	T invoke(Args... args)
	{
		return reinterpret_cast<T(*)(Args...)>(this->original_)(args...);
	}
};
