# Collect impl directory sources
# Paths relative to desktop/main (parent scope)

set(IMPL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/console_signal_stage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/early_welcome_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger_stage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/early_config_stage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/desktop_backbone_setup.cpp
)

# Return to parent via PARENT_SCOPE
set(IMPL_SOURCES ${IMPL_SOURCES} PARENT_SCOPE)
