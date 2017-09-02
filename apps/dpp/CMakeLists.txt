set(ESM_TARGET dpp)
add_library(${ESM_TARGET}Src STATIC "")
include("${CMAKE_SOURCE_DIR}/Core.cmake")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
)

add_esm_executable(
    ${ESM_TARGET}
    src/philo.c
    src/table.c
)
target_link_libraries(${ESM_TARGET} ${ESM_TARGET}Src)
