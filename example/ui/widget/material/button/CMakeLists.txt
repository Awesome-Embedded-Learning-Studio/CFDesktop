log_info("Button Example" "Building Material Button Example")

add_executable(button_example
    main.cpp
    ButtonExampleWindow.h
    ButtonExampleWindow.cpp
)

target_link_libraries(button_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(button_example "ui")
cf_register_example_launcher(button_example "ui")

if(WIN32)
    set_target_properties(button_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
