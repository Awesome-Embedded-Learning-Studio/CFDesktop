# Static library (has .cpp sources)
add_library(cffilesystem STATIC)

# Sources
target_sources(cffilesystem PRIVATE
    file_op.cpp
    filter_target.cpp
)

# Include directories
target_include_directories(cffilesystem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(cffilesystem PUBLIC
    Qt6::Core
)

# Alias for consistent naming
add_library(CFDesktop::filesystem ALIAS cffilesystem)
