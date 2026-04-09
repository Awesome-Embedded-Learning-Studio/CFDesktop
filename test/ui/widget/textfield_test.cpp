/**
 * @file textfield_test.cpp
 * @brief Unit tests for Material Design 3 TextField widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/textfield/textfield.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(TextFieldTest, A01_DefaultConstructor) {
    TextField tf;
    EXPECT_NO_FATAL_FAILURE(tf.text());
    EXPECT_TRUE(tf.isEnabled());
}

// --- B: Property setters ---

TEST(TextFieldTest, B01_SetText) {
    TextField tf;
    tf.setText("hello");
    EXPECT_EQ(tf.text(), "hello");
}

TEST(TextFieldTest, B02_SetPlaceholderText) {
    TextField tf;
    tf.setPlaceholderText("Enter...");
    EXPECT_EQ(tf.placeholderText(), "Enter...");
}

TEST(TextFieldTest, B03_SetReadOnly) {
    TextField tf;
    tf.setReadOnly(true);
    EXPECT_TRUE(tf.isReadOnly());
}

TEST(TextFieldTest, B04_EnableDisable) {
    TextField tf;
    tf.setEnabled(true);
    EXPECT_TRUE(tf.isEnabled());

    tf.setEnabled(false);
    EXPECT_FALSE(tf.isEnabled());
}

// --- C: Size hints ---

TEST(TextFieldTest, C01_SizeHint_NotZero) {
    TextField tf;
    QSize hint = tf.sizeHint();
    EXPECT_FALSE(hint.isNull());
}

// --- D: Paint ---

TEST(TextFieldTest, D01_Paint_WithText_NoCrash) {
    TextField tf;
    tf.setText("Hello World");
    widget_test::verifyPaintsPixels(&tf, QSize(200, 56));
}

TEST(TextFieldTest, D02_Paint_EmptyVsText_DifferentPixels) {
    TextField tf;

    // Render empty
    QImage emptyImage = widget_test::renderWidgetToImage(&tf, QSize(200, 56));

    // Render with text
    tf.setText("Hello World");
    QImage textImage = widget_test::renderWidgetToImage(&tf, QSize(200, 56));

    EXPECT_TRUE(widget_test::imagesDiffer(emptyImage, textImage))
        << "Empty and text renders should produce different visual output";
}

TEST(TextFieldTest, B05_SetVariant) {
    TextField tf;
    tf.setVariant(TextField::TextFieldVariant::Filled);
    EXPECT_EQ(tf.variant(), TextField::TextFieldVariant::Filled);

    tf.setVariant(TextField::TextFieldVariant::Outlined);
    EXPECT_EQ(tf.variant(), TextField::TextFieldVariant::Outlined);
}

TEST(TextFieldTest, B06_SetLabel) {
    TextField tf;
    tf.setLabel("Username");
    EXPECT_EQ(tf.label(), "Username");

    tf.setLabel("");
    EXPECT_TRUE(tf.label().isEmpty());
}

TEST(TextFieldTest, B07_SetHelperText) {
    TextField tf;
    tf.setHelperText("Enter your username");
    EXPECT_EQ(tf.helperText(), "Enter your username");
}

TEST(TextFieldTest, B08_SetErrorText) {
    TextField tf;
    tf.setErrorText("Field is required");
    EXPECT_EQ(tf.errorText(), "Field is required");
}

TEST(TextFieldTest, B09_SetMaxLength) {
    TextField tf;
    tf.setMaxLength(50);
    EXPECT_EQ(tf.maxLength(), 50);
}

TEST(TextFieldTest, B10_ShowCharacterCounter) {
    TextField tf;
    EXPECT_FALSE(tf.showCharacterCounter());

    tf.setShowCharacterCounter(true);
    EXPECT_TRUE(tf.showCharacterCounter());

    tf.setShowCharacterCounter(false);
    EXPECT_FALSE(tf.showCharacterCounter());
}

TEST(TextFieldTest, B11_SetPrefixSuffixIcon) {
    TextField tf;
    EXPECT_NO_FATAL_FAILURE(tf.setPrefixIcon(QIcon()));
    EXPECT_NO_FATAL_FAILURE(tf.setSuffixIcon(QIcon()));
}

TEST(TextFieldTest, C02_MinimumSizeHint_NotZero) {
    TextField tf;
    tf.setText("Sample");
    QSize hint = tf.minimumSizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

TEST(TextFieldTest, D03_Paint_EnabledDisabled_DifferentPixels) {
    TextField enabledTf;
    enabledTf.setText("Test");
    enabledTf.setEnabled(true);

    TextField disabledTf;
    disabledTf.setText("Test");
    disabledTf.setEnabled(false);

    QImage enabledImage = widget_test::renderWidgetToImage(&enabledTf, QSize(200, 56));
    QImage disabledImage = widget_test::renderWidgetToImage(&disabledTf, QSize(200, 56));

    EXPECT_TRUE(widget_test::imagesDiffer(enabledImage, disabledImage))
        << "Enabled and disabled TextField should produce different visual output";
}

TEST(TextFieldTest, D04_Paint_Variant_DifferentPixels) {
    TextField filledTf("Text", TextField::TextFieldVariant::Filled);

    TextField outlinedTf("Text", TextField::TextFieldVariant::Outlined);

    QImage filledImage = widget_test::renderWidgetToImage(&filledTf, QSize(200, 56));
    QImage outlinedImage = widget_test::renderWidgetToImage(&outlinedTf, QSize(200, 56));

    EXPECT_TRUE(widget_test::imagesDiffer(filledImage, outlinedImage))
        << "Filled and Outlined TextField variants should produce different visual output";
}

TEST(TextFieldTest, D05_Paint_WithLabel_NoCrash) {
    TextField tf;
    tf.setLabel("Email");
    tf.setText("user@example.com");
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&tf, QSize(200, 56)));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
