set(TargetName "Parser")

set(INCLUDE_DIR include/)
set(HEADER_FILES
    Token.hpp
    Tokenizer.hpp
    TokenizerExceptions.h)
set(SRC_FILES TokenizerExceptions.cpp)

list(TRANSFORM SRC_FILES PREPEND src/)
list(TRANSFORM HEADER_FILES PREPEND include/Parser/)

add_library(${TargetName} ${SRC_FILES} ${HEADER_FILES})

target_include_directories(${TargetName}
PUBLIC
    include
)

add_subdirectory(test)