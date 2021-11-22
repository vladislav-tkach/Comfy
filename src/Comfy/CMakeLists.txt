set(TargetName "Comfy")

set(SRC_FILES main.cpp)

list(TRANSFORM SRC_FILES PREPEND src/)

add_executable(${TargetName} ${SRC_FILES})

target_link_libraries(${TargetName}
PUBLIC
    Parser
)