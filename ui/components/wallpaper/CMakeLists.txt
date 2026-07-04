# Wallpaper layer components
add_library(cfdesktop_wallpaper STATIC
    WallPaperToken.cpp
    WallPaperAccessStorage.cpp
    ImageWallPaperLayer.cpp
    WallPaperEngine.cpp
    TransitionComposer.cpp
)

target_include_directories(cfdesktop_wallpaper PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

target_link_libraries(cfdesktop_wallpaper PRIVATE
    Qt6::Gui
    cflogger
    cfbase
    QuarkWidgets::quarkwidgets
    cfconfig
)