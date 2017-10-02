set(ESM_TARGET prodcons)
add_library(${ESM_TARGET}Src STATIC "")
include("${CMAKE_SOURCE_DIR}/Core.cmake")
include("${CMAKE_SOURCE_DIR}/platform/${ESM_PLATFORM}/CMakeLists.txt")

target_compile_definitions(${ESM_TARGET}Src
    PUBLIC
        -DESM_MAX_PRIO=1
)

target_include_directories(${ESM_TARGET}Src
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/include"
        "${CMAKE_SOURCE_DIR}/modules/include"
)

target_sources(${ESM_TARGET}Src
    PRIVATE
        "${CMAKE_SOURCE_DIR}/modules/src/bus.c"
)

add_esm_executable(
    ${ESM_TARGET}
    src/producer.c
    src/consumer.c
)
target_link_libraries(${ESM_TARGET} ${ESM_TARGET}Src)
