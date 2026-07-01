/**
 * @file    apps/noter/noter_panel.h
 * @brief   Noter main panel (standalone app).
 *
 * Root widget of the standalone Noter executable. Plain-text editor with
 * basic font formatting (size/bold/italic) and open/save, rendered with
 * QuarkWidgets MD3 buttons. Ported from CCIMXDesktop's CCIMXNoter.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup noter
 */

#pragma once

#include <QWidget>

class QKeyEvent;
class QLabel;
class QPaintEvent;
class QSlider;
class QTextCharFormat;
class QTextEdit;

namespace qw::widget::material {
class Button;
}

namespace cf::desktop::desktop_component {

/**
 * @brief  Root widget of the standalone Noter application.
 *
 * @ingroup noter
 */
class NoterPanel final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the Noter panel.
     *
     * @param[in] parent  Parent widget (nullptr for a top-level window).
     *
     * @throws  None
     * @since   0.20
     * @ingroup noter
     */
    explicit NoterPanel(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the panel.
     *
     * @throws  None
     * @since   0.20
     * @ingroup noter
     */
    ~NoterPanel() override;

  protected:
    /**
     * @brief   Paints the rounded Material card background.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws  None
     * @since   0.20
     * @ingroup noter
     */
    void paintEvent(QPaintEvent* event) override;

  private slots:
    /// @brief Opens a file dialog and loads text into the editor.
    void onOpen();
    /// @brief Opens a file dialog and saves the editor text.
    void onSave();
    /// @brief Applies the slider font size to the current selection.
    void onFontSizeChanged(int size);
    /// @brief Toggles bold on the current selection.
    void onBoldToggled(bool checked);
    /// @brief Toggles italic on the current selection.
    void onItalicToggled(bool checked);

  private:
    /// @brief Builds the toolbar + editor layout.
    void setupUi();
    /// @brief Merges a char format onto the current word or selection.
    void applyCharFormat(const QTextCharFormat& format);

    QTextEdit* editor_{nullptr};    ///< Plain-text edit area.
    QSlider* font_slider_{nullptr}; ///< Font size slider (8..40).
    QLabel* size_label_{nullptr};   ///< Current font size readout.
};

} // namespace cf::desktop::desktop_component
