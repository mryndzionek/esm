include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(
    smokers
    ${ESM_SOURCES}
    src/table.c
    src/smoker.c
)
