log_info("Material Widget Gallery" "Building Material Widget Gallery")

add_executable(material_widget_gallery
    main.cpp
    MaterialGalleryWindow.h
    MaterialGalleryWindow.cpp
    demos/ButtonDemo.cpp
    demos/ButtonDemo.h
    demos/CheckBoxDemo.cpp
    demos/CheckBoxDemo.h
    demos/ComboBoxDemo.cpp
    demos/ComboBoxDemo.h
    demos/GroupBoxDemo.cpp
    demos/GroupBoxDemo.h
    demos/LabelDemo.cpp
    demos/LabelDemo.h
    demos/ProgressBarDemo.cpp
    demos/ProgressBarDemo.h
    demos/RadioButtonDemo.cpp
    demos/RadioButtonDemo.h
    demos/TextAreaDemo.cpp
    demos/TextAreaDemo.h
    demos/TextFieldDemo.cpp
    demos/TextFieldDemo.h
    demos/SliderDemo.cpp
    demos/SliderDemo.h
    demos/SwitchDemo.cpp
    demos/SwitchDemo.h
)

target_link_libraries(material_widget_gallery PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    cfui
)

# 设置输出目录到 examples/ui/
cf_set_example_output_dir(material_widget_gallery "ui")
cf_register_example_launcher(material_widget_gallery "ui")

if(WIN32)
    set_target_properties(material_widget_gallery PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
