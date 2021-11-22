#include "gtest/gtest.h"
#include <map>
#include <sstream>
#include <string>

#include "Parser/Tokenizer.h"

using TokenMap = std::map<std::string, std::string>;

TEST(TokenizerTest, Tokenize_EmptyTokenMap_AnyStream_EmptyOptionalTokenQueue)
{
    const auto tokenizer = Tokenizer();

    auto stream1 = std::stringstream("");
    auto stream2 = std::stringstream("a");
    auto stream3 = std::stringstream("1");
    auto stream4 = std::stringstream(".");
    auto stream5 = std::stringstream("number 9");
    auto stream6 = std::stringstream("two words");
    auto stream7 = std::stringstream("word, but word");

    auto tokens = tokenizer.Tokenize(stream1);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream2);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream3);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream4);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream5);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream6);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream7);
    ASSERT_FALSE(tokens.has_value());
}

TEST(TokenizerTest,
     Tokenize_SingleEntryTokenMap_StreamWithTokenInTheBeggining_TokenQueue)
{
    const auto type  = std::string("Any character except line terminators");
    const auto regex = std::string(".");

    const auto token_map = TokenMap({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream1 = std::stringstream("\t");
    auto stream2 = std::stringstream("a");
    auto stream3 = std::stringstream("1");
    auto stream4 = std::stringstream(".");
    auto stream5 = std::stringstream("n");
    auto stream6 = std::stringstream("t");
    auto stream7 = std::stringstream("w");

    auto tokens = tokenizer.Tokenize(stream1);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ("\t", tokens->front().value);

    tokens = tokenizer.Tokenize(stream2);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ("a", tokens->front().value);

    tokens = tokenizer.Tokenize(stream3);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ("1", tokens->front().value);

    tokens = tokenizer.Tokenize(stream4);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ(".", tokens->front().value);

    tokens = tokenizer.Tokenize(stream5);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ("n", tokens->front().value);

    tokens = tokenizer.Tokenize(stream6);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ("t", tokens->front().value);

    tokens = tokenizer.Tokenize(stream7);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type, tokens->front().type);
    EXPECT_EQ("w", tokens->front().value);
}

TEST(
    TokenizerTest,
    Tokenize_SingleEntryTokenMap_StreamWithTokenNotInTheBeggining_EmptyOptionalTokenQueue)
{
    const auto type  = std::string("Any character except line terminators");
    const auto regex = std::string(".");

    const auto token_map = TokenMap({ { type, regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream1 = std::stringstream("\n\t");
    auto stream2 = std::stringstream("\na");
    auto stream3 = std::stringstream("\n1");
    auto stream4 = std::stringstream("\n.");
    auto stream5 = std::stringstream("\rn");
    auto stream6 = std::stringstream("\rt");
    auto stream7 = std::stringstream("\rw");

    auto tokens = tokenizer.Tokenize(stream1);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream2);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream3);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream4);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream5);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream6);
    ASSERT_FALSE(tokens.has_value());

    tokens = tokenizer.Tokenize(stream7);
    ASSERT_FALSE(tokens.has_value());
}

TEST(TokenizerTest,
     Tokenize_TwoEntriesTokenMap_StreamWithTokensInTheBeggining_TokenQueue)
{
    const auto type1  = std::string("Any non-digit character");
    const auto regex1 = std::string("\\D");
    const auto type2  = std::string("Any digit");
    const auto regex2 = std::string("\\d");

    const auto token_map = TokenMap({ { type1, regex1 }, { type2, regex2 } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream1 = std::stringstream("\t");
    auto stream2 = std::stringstream("1");
    auto stream3 = std::stringstream("9a");
    auto stream4 = std::stringstream(".3");
    auto stream5 = std::stringstream("a0n");
    auto stream6 = std::stringstream("5?6");
    auto stream7 = std::stringstream("  4");

    auto tokens = tokenizer.Tokenize(stream1);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ("\t", tokens->front().value);

    tokens = tokenizer.Tokenize(stream2);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("1", tokens->front().value);

    tokens = tokenizer.Tokenize(stream3);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("9", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ("a", tokens->front().value);

    tokens = tokenizer.Tokenize(stream4);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ(".", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("3", tokens->front().value);

    tokens = tokenizer.Tokenize(stream5);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ("a", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("0", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ("n", tokens->front().value);

    tokens = tokenizer.Tokenize(stream6);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("5", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ("?", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("6", tokens->front().value);

    tokens = tokenizer.Tokenize(stream7);
    ASSERT_TRUE(tokens.has_value());
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type1, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();
    EXPECT_EQ(type2, tokens->front().type);
    EXPECT_EQ("4", tokens->front().value);
}

TEST(
    TokenizerTest,
    Tokenize_TwoAmbiguousEntriesTokenMap_StreamWithTokenInTheBeggining_EmptyOptionalTokenQueue)
{
    const auto type1  = std::string("Any character except line terminators");
    const auto regex1 = std::string(".");
    const auto type2  = std::string("Any digit");
    const auto regex2 = std::string("\\d");

    const auto token_map = TokenMap({ { type1, regex1 }, { type2, regex2 } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream("1");

    auto tokens = tokenizer.Tokenize(stream);
    ASSERT_FALSE(tokens.has_value());
}

TEST(
    TokenizerTest,
    Tokenize_TwoSimilarEntriesTokenMap_StreamWithTokenInTheBeggining_LongestTokenTokenQueue)
{
    const auto type1  = std::string("Short token");
    const auto regex1 = std::string("a");
    const auto type2  = std::string("Long token");
    const auto regex2 = std::string("aaaaaaaaaa");

    const auto token_map = TokenMap({ { type1, regex1 }, { type2, regex2 } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream("aaaaaaaaaa");

    auto tokens = tokenizer.Tokenize(stream);
    ASSERT_TRUE(tokens.has_value());
    ASSERT_EQ(1, tokens->size());
    EXPECT_EQ("Long token", tokens->front().type);
    EXPECT_EQ("aaaaaaaaaa", tokens->front().value);
}

TEST(TokenizerTest,
     Tokenize_LanguageTokensTokenMap_StreamWithLoremIpsum_TokenQueue)
{
    const auto first_word_type   = std::string("First word in the sentance");
    const auto first_word_regex  = std::string("[A-Z][a-z]*");
    const auto word_type         = std::string("Word of a sentance");
    const auto word_regex        = std::string("[a-z][a-z]*");
    const auto integer_type      = std::string("Integer number");
    const auto integer_regex     = std::string("-?[1-9][0-9]*");
    const auto real_type         = std::string("Real number");
    const auto real_regex        = std::string("-?([1-9][0-9]*|0).[0-9]*");
    const auto punctuation_type  = std::string("Punctuation mark");
    const auto punctuation_regex = std::string(
        "(\\.\\.\\.)|[\\.,;:\\/\\\\()\\[\\]\\{\\}<>!@#$%\\^&*\\-=_+`~'\"]");
    const auto whitespace_type  = std::string("Whitespace");
    const auto whitespace_regex = std::string("\\s");

    const auto token_map = TokenMap({ { first_word_type, first_word_regex },
                                      { word_type, word_regex },
                                      { integer_type, integer_regex },
                                      { real_type, real_regex },
                                      { punctuation_type, punctuation_regex },
                                      { whitespace_type, whitespace_regex } });
    const auto tokenizer = Tokenizer(token_map);

    auto stream = std::stringstream(
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

    auto tokens = tokenizer.Tokenize(stream);
    ASSERT_TRUE(tokens.has_value());

    EXPECT_EQ(first_word_type, tokens->front().type);
    EXPECT_EQ("Lorem", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("ipsum", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("dolor", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("sit", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("amet", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(punctuation_type, tokens->front().type);
    EXPECT_EQ(",", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("consectetur", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("adipiscing", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(whitespace_type, tokens->front().type);
    EXPECT_EQ(" ", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(word_type, tokens->front().type);
    EXPECT_EQ("elit", tokens->front().value);
    tokens->pop();

    EXPECT_EQ(punctuation_type, tokens->front().type);
    EXPECT_EQ(".", tokens->front().value);
}