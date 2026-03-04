log_info("RadioButton Example" "Building Material RadioButton Example")

add_executable(radiobutton_example
    main.cpp
    RadioButtonExampleWindow.h
    RadioButtonExampleWindow.cpp
)

target_link_libraries(radiobutton_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(radiobutton_example "ui")
cf_register_example_launcher(radiobutton_example "ui")

if(WIN32)
    set_target_properties(radiobutton_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
