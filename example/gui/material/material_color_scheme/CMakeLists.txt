# Material Color Scheme Gallery Example
log_info("Material Color Scheme" "Building Color Gallery Example")

add_executable(material_color_scheme_gallery
    main.cpp
    MaterialColorSchemeMainWindow.h
    MaterialColorSchemeMainWindow.cpp
)

target_link_libraries(material_color_scheme_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# Windows 下设置为 GUI 应用（不显示控制台窗口）
if(WIN32)
    set_target_properties(material_color_scheme_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    # 自动部署 Qt DLL
    qt_add_windows_deploy(material_color_scheme_gallery AUTO_DEPLOY)
endif()
