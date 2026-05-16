#include "input.hpp"

#include <Windows.h>

namespace input
{
	namespace
	{
		constexpr size_t key_table_size = 0x100;
		bool keys[key_table_size]{};

		constexpr size_t combo_table_size = 0x2000;
		bool combo_states[combo_table_size]{};
	}

	bool is_key_down(const int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}

	bool is_key_down(const int* key_list, const size_t count)
	{
		for (size_t i = 0; i < count; i++) {
			if (!is_key_down(key_list[i])) return false;
		}
		return true;
	}

	bool is_key_pressed(const int key)
	{
		if (key < 0 || key >= static_cast<int>(key_table_size)) return false;

		if (!is_key_down(key)) {
			keys[key] = false;
			return false;
		}
		if (keys[key]) return false;
		keys[key] = true;
		return true;
	}

	bool is_key_pressed(const int* key_list, const size_t count)
	{
		if (count == 0) return false;

		size_t combo_hash = 0;
		for (size_t i = 0; i < count; i++) {
			const size_t k = static_cast<size_t>(key_list[i]);
			combo_hash += (k * k * k) + (k * 0x400);
		}
		combo_hash %= combo_table_size;

		bool& was_active = combo_states[combo_hash];
		if (!is_key_down(key_list, count)) {
			was_active = false;
			return false;
		}

		if (was_active) return false;
		was_active = true;
		return true;
	}
}
