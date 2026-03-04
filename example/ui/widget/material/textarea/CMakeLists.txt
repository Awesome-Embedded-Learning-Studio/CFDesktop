log_info("TextArea Example" "Building Material TextArea Example")

add_executable(textarea_example
    main.cpp
    TextAreaExampleWindow.h
    TextAreaExampleWindow.cpp
)

target_link_libraries(textarea_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(textarea_example "ui")
cf_register_example_launcher(textarea_example "ui")

if(WIN32)
    set_target_properties(textarea_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
