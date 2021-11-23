set(TargetName "Parser")

set(INCLUDE_DIR include/)
set(HEADER_FILES Token.h Tokenizer.h)
set(SRC_FILES )

list(TRANSFORM SRC_FILES PREPEND src/)
list(TRANSFORM HEADER_FILES PREPEND include/Parser/)

# add_library(${TargetName} ${SRC_FILES} ${HEADER_FILES})
add_library(${TargetName} INTERFACE)

# target_include_directories(${TargetName}
# PUBLIC
#     include
# )
target_include_directories(${TargetName}
INTERFACE
    include
)

add_subdirectory(test)