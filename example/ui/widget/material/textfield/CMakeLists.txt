log_info("TextField Example" "Building Material TextField Example")

add_executable(textfield_example
    main.cpp
    TextFieldExampleWindow.h
    TextFieldExampleWindow.cpp
)

target_link_libraries(textfield_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(textfield_example "ui")
cf_register_example_launcher(textfield_example "ui")

if(WIN32)
    set_target_properties(textfield_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
