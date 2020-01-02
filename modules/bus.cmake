SET(ESM_APP_SIGNALS ${ESM_APP_SIGNALS} bus_req bus_rsp bus_ack)

target_sources(${APP_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/src/bus.c"
)
