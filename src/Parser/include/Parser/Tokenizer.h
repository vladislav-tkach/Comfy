#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <concepts>
#include <cstddef>
#include <istream>
#include <iterator>
#include <map>
#include <optional>
#include <queue>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>

#include "Parser/Token.h"

template<typename T>
concept IsStringOrRegex =
    std::is_same_v<T, std::string> || std::is_same_v<T, std::regex>;

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
class Tokenizer final
{
public:
    Tokenizer() = default;
    explicit Tokenizer(const std::map<TTokenType, TRegex>& i_token_map);

    std::optional<std::queue<Token<TTokenType>>>
    Tokenize(std::istream& i_stream) const;

private:
    std::optional<Token<TTokenType>>
    GetFirstToken(const std::string::const_iterator& i_begin,
                  const std::string::const_iterator& i_end) const;

    std::map<std::size_t, std::regex> m_token_map;
};

static constexpr std::size_t min_l1_cache_size = 0x2000;

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline Tokenizer<TTokenType, TRegex>::Tokenizer(
    const std::map<TTokenType, TRegex>& i_token_map)
{
    if constexpr (std::is_same_v<TRegex, std::string>)
        for (const auto& [type, regex_string] : i_token_map)
            m_token_map[static_cast<std::size_t>(type)] =
                std::regex(regex_string, std::regex::collate);

    else
        for (const auto& [type, regex_string] : i_token_map)
            m_token_map[static_cast<std::size_t>(type)] = regex_string;
}

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline std::optional<std::queue<Token<TTokenType>>>
Tokenizer<TTokenType, TRegex>::Tokenize(std::istream& i_stream) const
{
    auto tokens = std::queue<Token<TTokenType>>();
    auto buffer = std::string(min_l1_cache_size, ' ');

    while (!i_stream.eof()) {
        i_stream.read(&buffer[0], min_l1_cache_size);

        auto begin = buffer.begin();
        auto end   = begin;
        std::advance(end, i_stream.gcount());

        while (begin != end) {
            auto token = GetFirstToken(begin, end);

            if (!token.has_value()) return {};

            tokens.push(token.value());
            std::advance(begin, token->value.size());
        }
    }

    if (tokens.empty())
        return {};
    else
        return { tokens };
}

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline std::optional<Token<TTokenType>>
Tokenizer<TTokenType, TRegex>::GetFirstToken(
    const std::string::const_iterator& i_begin,
    const std::string::const_iterator& i_end) const
{
    auto token = Token<TTokenType>();

    for (const auto& [type, regex] : m_token_map) {
        auto matches = std::smatch();

        std::regex_search(i_begin, i_end, matches, regex,
                          std::regex_constants::match_continuous);

        if (matches.size() == 0) continue;

        if (matches[0].second - matches[0].first > token.value.size())
            token = { static_cast<TTokenType>(type),
                      std::string(matches[0].first, matches[0].second) };
        else if (matches[0].second - matches[0].first == token.value.size())
            return {};
    }

    if (token.value != "")
        return { token };
    else
        return {};
}

#endif