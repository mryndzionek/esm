include_directories("include")
add_executable(
    smokers
    ${ESM_SOURCES}
    src/table.c
    src/smoker.c
)
