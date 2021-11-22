set(TargetName "Tokenizer.UnitTests")

set(SRC_FILES Tokenizer.UnitTests.cpp)

add_executable(${TargetName} ${SRC_FILES})

target_link_libraries(${TargetName}
PUBLIC
	gtest
	gtest_main
	Parser
)

add_test(NAME ${TargetName} COMMAND ${TargetName})