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

# 设置输出目录到 examples/gui/
cf_set_example_output_dir(material_typography_gallery "gui")
cf_register_example_launcher(material_typography_gallery "gui")

# Windows 下设置为 GUI 应用（不显示控制台窗口）
if(WIN32)
    set_target_properties(material_typography_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
