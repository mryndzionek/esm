include_directories("include")
add_executable(
    dpp
    ${ESM_SOURCES}
    src/philo.c
    src/table.c
)
