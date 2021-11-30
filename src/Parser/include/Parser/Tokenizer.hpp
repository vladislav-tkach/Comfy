#ifndef COMFY_TOKENIZER_H
#define COMFY_TOKENIZER_H

#include "Parser/Token.hpp"
#include "Parser/TokenizerExceptions.h"

#include <cstddef>
#include <format>
#include <istream>
#include <iterator>
#include <map>
#include <queue>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>

template <typename TTokenType>
requires IsEnum<TTokenType>
class Tokenizer final
{
public:
    Tokenizer() = default;

    explicit Tokenizer(const std::map<TTokenType, std::regex>& i_token_map);
    explicit Tokenizer(const std::map<TTokenType, std::string>& i_token_map);

    [[nodiscard]] std::queue<Token<TTokenType>> Tokenize(
        std::string_view i_string_view) const;
    [[nodiscard]] std::queue<Token<TTokenType>> Tokenize(
        std::string_view i_string_view, TTokenType i_error_type) const;

private:
    [[nodiscard]] Token<TTokenType> GetFirstToken(
        std::string_view i_string_view) const;
    [[nodiscard]] Token<TTokenType> GetFirstToken(
        std::string_view i_string_view, TTokenType i_error_type) const;

    std::map<std::size_t, std::regex> m_token_map;
};

template <typename TTokenType>
requires IsEnum<TTokenType>
inline Tokenizer<TTokenType>::Tokenizer(
    const std::map<TTokenType, std::regex>& i_token_map)
{
    for (const auto& [type, regex_string] : i_token_map)
        m_token_map[static_cast<std::size_t>(type)] = regex_string;
}

template <typename TTokenType>
requires IsEnum<TTokenType>
inline Tokenizer<TTokenType>::Tokenizer(
    const std::map<TTokenType, std::string>& i_token_map)
{
    for (const auto& [type, regex_string] : i_token_map)
        m_token_map[static_cast<std::size_t>(type)] = std::regex(regex_string);
}

template <typename TTokenType>
requires IsEnum<TTokenType>
[[nodiscard]] inline std::queue<Token<TTokenType>> Tokenizer<
    TTokenType>::Tokenize(std::string_view i_string_view) const
{
    auto tokens = std::queue<Token<TTokenType>>();

    while (!i_string_view.empty()) {
        const auto token = GetFirstToken(i_string_view);

        tokens.push(token);
        i_string_view.remove_prefix(token.value.size());
    }

    return tokens;
}

template <typename TTokenType>
requires IsEnum<TTokenType>
[[nodiscard]] inline std::queue<Token<TTokenType>> Tokenizer<
    TTokenType>::Tokenize(std::string_view i_string_view,
                          TTokenType i_error_type) const
{
    auto tokens = std::queue<Token<TTokenType>>();

    while (!i_string_view.empty()) {
        const auto token = GetFirstToken(i_string_view, i_error_type);

        tokens.push(token);
        i_string_view.remove_prefix(token.value.size());
    }

    return tokens;
}

template <typename TTokenType>
requires IsEnum<TTokenType>
[[nodiscard]] inline Token<TTokenType> Tokenizer<TTokenType>::GetFirstToken(
    std::string_view i_string_view) const
{
    auto token = Token<TTokenType>();

    for (const auto& [type, regex] : m_token_map) {
        auto matches = std::match_results<std::string_view::const_iterator>();

        std::regex_search(i_string_view.begin(), i_string_view.end(), matches,
                          regex,
                          std::regex_constants::match_continuous |
                              std::regex_constants::match_not_null);

        if (matches.size() == 0) continue;

        const auto token_size = matches[0].second - matches[0].first;

        if (token_size > token.value.size())
            token = { static_cast<TTokenType>(type),
                      std::string(matches[0].first, matches[0].second) };
        else if (token_size == token.value.size())
            throw AmbiguousTokenException(std::format(
                "Ambiguous token types' ID's: \'{}\' and \'{}\' of "
                "the \'{}\' for the series of characters: \'{}\'.",
                static_cast<std::size_t>(token.type),
                static_cast<std::size_t>(type), typeid(TTokenType).name(),
                std::string(matches[0].first, matches[0].second)));
    }

    if (token.value != "")
        return token;
    else
        throw UnknownTokenTypeException(std::format(
            "Unknown token type for the series of characters: \'{}\'.",
            std::string(i_string_view.begin(), i_string_view.end())));
}

template <typename TTokenType>
requires IsEnum<TTokenType>
[[nodiscard]] inline Token<TTokenType> Tokenizer<TTokenType>::GetFirstToken(
    std::string_view i_string_view, TTokenType i_error_type) const
{
    auto token               = Token<TTokenType>();
    auto nearest_token_begin = i_string_view.end();

    for (const auto& [type, regex] : m_token_map) {
        auto matches = std::match_results<std::string_view::const_iterator>();

        std::regex_search(i_string_view.begin(), i_string_view.end(), matches,
                          regex, std::regex_constants::match_not_null);

        if (matches.size() == 0) continue;
        if (matches[0].first != i_string_view.begin()) {
            if (matches[0].first < nearest_token_begin) {
                nearest_token_begin = matches[0].first;
            }
            continue;
        }

        const auto token_size = matches[0].second - matches[0].first;

        if (token_size > token.value.size())
            token = { static_cast<TTokenType>(type),
                      std::string(matches[0].first, matches[0].second) };
        else if (token_size == token.value.size())
            throw AmbiguousTokenException(std::format(
                "Ambiguous token types' ID's: \'{}\' and \'{}\' of "
                "the \'{}\' for the series of characters: \'{}\'.",
                static_cast<std::size_t>(token.type),
                static_cast<std::size_t>(type), typeid(TTokenType).name(),
                std::string(matches[0].first, matches[0].second)));
    }

    if (token.value != "")
        return token;
    else
        return { i_error_type,
                 std::string(i_string_view.begin(), nearest_token_begin) };
}

#endif