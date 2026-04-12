# Config manager module
# Static library (has .cpp sources)
add_library(cfconfig STATIC)

# Sources
target_sources(cfconfig PRIVATE
    src/cfconfig.cpp
    src/cfconfig_path_provider.cpp
    src/impl/config_impl.cpp
    src/impl/config_domain.cpp
    src/impl/config_key.cpp
    src/impl/config_backend_factory.cpp
    src/impl/qsettings_backend.cpp
    src/impl/json_backend.cpp
)

# Include directories
target_include_directories(cfconfig PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(cfconfig PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(cfconfig PUBLIC
    CFDesktop::base
    Qt6::Core
)

# Alias for consistent naming
add_library(CFDesktop::config ALIAS cfconfig)
