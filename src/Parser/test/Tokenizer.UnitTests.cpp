#include "Parser/Tokenizer.hpp"

#include "Parser/Token.hpp"
#include "Parser/TokenizerExceptions.h"

#include "gtest/gtest.h"

#include <map>
#include <sstream>
#include <string>

namespace
{
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

TEST(TokenizerTest, Tokenize_EmptyTokenMap_EmptyStream_EmptyQueue)
{
    const auto tokenizer = Tokenizer<TokenType1>();

    auto stream = std::stringstream("");

    const auto tokens = tokenizer.Tokenize(stream);

    ASSERT_TRUE(tokens.empty());
}

TEST(TokenizerTest,
     Tokenize_EmptyTokenMap_AnyStream_ThrowsUnknownTokenTypeException)
{
    const auto tokenizer = Tokenizer<TokenType1>();

    auto stream1 = std::stringstream("a");
    auto stream2 = std::stringstream("1");
    auto stream3 = std::stringstream(".");
    auto stream4 = std::stringstream("number 9");
    auto stream5 = std::stringstream("two words");
    auto stream6 = std::stringstream("word, but word");

    ASSERT_THROW(const auto tokens1 = tokenizer.Tokenize(stream1),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens2 = tokenizer.Tokenize(stream2),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens3 = tokenizer.Tokenize(stream3),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens4 = tokenizer.Tokenize(stream4),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens5 = tokenizer.Tokenize(stream5),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens6 = tokenizer.Tokenize(stream6),
                 UnknownTokenTypeException);
}

TEST(
    TokenizerTest,
    Tokenize_SingleStringEntryTokenMap_StreamWithTokenInTheBeggining_TokenQueue)
{
    const auto type  = TokenType1::NON_LINE_BREAKER;
    const auto regex = std::string(".");

    const auto token_map =
        std::map<TokenType1, std::string>({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream1 = std::stringstream("\t");
    auto stream2 = std::stringstream("a");
    auto stream3 = std::stringstream("1");
    auto stream4 = std::stringstream(".");
    auto stream5 = std::stringstream("n");
    auto stream6 = std::stringstream("t");
    auto stream7 = std::stringstream("w");

    auto tokens = tokenizer.Tokenize(stream1);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("\t", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream2);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("a", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream3);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("1", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream4);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ(".", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream5);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("n", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream6);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("t", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream7);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("w", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());
}

TEST(
    TokenizerTest,
    Tokenize_SingleEntryTokenMap_StreamWithTokenNotInTheBeggining_ThrowsUnknownTokenTypeException)
{
    const auto type  = TokenType1::NON_LINE_BREAKER;
    const auto regex = std::string(".");

    const auto token_map =
        std::map<TokenType1, std::string>({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream1 = std::stringstream("\n\t");
    auto stream2 = std::stringstream("\na");
    auto stream3 = std::stringstream("\n1");
    auto stream4 = std::stringstream("\n.");
    auto stream5 = std::stringstream("\rn");
    auto stream6 = std::stringstream("\rt");
    auto stream7 = std::stringstream("\rw");

    ASSERT_THROW(const auto tokens1 = tokenizer.Tokenize(stream1),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens2 = tokenizer.Tokenize(stream2),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens3 = tokenizer.Tokenize(stream3),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens4 = tokenizer.Tokenize(stream4),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens5 = tokenizer.Tokenize(stream5),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens6 = tokenizer.Tokenize(stream6),
                 UnknownTokenTypeException);

    ASSERT_THROW(const auto tokens7 = tokenizer.Tokenize(stream7),
                 UnknownTokenTypeException);
}

TEST(TokenizerTest,
     Tokenize_TwoEntriesTokenMap_StreamWithTokensInTheBeggining_TokenQueue)
{
    const auto type1  = TokenType1::NON_DIGIT;
    const auto regex1 = std::string("\\D");
    const auto type2  = TokenType1::DIGIT;
    const auto regex2 = std::string("\\d");

    const auto token_map = std::map<TokenType1, std::string>(
        { { type1, regex1 }, { type2, regex2 } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream1 = std::stringstream("\t");
    auto stream2 = std::stringstream("1");
    auto stream3 = std::stringstream("9a");
    auto stream4 = std::stringstream(".3");
    auto stream5 = std::stringstream("a0n");
    auto stream6 = std::stringstream("5?6");
    auto stream7 = std::stringstream("  4");

    auto tokens = tokenizer.Tokenize(stream1);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ("\t", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream2);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("1", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream3);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("9", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ("a", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream4);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ(".", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("3", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream5);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ("a", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("0", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ("n", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream6);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("5", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ("?", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("6", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());

    tokens = tokenizer.Tokenize(stream7);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type1, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("4", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());
}

TEST(
    TokenizerTest,
    Tokenize_TwoAmbiguousEntriesTokenMap_StreamWithTokenInTheBeggining_ThrowaAmbiguousTokenException)
{
    const auto type1  = TokenType1::NON_LINE_BREAKER;
    const auto regex1 = std::string(".");
    const auto type2  = TokenType1::DIGIT;
    const auto regex2 = std::string("\\d");

    const auto token_map = std::map<TokenType1, std::string>(
        { { type1, regex1 }, { type2, regex2 } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream("1");

    ASSERT_THROW(
        auto tokens = tokenizer.Tokenize(stream), AmbiguousTokenException);
}

TEST(
    TokenizerTest,
    Tokenize_TwoSimilarEntriesTokenMap_StreamWithTokenInTheBeggining_LongestTokenTokenQueue)
{
    const auto type1  = TokenType2::SHORT_TOKEN;
    const auto regex1 = std::string("a");
    const auto type2  = TokenType2::LONG_TOKEN;
    const auto regex2 = std::string("aaaaaaaaaa");

    const auto token_map = std::map<TokenType2, std::string>(
        { { type1, regex1 }, { type2, regex2 } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream("aaaaaaaaaa");

    auto tokens = tokenizer.Tokenize(stream);
    ASSERT_FALSE(tokens.empty());
    ASSERT_EQ(1, tokens.size());
    EXPECT_EQ(type2, tokens.front().type);
    EXPECT_EQ("aaaaaaaaaa", tokens.front().value);
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

    const auto token_map = std::map<TokenType3, std::string>(
        { { first_word_type, first_word_regex },
          { word_type, word_regex },
          { integer_type, integer_regex },
          { real_type, real_regex },
          { punctuation_type, punctuation_regex },
          { whitespace_type, whitespace_regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream("Lorem ipsum dolor sit amet, consectetur "
                                    "adipiscing elit.");

    auto tokens = tokenizer.Tokenize(stream);

    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(first_word_type, tokens.front().type);
    EXPECT_EQ("Lorem", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("ipsum", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("dolor", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("sit", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("amet", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(punctuation_type, tokens.front().type);
    EXPECT_EQ(",", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("consectetur", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("adipiscing", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(whitespace_type, tokens.front().type);
    EXPECT_EQ(" ", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(word_type, tokens.front().type);
    EXPECT_EQ("elit", tokens.front().value);
    tokens.pop();

    EXPECT_EQ(punctuation_type, tokens.front().type);
    EXPECT_EQ(".", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());
}

TEST(TokenizerTest,
     Tokenize_SingleRegexEntryTokenMap_StreamWithTokenInTheBeggining_TokenQueue)
{
    const auto type  = TokenType1::NON_LINE_BREAKER;
    const auto regex = std::regex(".");

    const auto token_map =
        std::map<TokenType1, std::regex>({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream("\t");

    auto tokens = tokenizer.Tokenize(stream);
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("\t", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());
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

    auto stream = std::stringstream("4b");

    auto tokens = tokenizer.Tokenize(stream, error_type);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(type, tokens.front().type);
    EXPECT_EQ("4", tokens.front().value);
    tokens.pop();
    EXPECT_EQ(error_type, tokens.front().type);
    EXPECT_EQ("b", tokens.front().value);
    tokens.pop();
    ASSERT_TRUE(tokens.empty());
}