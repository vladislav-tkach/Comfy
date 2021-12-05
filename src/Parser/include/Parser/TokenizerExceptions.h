#ifndef COMFY_PARSER_TOKENIZER_EXCEPTIONS_H
#define COMFY_PARSER_TOKENIZER_EXCEPTIONS_H

#include <stdexcept>

class AmbiguousTokenException final : public std::runtime_error
{
public:
    AmbiguousTokenException(const std::string& i_string);

    virtual ~AmbiguousTokenException();

    virtual const char* what() const noexcept;
};

class UnknownTokenTypeException final : public std::runtime_error
{
public:
    UnknownTokenTypeException(const std::string& i_string);

    virtual ~UnknownTokenTypeException();

    virtual const char* what() const noexcept;
};

#endif