#pragma once

#include <Windows.h>
#include "sdk.hpp"

#define BUFF_HASH(str) (std::integral_constant<std::uint32_t, hash_fnv1a_ignorecase(str)>::value)
#define BUFF_HASH_REAL(str) hash_fnv1a_ignorecase(str)
#define SPELL_HASH(str) (std::integral_constant<std::uint32_t, hash_elf_ignorecase(str)>::value)
#define SPELL_HASH_REAL(str) hash_elf_ignorecase(str)
#define CHARACTER_HASH(str) (std::integral_constant<std::uint32_t, hash_sdbm_ignorecase(str)>::value)
#define CHARACTER_HASH_REAL(str) hash_sdbm_ignorecase(str)
#define NAME_HASH(str) (std::integral_constant<std::uint32_t, hash_sdbm_ignorecase(str)>::value)
#define NAME_HASH_REAL(str) hash_sdbm_ignorecase(str)
#define IMAGE_HASH(str) (std::integral_constant<std::uint32_t, hash_sdbm_ignorecase(str)>::value)
#define IMAGE_HASH_REAL(str) hash_sdbm_ignorecase(str)


constexpr uint8_t char_to_lower(const uint8_t input)
{
	if (static_cast<std::uint8_t>(input - 0x41) > 0x19u)
	{
		return input;
	}

	return input + 0x20;
}

constexpr std::uint32_t hash_elf_ignorecase(const char* str)
{
	std::uint32_t hash = 0;

	for (auto i = 0u; str[i]; ++i)
	{
		hash = char_to_lower(str[i]) + 0x10 * hash;

		if (hash & 0xF0000000)
		{
			hash ^= (hash & 0xF0000000) ^ ((hash & 0xF0000000) >> 24);
		}
	}

	return hash;
}

constexpr std::uint32_t hash_sdbm_ignorecase(const char* str)
{
	std::uint32_t hash = 0;

	for (auto i = 0u; str[i]; ++i)
	{
		hash = hash * 65599 + char_to_lower(str[i]);
	}

	return hash;
}

constexpr std::uint32_t hash_fnv1a_ignorecase(const char* str)
{
	std::uint32_t hash = 0x811C9DC5;

	for (auto i = 0u; str[i]; ++i)
	{
		hash = 16777619 * (hash ^ char_to_lower(str[i]));
	}

	return hash;
}