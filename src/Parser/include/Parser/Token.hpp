#ifndef COMFY_TOKEN_H
#define COMFY_TOKEN_H

#include <concepts>
#include <string>
#include <type_traits>

template<typename T>
concept IsEnum = std::is_enum_v<T>;

template<typename TEnum>
requires IsEnum<TEnum>
struct Token final
{
    TEnum type;
    std::string value;
};

#endif