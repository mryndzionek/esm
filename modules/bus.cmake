SET(ESM_MODULE_SIGNALS ${ESM_MODULE_SIGNALS} bus_req bus_rsp bus_ack)

target_sources(${APP_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/src/bus.c"
)
