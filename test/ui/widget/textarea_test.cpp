/**
 * @file textarea_test.cpp
 * @brief Unit tests for Material Design 3 TextArea widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/textarea/textarea.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(TextAreaTest, A01_DefaultConstructor) {
    TextArea ta;
    EXPECT_NO_FATAL_FAILURE(ta.toPlainText());
    EXPECT_TRUE(ta.isEnabled());
}

// --- B: Property setters ---

TEST(TextAreaTest, B01_SetPlainText) {
    TextArea ta;
    ta.setPlainText("hello");
    EXPECT_EQ(ta.toPlainText(), "hello");
}

TEST(TextAreaTest, B02_SetReadOnly) {
    TextArea ta;
    ta.setReadOnly(true);
    EXPECT_TRUE(ta.isReadOnly());
}

TEST(TextAreaTest, B03_EnableDisable) {
    TextArea ta;
    ta.setEnabled(true);
    EXPECT_TRUE(ta.isEnabled());

    ta.setEnabled(false);
    EXPECT_FALSE(ta.isEnabled());
}

// --- C: Size hints ---

TEST(TextAreaTest, C01_SizeHint_Valid) {
    TextArea ta;
    QSize hint = ta.sizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

// --- D: Paint ---

TEST(TextAreaTest, D01_Paint_WithText_NoCrash) {
    TextArea ta;
    ta.setPlainText("Hello World");
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&ta, QSize(200, 100)));
}

TEST(TextAreaTest, B04_SetVariant) {
    TextArea ta;
    ta.setVariant(TextArea::TextAreaVariant::Filled);
    EXPECT_EQ(ta.variant(), TextArea::TextAreaVariant::Filled);

    ta.setVariant(TextArea::TextAreaVariant::Outlined);
    EXPECT_EQ(ta.variant(), TextArea::TextAreaVariant::Outlined);
}

TEST(TextAreaTest, B05_SetLabel) {
    TextArea ta;
    ta.setLabel("Description");
    EXPECT_EQ(ta.label(), "Description");
}

TEST(TextAreaTest, B06_SetHelperText) {
    TextArea ta;
    ta.setHelperText("Enter a description");
    EXPECT_EQ(ta.helperText(), "Enter a description");
}

TEST(TextAreaTest, B07_SetErrorText) {
    TextArea ta;
    ta.setErrorText("Description is required");
    EXPECT_EQ(ta.errorText(), "Description is required");
}

TEST(TextAreaTest, B08_SetMaxLength) {
    TextArea ta;
    ta.setMaxLength(500);
    EXPECT_EQ(ta.maxLength(), 500);
}

TEST(TextAreaTest, B09_ShowCharacterCounter) {
    TextArea ta;
    ta.setShowCharacterCounter(true);
    EXPECT_TRUE(ta.showCharacterCounter());

    ta.setShowCharacterCounter(false);
    EXPECT_FALSE(ta.showCharacterCounter());
}

TEST(TextAreaTest, B10_SetMinLines) {
    TextArea ta;
    ta.setMinLines(3);
    EXPECT_EQ(ta.minLines(), 3);
}

TEST(TextAreaTest, B11_SetMaxLines) {
    TextArea ta;
    ta.setMaxLines(8);
    EXPECT_EQ(ta.maxLines(), 8);
}

TEST(TextAreaTest, C02_MinimumSizeHint_NotZero) {
    TextArea ta;
    QSize hint = ta.minimumSizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(TextAreaTest, D02_Paint_EnabledDisabled_DifferentPixels) {
    TextArea enabledTa;
    enabledTa.setPlainText("Test content");
    enabledTa.setEnabled(true);

    TextArea disabledTa;
    disabledTa.setPlainText("Test content");
    disabledTa.setEnabled(false);

    QImage enabledImage = widget_test::renderWidgetToImage(&enabledTa, QSize(200, 100));
    QImage disabledImage = widget_test::renderWidgetToImage(&disabledTa, QSize(200, 100));

    EXPECT_TRUE(widget_test::imagesDiffer(enabledImage, disabledImage))
        << "Enabled and disabled TextArea should produce different visual output";
}

TEST(TextAreaTest, D03_Paint_WithLabel_NoCrash) {
    TextArea ta;
    ta.setLabel("Comments");
    ta.setPlainText("Some text here");
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&ta, QSize(200, 100)));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
