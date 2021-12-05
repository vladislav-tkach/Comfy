#ifndef COMFY_PARSER_TOKEN_H
#define COMFY_PARSER_TOKEN_H

#include <string>
#include <type_traits>

namespace Comfy
{
template <typename T>
concept IsEnum = std::is_enum_v<T>;

template <IsEnum TEnum>
struct Token final
{
    using enum_type = TEnum;

    TEnum type;
    std::string value;

    bool operator==(const Token& i_other) const = default;
};

template <typename T>
concept IsToken = std::is_same_v<T, Token<typename T::enum_type>>;
} // namespace Comfy

#endif