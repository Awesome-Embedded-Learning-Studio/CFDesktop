# Material Gallery Example
log_info("Material Gallery" "Building Material Gallery Example")

add_executable(material_gallery
    main.cpp
    MaterialGalleryMainWindow.h
    MaterialGalleryMainWindow.cpp
    ThemeSidebar.h
    ThemeSidebar.cpp
    ThemePageWidget.h
    ThemePageWidget.cpp
    ToastWidget.h
    ToastWidget.cpp
    ColorSchemePage.h
    ColorSchemePage.cpp
    MotionSpecPage.h
    MotionSpecPage.cpp
    RadiusScalePage.h
    RadiusScalePage.cpp
    TypographyPage.h
    TypographyPage.cpp
)

target_link_libraries(material_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/gui/
cf_set_example_output_dir(material_gallery "gui")
cf_register_example_launcher(material_gallery "gui")

# Windows 下设置为 GUI 应用（不显示控制台窗口）
if(WIN32)
    set_target_properties(material_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
