# Material Typography Gallery Example
log_info("Material Typography" "Building Typography Gallery Example")

add_executable(material_typography_gallery
    main.cpp
    MaterialTypographyMainWindow.h
    MaterialTypographyMainWindow.cpp
)

target_link_libraries(material_typography_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# Windows 下设置为 GUI 应用（不显示控制台窗口）
if(WIN32)
    set_target_properties(material_typography_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    # 自动部署 Qt DLL
    qt_add_windows_deploy(material_typography_gallery AUTO_DEPLOY)
endif()
