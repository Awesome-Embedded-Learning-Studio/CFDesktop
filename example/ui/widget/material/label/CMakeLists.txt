log_info("Label Example" "Building Material Label Example")

add_executable(label_example
    main.cpp
    LabelExampleWindow.h
    LabelExampleWindow.cpp
)

target_link_libraries(label_example PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(label_example "ui")
cf_register_example_launcher(label_example "ui")

if(WIN32)
    set_target_properties(label_example PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
