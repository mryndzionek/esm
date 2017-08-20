include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(
    blink
    ${ESM_SOURCES}
    src/blink.c
)
