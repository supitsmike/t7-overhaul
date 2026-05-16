#pragma once

namespace input
{
	bool is_key_down(int key);
	bool is_key_down(const int* key_list, size_t count);

	template <typename... Keys>
	bool is_key_down(Keys... keys)
	{
		const int key_array[] = { static_cast<int>(keys)... };
		return is_key_down(key_array, sizeof...(keys));
	}

	bool is_key_pressed(int key);
	bool is_key_pressed(const int* key_list, size_t count);

	template <typename... Keys>
	bool is_key_pressed(Keys... keys)
	{
		const int key_array[] = { static_cast<int>(keys)... };
		return is_key_pressed(key_array, sizeof...(keys));
	}
}
