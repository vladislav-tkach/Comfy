#include "Parser/TokenizerExceptions.h"

AmbiguousTokenException::AmbiguousTokenException(const std::string& i_string)
    : std::runtime_error(i_string)
{}

AmbiguousTokenException::~AmbiguousTokenException() = default;

const char* AmbiguousTokenException::what() const noexcept
{
    return std::runtime_error::what();
}

UnknownTokenTypeException::UnknownTokenTypeException(
    const std::string& i_string)
    : std::runtime_error(i_string)
{}

UnknownTokenTypeException::~UnknownTokenTypeException() = default;

const char* UnknownTokenTypeException::what() const noexcept
{
    return std::runtime_error::what();
}