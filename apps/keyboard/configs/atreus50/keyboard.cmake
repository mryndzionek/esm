SET(ESM_APP_IDS ${ESM_APP_IDS} tap_detector1 tap_detector2)
SET(ESM_APP_SIGNALS ${ESM_APP_SIGNALS} tap)
SET(ESM_APP_GROUPS ${ESM_APP_GROUPS} taps)

target_sources(${APP_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tap_detector.c
)

