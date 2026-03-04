log_info("GroupBox Example" "Building Material GroupBox Example")

add_executable(groupbox_example
    main.cpp
    GroupBoxExampleWindow.h
    GroupBoxExampleWindow.cpp
)

target_link_libraries(groupbox_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(groupbox_example "ui")
cf_register_example_launcher(groupbox_example "ui")

if(WIN32)
    set_target_properties(groupbox_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
