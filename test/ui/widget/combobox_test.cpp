/**
 * @file combobox_test.cpp
 * @brief Unit tests for Material Design 3 ComboBox widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/comboBox/combobox.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(ComboBoxTest, A01_DefaultConstructor) {
    ComboBox cb;
    EXPECT_NO_FATAL_FAILURE(cb.count());
    EXPECT_TRUE(cb.isEnabled());
}

// --- B: Property setters ---

TEST(ComboBoxTest, B01_AddItems) {
    ComboBox cb;
    cb.addItems({"A", "B", "C"});
    EXPECT_EQ(cb.count(), 3);
}

TEST(ComboBoxTest, B02_SetCurrentIndex) {
    ComboBox cb;
    cb.addItems({"A", "B", "C"});
    cb.setCurrentIndex(1);
    EXPECT_EQ(cb.currentIndex(), 1);
}

TEST(ComboBoxTest, B03_EnableDisable) {
    ComboBox cb;
    cb.setEnabled(true);
    EXPECT_TRUE(cb.isEnabled());

    cb.setEnabled(false);
    EXPECT_FALSE(cb.isEnabled());
}

TEST(ComboBoxTest, B04_SetVariant) {
    ComboBox cb;
    cb.setVariant(ComboBox::ComboBoxVariant::Filled);
    EXPECT_EQ(cb.variant(), ComboBox::ComboBoxVariant::Filled);

    cb.setVariant(ComboBox::ComboBoxVariant::Outlined);
    EXPECT_EQ(cb.variant(), ComboBox::ComboBoxVariant::Outlined);
}

// --- C: Size hints ---

TEST(ComboBoxTest, C01_SizeHint_NotZero) {
    ComboBox cb;
    cb.addItems({"A", "B", "C"});
    QSize hint = cb.sizeHint();
    EXPECT_FALSE(hint.isNull());
}

TEST(ComboBoxTest, C02_MinimumSizeHint_NotZero) {
    ComboBox cb;
    cb.addItems({"A", "B", "C"});
    QSize hint = cb.minimumSizeHint();
    EXPECT_GT(hint.width(), 0);
    EXPECT_GT(hint.height(), 0);
}

// --- D: Paint ---

TEST(ComboBoxTest, D01_Paint_WithItems_NoCrash) {
    ComboBox cb;
    cb.addItems({"A", "B", "C"});
    cb.setCurrentIndex(0);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&cb, QSize(200, 48)));
}

TEST(ComboBoxTest, D02_Paint_Variant_DifferentPixels) {
    ComboBox filledCb;
    filledCb.addItems({"A", "B", "C"});
    filledCb.setVariant(ComboBox::ComboBoxVariant::Filled);
    filledCb.setCurrentIndex(0);
    QImage filledImage = widget_test::renderWidgetToImage(&filledCb, QSize(200, 48));

    ComboBox outlinedCb;
    outlinedCb.addItems({"A", "B", "C"});
    outlinedCb.setVariant(ComboBox::ComboBoxVariant::Outlined);
    outlinedCb.setCurrentIndex(0);
    QImage outlinedImage = widget_test::renderWidgetToImage(&outlinedCb, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(filledImage, outlinedImage))
        << "Filled and Outlined ComboBox variants should produce different visual output";
}

TEST(ComboBoxTest, D03_Paint_DifferentIndex_DifferentPixels) {
    ComboBox cb;
    cb.addItems({"Short", "A Much Longer Item Name", "C"});
    cb.setCurrentIndex(0);
    QImage img0 = widget_test::renderWidgetToImage(&cb, QSize(200, 48));

    cb.setCurrentIndex(1);
    QImage img1 = widget_test::renderWidgetToImage(&cb, QSize(200, 48));

    EXPECT_TRUE(widget_test::imagesDiffer(img0, img1))
        << "ComboBox with different selected items should produce different visual output";
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
