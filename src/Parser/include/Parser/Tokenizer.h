#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <istream>
#include <iterator>
#include <map>
#include <optional>
#include <queue>
#include <string>

#include "Parser/Token.h"

class Tokenizer final
{
public:
    Tokenizer();
    explicit Tokenizer(const std::map<std::string, std::string>& i_token_types);

    std::optional<std::queue<Token>> Tokenize(std::istream& i_stream) const;

private:
    std::optional<Token>
    GetFirstToken(const std::string::const_iterator& i_begin,
                  const std::string::const_iterator& i_end) const;

    std::map<std::string, std::string> m_token_map;
};

#endif