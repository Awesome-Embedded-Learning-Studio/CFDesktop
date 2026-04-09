# Wallpaper layer components
add_library(cfdesktop_wallpaper STATIC
    WallPaperToken.cpp
    WallPaperAccessStorage.cpp
    ImageWallPaperLayer.cpp
)

target_include_directories(cfdesktop_wallpaper PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cfdesktop_wallpaper PRIVATE
    Qt6::Gui
    cflogger_headers
    cfbase
    cf_ui_base
)
target_compile_definitions(cfdesktop_wallpaper PRIVATE CFLOG_STATIC_BUILD)
