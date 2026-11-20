#pragma once

#ifdef __cplusplus
#include <cstddef>
#include <cstring>

inline void NDIlib_file_copy_event_string(char* p_dst, const size_t dst_size, const char* p_src)
{
	if (p_dst == nullptr || dst_size == 0)
	{
		return;
	}

	if (p_src == nullptr)
	{
		p_dst[0] = '\0';
		return;
	}

	std::strncpy(p_dst, p_src, dst_size - 1);
	p_dst[dst_size - 1] = '\0';
}
#endif // __cplusplus
