# Material Radius Scale Gallery Example
log_info("Material Radius Scale" "Building Radius Scale Gallery Example")

add_executable(material_radius_scale_gallery
    main.cpp
    MaterialRadiusScaleMainWindow.h
    MaterialRadiusScaleMainWindow.cpp
)

target_link_libraries(material_radius_scale_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/gui/
cf_set_example_output_dir(material_radius_scale_gallery "gui")
cf_register_example_launcher(material_radius_scale_gallery "gui")

# Windows 下设置为 GUI 应用（不显示控制台窗口）
if(WIN32)
    set_target_properties(material_radius_scale_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
