#include "Parser/Tokenizer.hpp"

#include "Parser/Token.hpp"
#include "Parser/TokenizerExceptions.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstddef>
#include <map>
#include <ostream>
#include <queue>
#include <string>
#include <type_traits>

using ::testing::IsEmpty;
using ::testing::PrintToString;

template <IsToken TToken>
std::ostream& operator<<(std::ostream& o_stream,
                         const std::queue<TToken>& i_token_queue)
{
    auto queue_copy = i_token_queue;

    if (queue_copy.empty()) o_stream << "{}";

    o_stream << "{ ";

    while (queue_copy.size() > 1) {
        o_stream << "{ " << static_cast<std::size_t>(queue_copy.front().type)
                 << ", \"" << queue_copy.front().value << "\" }, ";

        queue_copy.pop();
    }

    o_stream << "{ " << static_cast<std::size_t>(queue_copy.front().type)
             << ", \"" << queue_copy.front().value << "\" }";

    o_stream << " }";

    return o_stream;
}

MATCHER_P(QueueEq, i_queue, PrintToString(i_queue))
{
    if (arg.size() != i_queue.size()) return false;

    auto expected_queue = arg;
    auto actual_queue   = i_queue;

    while (!expected_queue.empty()) {
        if (expected_queue.front() != actual_queue.front()) return false;

        expected_queue.pop();
        actual_queue.pop();
    }

    return true;
}

namespace
{
    enum class TokenType0
    {
    };

    enum class TokenType1
    {
        NON_LINE_BREAKER,
        DIGIT,
        NON_DIGIT,
    };

    enum class TokenType2
    {
        SHORT_TOKEN,
        LONG_TOKEN,
    };

    enum class TokenType3
    {
        FIRST_WORD,
        WORD,
        INTEGER,
        REAL,
        PUNCTUATION,
        WHITESPACE,
    };

    enum class TokenType4
    {
        DIGIT,
        ERROR,
    };
} // namespace

TEST(TokenizerTest, Tokenize_EmptyTokenMap_EmptyString_EmptyQueue)
{
    const auto tokenizer = Tokenizer<Token<TokenType0>>();

    const auto string = std::string("");

    const auto tokens = tokenizer.Tokenize(string);

    ASSERT_THAT(tokens, IsEmpty());
}

TEST(TokenizerTest,
     Tokenize_EmptyTokenMap_AnyString_ThrowsUnknownTokenTypeException)
{
    const auto tokenizer = Tokenizer<Token<TokenType1>>();

    const auto string1 = std::string("a");
    const auto string2 = std::string("1");
    const auto string3 = std::string(".");
    const auto string4 = std::string("number 9");
    const auto string5 = std::string("two words");
    const auto string6 = std::string("word, but word");

    ASSERT_THROW(const auto tokens1 = tokenizer.Tokenize(string1),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens2 = tokenizer.Tokenize(string2),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens3 = tokenizer.Tokenize(string3),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens4 = tokenizer.Tokenize(string4),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens5 = tokenizer.Tokenize(string5),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens6 = tokenizer.Tokenize(string6),
                 UnknownTokenTypeException);
}

TEST(
    TokenizerTest,
    Tokenize_SingleStringEntryTokenMap_StreamWithTokenInTheBeggining_TokenQueue)
{
    const auto type  = TokenType1::NON_LINE_BREAKER;
    const auto regex = std::string(".");

    const auto token_map = std::map<TokenType1, std::string>({
        { type, regex },
    });
    const auto tokenizer = Tokenizer(token_map);

    const auto string1 = std::string("\t");
    const auto string2 = std::string("a");
    const auto string3 = std::string("1");
    const auto string4 = std::string(".");

    const auto actual_tokens1   = tokenizer.Tokenize(string1);
    const auto expected_tokens1 = std::queue<Token<TokenType1>>({
        { type, "\t" },
    });
    ASSERT_THAT(actual_tokens1, QueueEq(expected_tokens1));

    const auto actual_tokens2   = tokenizer.Tokenize(string2);
    const auto expected_tokens2 = std::queue<Token<TokenType1>>({
        { type, "a" },
    });
    ASSERT_THAT(actual_tokens2, QueueEq(expected_tokens2));

    const auto actual_tokens3   = tokenizer.Tokenize(string3);
    const auto expected_tokens3 = std::queue<Token<TokenType1>>({
        { type, "1" },
    });
    ASSERT_THAT(actual_tokens3, QueueEq(expected_tokens3));

    const auto actual_tokens4   = tokenizer.Tokenize(string4);
    const auto expected_tokens4 = std::queue<Token<TokenType1>>({
        { type, "." },
    });
    ASSERT_THAT(actual_tokens4, QueueEq(expected_tokens4));
}

TEST(
    TokenizerTest,
    Tokenize_SingleEntryTokenMap_StreamWithTokenNotInTheBeggining_ThrowsUnknownTokenTypeException)
{
    const auto type  = TokenType1::NON_LINE_BREAKER;
    const auto regex = std::string(".");

    const auto token_map = std::map<TokenType1, std::string>({
        { type, regex },
    });
    const auto tokenizer = Tokenizer(token_map);

    const auto string1 = std::string("\n\t");
    const auto string2 = std::string("\na");
    const auto string3 = std::string("\n1");
    const auto string4 = std::string("\n.");
    const auto string5 = std::string("\rn");
    const auto string6 = std::string("\rt");
    const auto string7 = std::string("\rw");

    ASSERT_THROW(const auto tokens1 = tokenizer.Tokenize(string1),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens2 = tokenizer.Tokenize(string2),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens3 = tokenizer.Tokenize(string3),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens4 = tokenizer.Tokenize(string4),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens5 = tokenizer.Tokenize(string5),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens6 = tokenizer.Tokenize(string6),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens7 = tokenizer.Tokenize(string7),
                 UnknownTokenTypeException);
}

TEST(TokenizerTest,
     Tokenize_TwoEntriesTokenMap_StreamWithTokensInTheBeggining_TokenQueue)
{
    const auto type1  = TokenType1::NON_DIGIT;
    const auto regex1 = std::string("\\D");
    const auto type2  = TokenType1::DIGIT;
    const auto regex2 = std::string("\\d");

    const auto token_map = std::map<TokenType1, std::string>({
        { type1, regex1 },
        { type2, regex2 },
    });
    const auto tokenizer = Tokenizer(token_map);

    const auto string1 = std::string("\t");
    const auto string2 = std::string("1");
    const auto string3 = std::string("9a");
    const auto string4 = std::string(".3");
    const auto string5 = std::string("a0n");
    const auto string6 = std::string("5?6");
    const auto string7 = std::string("  4");

    const auto actual_tokens1   = tokenizer.Tokenize(string1);
    const auto expected_tokens1 = std::queue<Token<TokenType1>>({
        { type1, "\t" },
    });
    ASSERT_THAT(actual_tokens1, QueueEq(expected_tokens1));

    const auto actual_tokens2   = tokenizer.Tokenize(string2);
    const auto expected_tokens2 = std::queue<Token<TokenType1>>({
        { type2, "1" },
    });
    ASSERT_THAT(actual_tokens2, QueueEq(expected_tokens2));

    const auto actual_tokens3   = tokenizer.Tokenize(string3);
    const auto expected_tokens3 = std::queue<Token<TokenType1>>({
        { type2, "9" },
        { type1, "a" },
    });
    ASSERT_THAT(actual_tokens3, QueueEq(expected_tokens3));

    const auto actual_tokens4   = tokenizer.Tokenize(string4);
    const auto expected_tokens4 = std::queue<Token<TokenType1>>({
        { type1, "." },
        { type2, "3" },
    });
    ASSERT_THAT(actual_tokens4, QueueEq(expected_tokens4));

    const auto actual_tokens5   = tokenizer.Tokenize(string5);
    const auto expected_tokens5 = std::queue<Token<TokenType1>>({
        { type1, "a" },
        { type2, "0" },
        { type1, "n" },
    });
    ASSERT_THAT(actual_tokens5, QueueEq(expected_tokens5));

    const auto actual_tokens6   = tokenizer.Tokenize(string6);
    const auto expected_tokens6 = std::queue<Token<TokenType1>>({
        { type2, "5" },
        { type1, "?" },
        { type2, "6" },
    });
    ASSERT_THAT(actual_tokens6, QueueEq(expected_tokens6));

    const auto actual_tokens7   = tokenizer.Tokenize(string7);
    const auto expected_tokens7 = std::queue<Token<TokenType1>>({
        { type1, " " },
        { type1, " " },
        { type2, "4" },
    });
    ASSERT_THAT(actual_tokens7, QueueEq(expected_tokens7));
}

TEST(
    TokenizerTest,
    Tokenize_TwoAmbiguousEntriesTokenMap_StreamWithTokenInTheBeggining_ThrowaAmbiguousTokenException)
{
    const auto type1  = TokenType1::NON_LINE_BREAKER;
    const auto regex1 = std::string(".");
    const auto type2  = TokenType1::DIGIT;
    const auto regex2 = std::string("\\d");

    const auto token_map = std::map<TokenType1, std::string>({
        { type1, regex1 },
        { type2, regex2 },
    });
    const auto tokenizer = Tokenizer(token_map);

    const auto string = std::string("1");

    ASSERT_THROW(const auto tokens = tokenizer.Tokenize(string),
                 AmbiguousTokenException);
}

TEST(
    TokenizerTest,
    Tokenize_TwoSimilarEntriesTokenMap_StreamWithTokenInTheBeggining_LongestTokenTokenQueue)
{
    const auto type1  = TokenType2::SHORT_TOKEN;
    const auto regex1 = std::string("a");
    const auto type2  = TokenType2::LONG_TOKEN;
    const auto regex2 = std::string("aaaaaaaaaa");

    const auto token_map = std::map<TokenType2, std::string>({
        { type1, regex1 },
        { type2, regex2 },
    });
    const auto tokenizer = Tokenizer(token_map);

    const auto string = std::string("aaaaaaaaaa");

    const auto actual_tokens   = tokenizer.Tokenize(string);
    const auto expected_tokens = std::queue<Token<TokenType2>>({
        { type2, "aaaaaaaaaa" },
    });
    ASSERT_THAT(actual_tokens, QueueEq(expected_tokens));
}

TEST(TokenizerTest,
     Tokenize_LanguageTokensTokenMap_StreamWithLoremIpsum_TokenQueue)
{
    const auto first_word_type   = TokenType3::FIRST_WORD;
    const auto first_word_regex  = std::string("[A-Z][a-z]*");
    const auto word_type         = TokenType3::WORD;
    const auto word_regex        = std::string("[a-z][a-z]*");
    const auto integer_type      = TokenType3::INTEGER;
    const auto integer_regex     = std::string("-?[1-9][0-9]*");
    const auto real_type         = TokenType3::REAL;
    const auto real_regex        = std::string("-?([1-9][0-9]*|0).[0-9]*");
    const auto punctuation_type  = TokenType3::PUNCTUATION;
    const auto punctuation_regex = std::string(
        "(\\.\\.\\.)|[\\.,;:\\/\\\\()\\[\\]\\{\\}<>!@#$%\\^&*\\-=_+`~'\"]");
    const auto whitespace_type  = TokenType3::WHITESPACE;
    const auto whitespace_regex = std::string("\\s");

    const auto token_map = std::map<TokenType3, std::string>({
        { first_word_type, first_word_regex },
        { word_type, word_regex },
        { integer_type, integer_regex },
        { real_type, real_regex },
        { punctuation_type, punctuation_regex },
        { whitespace_type, whitespace_regex },
    });
    const auto tokenizer = Tokenizer(token_map);

    const auto string = std::string("Lorem ipsum dolor sit amet, consectetur "
                                    "adipiscing elit.");

    const auto actual_tokens   = tokenizer.Tokenize(string);
    const auto expected_tokens = std::queue<Token<TokenType3>>({
        { first_word_type, "Lorem" },
        { whitespace_type, " " },
        { word_type, "ipsum" },
        { whitespace_type, " " },
        { word_type, "dolor" },
        { whitespace_type, " " },
        { word_type, "sit" },
        { whitespace_type, " " },
        { word_type, "amet" },
        { punctuation_type, "," },
        { whitespace_type, " " },
        { word_type, "consectetur" },
        { whitespace_type, " " },
        { word_type, "adipiscing" },
        { whitespace_type, " " },
        { word_type, "elit" },
        { punctuation_type, "." },
    });
    ASSERT_THAT(actual_tokens, QueueEq(expected_tokens));
}

TEST(TokenizerTest,
     Tokenize_SingleRegexEntryTokenMap_StreamWithTokenInTheBeggining_TokenQueue)
{
    const auto type  = TokenType1::NON_LINE_BREAKER;
    const auto regex = std::regex(".");

    const auto token_map =
        std::map<TokenType1, std::regex>({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    const auto string1 = std::string("\t");
    const auto string2 = std::string("a");
    const auto string3 = std::string("1");
    const auto string4 = std::string(".");

    const auto actual_tokens1   = tokenizer.Tokenize(string1);
    const auto expected_tokens1 = std::queue<Token<TokenType1>>({
        { type, "\t" },
    });
    ASSERT_THAT(actual_tokens1, QueueEq(expected_tokens1));

    const auto actual_tokens2   = tokenizer.Tokenize(string2);
    const auto expected_tokens2 = std::queue<Token<TokenType1>>({
        { type, "a" },
    });
    ASSERT_THAT(actual_tokens2, QueueEq(expected_tokens2));

    const auto actual_tokens3   = tokenizer.Tokenize(string3);
    const auto expected_tokens3 = std::queue<Token<TokenType1>>({
        { type, "1" },
    });
    ASSERT_THAT(actual_tokens3, QueueEq(expected_tokens3));

    const auto actual_tokens4   = tokenizer.Tokenize(string4);
    const auto expected_tokens4 = std::queue<Token<TokenType1>>({
        { type, "." },
    });
    ASSERT_THAT(actual_tokens4, QueueEq(expected_tokens4));
}

TEST(
    TokenizerTest,
    TokenizeWithErrorTokenType_SingleRegexEntryTokenMap_StreamWithTokenInTheBeggining_TokenQueue)
{
    const auto type       = TokenType4::DIGIT;
    const auto regex      = std::regex("\\d");
    const auto error_type = TokenType4::ERROR;

    const auto token_map =
        std::map<TokenType4, std::regex>({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    const auto string = std::string("4b");

    const auto actual_tokens   = tokenizer.Tokenize(string, error_type);
    const auto expected_tokens = std::queue<Token<TokenType4>>({
        { type, "4" },
        { error_type, "b" },
    });
    ASSERT_THAT(actual_tokens, QueueEq(expected_tokens));
}