/**
 * @file separator_test.cpp
 * @brief Unit tests for Material Design 3 Separator widget.
 */

#include "widget_test_helper.h"

#include <QApplication>
#include <gtest/gtest.h>

#include "ui/widget/material/widget/separator/separator.h"

using namespace cf::ui::widget::material;

// --- A: Construction ---

TEST(SeparatorTest, A01_DefaultConstructor) {
    Separator sep;
    EXPECT_NO_FATAL_FAILURE(sep.mode());
    EXPECT_TRUE(sep.isEnabled());
}

// --- B: Property setters ---

TEST(SeparatorTest, B01_EnableDisable) {
    Separator sep;
    sep.setEnabled(true);
    EXPECT_TRUE(sep.isEnabled());

    sep.setEnabled(false);
    EXPECT_FALSE(sep.isEnabled());
}

TEST(SeparatorTest, B02_SetMode) {
    Separator sep;
    sep.setMode(SeparatorMode::FullBleed);
    EXPECT_EQ(sep.mode(), SeparatorMode::FullBleed);

    sep.setMode(SeparatorMode::Inset);
    EXPECT_EQ(sep.mode(), SeparatorMode::Inset);

    sep.setMode(SeparatorMode::MiddleInset);
    EXPECT_EQ(sep.mode(), SeparatorMode::MiddleInset);
}

TEST(SeparatorTest, B03_SetOrientation) {
    Separator sep;
    sep.setOrientation(Qt::Horizontal);
    EXPECT_EQ(sep.orientation(), Qt::Horizontal);

    sep.setOrientation(Qt::Vertical);
    EXPECT_EQ(sep.orientation(), Qt::Vertical);
}

// --- C: Size hints ---

TEST(SeparatorTest, C01_SizeHint_NotZero) {
    Separator sep;
    QSize hint = sep.sizeHint();
    EXPECT_FALSE(hint.isNull());
}

TEST(SeparatorTest, C02_SizeHint_Horizontal_Vertical) {
    Separator hSep(Qt::Horizontal);
    EXPECT_GT(hSep.sizeHint().width(), 0);

    Separator vSep(Qt::Vertical);
    EXPECT_GT(vSep.sizeHint().height(), 0);
}

// --- D: Paint ---

TEST(SeparatorTest, D01_Paint_NoCrash) {
    Separator sep;
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&sep, QSize(200, 2)));
}

TEST(SeparatorTest, D02_Paint_AllModes_NoCrash) {
    const SeparatorMode modes[] = {SeparatorMode::FullBleed, SeparatorMode::Inset,
                                   SeparatorMode::MiddleInset};
    for (auto mode : modes) {
        Separator sep(Qt::Horizontal);
        sep.setMode(mode);
        EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&sep, QSize(200, 4)));
    }
}

TEST(SeparatorTest, D03_Paint_Vertical_NoCrash) {
    Separator sep(Qt::Vertical);
    EXPECT_NO_FATAL_FAILURE(widget_test::renderWidgetToImage(&sep, QSize(4, 200)));
}

int main(int argc, char* argv[]) {
    qputenv("QT_QPA_PLATFORM", "offscreen");
    ensureFontsAvailable();
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
