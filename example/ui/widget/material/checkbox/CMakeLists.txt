log_info("CheckBox Example" "Building Material CheckBox Example")

add_executable(checkbox_example
    main.cpp
    CheckBoxExampleWindow.h
    CheckBoxExampleWindow.cpp
)

target_link_libraries(checkbox_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(checkbox_example "ui")
cf_register_example_launcher(checkbox_example "ui")

if(WIN32)
    set_target_properties(checkbox_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
