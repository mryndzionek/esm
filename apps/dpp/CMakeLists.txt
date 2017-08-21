include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_executable(
    dpp
    ${ESM_SOURCES}
    src/philo.c
    src/table.c
)
