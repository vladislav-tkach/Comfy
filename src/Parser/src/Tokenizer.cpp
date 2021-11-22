#include <cstddef>
#include <iterator>
#include <map>
#include <optional>
#include <regex>

#include "Parser/Tokenizer.h"

namespace
{
    constexpr std::size_t min_l1_cache_size = 0x2000;
}

Tokenizer::Tokenizer() = default;

Tokenizer::Tokenizer(const std::map<std::string, std::string>& i_token_types)
    : m_token_map(i_token_types)
{}

std::optional<std::queue<Token>>
Tokenizer::Tokenize(std::istream& i_stream) const
{
    auto tokens = std::queue<Token>();
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

std::optional<Token>
Tokenizer::GetFirstToken(const std::string::const_iterator& i_begin,
                         const std::string::const_iterator& i_end) const
{
    auto token = Token();

    for (const auto& [type, regex] : m_token_map) {
        auto matches = std::smatch();

        std::regex_search(i_begin, i_end, matches,
                          std::regex(regex, std::regex::collate),
                          std::regex_constants::match_continuous);

        if (matches.size() == 0) continue;

        if (matches[0].second - matches[0].first > token.value.size())
            token = { type, std::string(matches[0].first, matches[0].second) };
        else if (matches[0].second - matches[0].first == token.value.size())
            return {};
    }

    if (token.value != "")
        return { token };
    else
        return {};
}