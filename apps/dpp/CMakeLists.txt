set(ESM_TARGET dpp)
add_library(${ESM_TARGET}Src STATIC "${ESM_CORE}")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")
target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

add_executable(
    ${ESM_TARGET}
    ${ESM_SOURCES}
    src/philo.c
    src/table.c
)
target_link_libraries(dpp ${ESM_TARGET}Src)
