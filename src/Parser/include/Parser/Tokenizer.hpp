#ifndef COMFY_PARSER_TOKENIZER_H
#define COMFY_PARSER_TOKENIZER_H

#include "Parser/Token.hpp"
#include "Parser/TokenizerExceptions.h"

#include <cstddef>
#include <format>
#include <iterator>
#include <map>
#include <queue>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>

template <IsToken TToken>
class Tokenizer final
{
public:
    Tokenizer() = default;

    Tokenizer(const Tokenizer& i_other) = delete;
    Tokenizer(Tokenizer&& i_other)      = delete;
    Tokenizer& operator=(const Tokenizer& i_other) = delete;
    Tokenizer& operator=(Tokenizer&& i_other) = delete;

    explicit Tokenizer(
        const std::map<typename TToken::enum_type, std::regex>& i_token_map)
    {
        for (const auto& [type, regex] : i_token_map)
            m_token_map[static_cast<std::size_t>(type)] = regex;
    }

    explicit Tokenizer(
        const std::map<typename TToken::enum_type, std::string>& i_token_map)
    {
        for (const auto& [type, regex_string] : i_token_map)
            m_token_map[static_cast<std::size_t>(type)] = regex_string;
    }

    [[nodiscard]] std::queue<TToken> Tokenize(
        std::string_view i_string_view) const
    {
        auto tokens = std::queue<TToken>();

        while (!i_string_view.empty()) {
            const auto token = GetFirstToken(i_string_view);

            tokens.push(token);
            i_string_view.remove_prefix(token.value.size());
        }

        return tokens;
    }

    [[nodiscard]] std::queue<TToken> Tokenize(
        std::string_view i_string_view,
        typename TToken::enum_type i_error_type) const
    {
        auto tokens = std::queue<TToken>();

        while (!i_string_view.empty()) {
            const auto token = GetFirstToken(i_string_view, i_error_type);

            tokens.push(token);
            i_string_view.remove_prefix(token.value.size());
        }

        return tokens;
    }

private:
    [[nodiscard]] TToken GetFirstToken(std::string_view i_string_view) const
    {
        auto token = TToken();

        for (const auto& [type, regex] : m_token_map) {
            auto matches =
                std::match_results<std::string_view::const_iterator>();

            std::regex_search(i_string_view.begin(), i_string_view.end(),
                              matches, regex,
                              std::regex_constants::match_continuous |
                                  std::regex_constants::match_not_null);

            if (matches.size() == 0) continue;

            const auto token_size = matches[0].second - matches[0].first;

            if (token_size > token.value.size())
                token = { static_cast<typename TToken::enum_type>(type),
                          std::string(matches[0].first, matches[0].second) };
            else if (token_size == token.value.size())
                throw AmbiguousTokenException(std::format(
                    "Ambiguous token types' ID's: \'{}\' and \'{}\' of "
                    "the \'{}\' for the series of characters: \'{}\'.",
                    static_cast<std::size_t>(token.type),
                    static_cast<std::size_t>(type),
                    typeid(typename TToken::enum_type).name(),
                    std::string(matches[0].first, matches[0].second)));
        }

        if (token.value != "")
            return token;
        else
            throw UnknownTokenTypeException(std::format(
                "Unknown token type for the series of characters: \'{}\'.",
                std::string(i_string_view.begin(), i_string_view.end())));
    }

    [[nodiscard]] TToken GetFirstToken(
        std::string_view i_string_view,
        typename TToken::enum_type i_error_type) const
    {
        auto token               = TToken();
        auto nearest_token_begin = i_string_view.end();

        for (const auto& [type, regex] : m_token_map) {
            auto matches =
                std::match_results<std::string_view::const_iterator>();

            std::regex_search(i_string_view.begin(), i_string_view.end(),
                              matches, regex,
                              std::regex_constants::match_not_null);

            if (matches.size() == 0) continue;
            if (matches[0].first != i_string_view.begin()) {
                if (matches[0].first < nearest_token_begin) {
                    nearest_token_begin = matches[0].first;
                }
                continue;
            }

            const auto token_size = matches[0].second - matches[0].first;

            if (token_size > token.value.size())
                token = { static_cast<typename TToken::enum_type>(type),
                          std::string(matches[0].first, matches[0].second) };
            else if (token_size == token.value.size())
                throw AmbiguousTokenException(std::format(
                    "Ambiguous token types' ID's: \'{}\' and \'{}\' of "
                    "the \'{}\' for the series of characters: \'{}\'.",
                    static_cast<std::size_t>(token.type),
                    static_cast<std::size_t>(type),
                    typeid(typename TToken::enum_type).name(),
                    std::string(matches[0].first, matches[0].second)));
        }

        if (token.value != "")
            return token;
        else
            return { i_error_type,
                     std::string(i_string_view.begin(), nearest_token_begin) };
    }

    std::map<std::size_t, std::regex> m_token_map;
};

template <IsEnum TEnum>
Tokenizer(const std::map<TEnum, std::regex>& i_token_map)
    -> Tokenizer<Token<TEnum>>;

template <IsEnum TEnum>
Tokenizer(const std::map<TEnum, std::string>& i_token_map)
    -> Tokenizer<Token<TEnum>>;

#endif