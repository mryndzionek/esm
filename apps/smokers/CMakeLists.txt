set(ESM_TARGET smokers)
add_library(${ESM_TARGET}Src STATIC "${ESM_CORE}")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")
target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

add_executable(
    ${ESM_TARGET}
    ${ESM_SOURCES}
    src/table.c
    src/smoker.c
)
target_link_libraries(smokers ${ESM_TARGET}Src)
