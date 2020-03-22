def_esm_app(clock)

set(ESM_HSM ON)
include("${CMAKE_SOURCE_DIR}/modules/debouncer.cmake")

set(platfms "linux" "test")

if(${ESM_PLATFORM} IN_LIST platfms)
	set(ESM_APP_IDS ${ESM_APP_IDS} ds3231_sm1)
	target_link_libraries(${APP_NAME} "sox")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
endif()

target_compile_definitions(${APP_NAME}
    PUBLIC
        -DESM_MAX_PRIO=2
        -DSK6812_LEDS_NUM=142
)

target_include_directories(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/include
)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/clock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rtc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/strip.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/player.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/../keyboard/src/backlight_heatmap.c
        ${CMAKE_SOURCE_DIR}/lib/src/sk6812.c
)

end_esm_app()
