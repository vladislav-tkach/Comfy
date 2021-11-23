#ifndef COMFY_TOKENIZER_H
#define COMFY_TOKENIZER_H

#include <concepts>
#include <cstddef>
#include <format>
#include <istream>
#include <iterator>
#include <map>
#include <queue>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>

#include "Parser/Token.h"
#include "Parser/TokenizerExceptions.h"

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

    std::queue<Token<TTokenType>> Tokenize(std::istream& i_stream) const;
    std::queue<Token<TTokenType>> Tokenize(std::istream& i_stream,
                                           TTokenType    i_error_type) const;

private:
    Token<TTokenType>
                      GetFirstToken(const std::string::const_iterator& i_begin,
                                    const std::string::const_iterator& i_end) const;
    Token<TTokenType> GetFirstToken(const std::string::const_iterator& i_begin,
                                    const std::string::const_iterator& i_end,
                                    TTokenType i_error_type) const;

    std::map<std::size_t, std::regex> m_token_map;
};

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

static constexpr std::size_t min_l1_cache_size = 0x2000;

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline std::queue<Token<TTokenType>>
Tokenizer<TTokenType, TRegex>::Tokenize(std::istream& i_stream) const
{
    auto tokens = std::queue<Token<TTokenType>>();
    auto buffer = std::string(min_l1_cache_size, '\0');

    while (!i_stream.eof()) {
        i_stream.read(&buffer[0], min_l1_cache_size);

        auto begin = buffer.begin();
        auto end   = begin;
        std::advance(end, i_stream.gcount());

        while (begin != end) {
            const auto token = GetFirstToken(begin, end);

            tokens.push(token);
            std::advance(begin, token.value.size());
        }
    }

    return tokens;
}

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline std::queue<Token<TTokenType>>
Tokenizer<TTokenType, TRegex>::Tokenize(std::istream& i_stream,
                                        TTokenType    i_error_type) const
{
    auto tokens = std::queue<Token<TTokenType>>();
    auto buffer = std::string(min_l1_cache_size, ' ');

    while (!i_stream.eof()) {
        i_stream.read(&buffer[0], min_l1_cache_size);

        auto begin = buffer.begin();
        auto end   = begin;
        std::advance(end, i_stream.gcount());

        while (begin != end) {
            const auto token = GetFirstToken(begin, end, i_error_type);

            tokens.push(token.value());
            std::advance(begin, token->value.size());
        }
    }

    return tokens;
}

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline Token<TTokenType> Tokenizer<TTokenType, TRegex>::GetFirstToken(
    const std::string::const_iterator& i_begin,
    const std::string::const_iterator& i_end) const
{
    auto token = Token<TTokenType>();

    for (const auto& [type, regex] : m_token_map) {
        auto matches = std::smatch();

        std::regex_search(i_begin, i_end, matches, regex,
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
                "the \'{}\' for the series of characters: \'{}\'",
                static_cast<std::size_t>(token.type),
                static_cast<std::size_t>(type), typeid(TTokenType).name(),
                std::string(matches[0].first, matches[0].second)));
    }

    if (token.value != "")
        return token;
    else
        throw UnknownTokenTypeException(std::format(
            "Unknown token type for the series of characters: \'{}\'",
            std::string(i_begin, i_end)));
}

template<typename TTokenType, typename TRegex>
requires IsEnum<TTokenType> && IsStringOrRegex<TRegex>
inline Token<TTokenType> Tokenizer<TTokenType, TRegex>::GetFirstToken(
    const std::string::const_iterator& i_begin,
    const std::string::const_iterator& i_end, TTokenType i_error_type) const
{
    auto token               = Token<TTokenType>();
    auto nearest_token_begin = i_end;
    auto nearest_token_end   = i_end;

    for (const auto& [type, regex] : m_token_map) {
        auto matches = std::smatch();

        std::regex_search(i_begin, i_end, matches, regex,
                          std::regex_constants::match_not_null);

        if (matches.size() == 0) continue;
        if (matches[0].first != i_begin) {
            if (matches[0].first < nearest_token_begin) {
                nearest_token_begin = matches[0].first;
                nearest_token_end   = matches[0].second;
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
                "the \'{}\' for the series of characters: \'{}\'",
                static_cast<std::size_t>(token.type),
                static_cast<std::size_t>(type), typeid(TTokenType).name(),
                std::string(matches[0].first, matches[0].second)));
    }

    if (token.value != "")
        return token;
    else
        return { i_error_type,
                 std::string(nearest_token_begin, nearest_token_end) };
}

#endif