# =============================================================================
# Widget Tests - Material Design 3 Widget Unit Tests
# =============================================================================

# Common link libraries for widget tests
# Note: Use cfui shared library to match __declspec(dllimport) in headers.
# No gtest_main — each test provides its own main() with QGuiApplication
set(WIDGET_TEST_LIBS
    cfui;Qt6::Widgets;GTest::gtest
)

# =============================================================================
# Button
# =============================================================================
add_gtest_executable(
    TEST_NAME      button_test
    SOURCE_FILE    button_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;button"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# CheckBox
# =============================================================================
add_gtest_executable(
    TEST_NAME      checkbox_test
    SOURCE_FILE    checkbox_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;checkbox"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# RadioButton
# =============================================================================
add_gtest_executable(
    TEST_NAME      radiobutton_test
    SOURCE_FILE    radiobutton_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;radiobutton"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# Switch
# =============================================================================
add_gtest_executable(
    TEST_NAME      switch_test
    SOURCE_FILE    switch_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;switch"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# Slider
# =============================================================================
add_gtest_executable(
    TEST_NAME      slider_test
    SOURCE_FILE    slider_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;slider"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# TextField
# =============================================================================
add_gtest_executable(
    TEST_NAME      textfield_test
    SOURCE_FILE    textfield_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;textfield"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# TextArea
# =============================================================================
add_gtest_executable(
    TEST_NAME      textarea_test
    SOURCE_FILE    textarea_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;textarea"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# Label
# =============================================================================
add_gtest_executable(
    TEST_NAME      label_test
    SOURCE_FILE    label_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;label"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# ProgressBar
# =============================================================================
add_gtest_executable(
    TEST_NAME      progressbar_test
    SOURCE_FILE    progressbar_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;progressbar"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# ComboBox
# =============================================================================
add_gtest_executable(
    TEST_NAME      combobox_test
    SOURCE_FILE    combobox_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;combobox"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# GroupBox
# =============================================================================
add_gtest_executable(
    TEST_NAME      groupbox_test
    SOURCE_FILE    groupbox_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;groupbox"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# ListView
# =============================================================================
add_gtest_executable(
    TEST_NAME      listview_test
    SOURCE_FILE    listview_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;listview"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# TableView
# =============================================================================
add_gtest_executable(
    TEST_NAME      tableview_test
    SOURCE_FILE    tableview_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;tableview"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# TreeView
# =============================================================================
add_gtest_executable(
    TEST_NAME      treeview_test
    SOURCE_FILE    treeview_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;treeview"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# TabView
# =============================================================================
add_gtest_executable(
    TEST_NAME      tabview_test
    SOURCE_FILE    tabview_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;tabview"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# ScrollView
# =============================================================================
add_gtest_executable(
    TEST_NAME      scrollview_test
    SOURCE_FILE    scrollview_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;scrollview"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# Separator
# =============================================================================
add_gtest_executable(
    TEST_NAME      separator_test
    SOURCE_FILE    separator_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;separator"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# SpinBox
# =============================================================================
add_gtest_executable(
    TEST_NAME      spinbox_test
    SOURCE_FILE    spinbox_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;spinbox"
    LOG_MODULE     ui_widget_tests
)

# =============================================================================
# DoubleSpinBox
# =============================================================================
add_gtest_executable(
    TEST_NAME      doublespinbox_test
    SOURCE_FILE    doublespinbox_test.cpp
    LINK_LIBRARIES ${WIDGET_TEST_LIBS}
    LABELS         "ui;widget;doublespinbox"
    LOG_MODULE     ui_widget_tests
)

log_info("UI_Widget_Tests" "Configured ui/widget tests")
