/**
 * @file    crash_reporter_dialog.h
 * @brief   Post-crash report dialog shown on the next boot.
 *
 * CrashReporterDialog reads a finalized crash JSON (written by cfcrash) and
 * shows the signal, the symbolized stack (or raw addresses when unresolved),
 * and the last log lines, with copy-to-clipboard and dismiss ("don't show
 * again" via a .seen marker) actions. Same frameless + fade/slide popup
 * pattern as AppLauncher/ControlCenter.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash_reporter
 */

#pragma once

#include <QColor>
#include <QRect>
#include <QString>
#include <QWidget>

class QKeyEvent;
class QLabel;
class QPaintEvent;
class QTextEdit;

namespace qw::components::material {
class CFMaterialFadeAnimation;
class CFMaterialSlideAnimation;
} // namespace qw::components::material
namespace qw::widget::material {
class Button;
} // namespace qw::widget::material

namespace cf::desktop::desktop_component {

/**
 * @brief  Popup that surfaces one crash report on the next boot.
 *
 * @ingroup crash_reporter
 */
class CrashReporterDialog : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the dialog for one crash JSON file.
     *
     * @param[in]  crash_json_path  Path to the finalized crash .json.
     * @param[in]  parent           Owning widget (the desktop surface).
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    explicit CrashReporterDialog(const QString& crash_json_path, QWidget* parent = nullptr);

    /**
     * @brief  Destructs the dialog.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    ~CrashReporterDialog() override;

    /**
     * @brief  Centers and shows the dialog.
     *
     * @param[in]  available  The free screen geometry.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    void popup(const QRect& available);

    /**
     * @brief  Starts the exit animation; hides when it finishes.
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    void hidePanel();

    /**
     * @brief  Reports whether the dialog is currently visible.
     *
     * @return     True when shown.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    bool isShowing() const noexcept;

  signals:
    /**
     * @brief  Emitted when the user dismisses the report (it was marked seen).
     *
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    void dismissed();

  protected:
    /**
     * @brief  Paints the rounded Material surface background.
     *
     * @param[in]  event  The paint event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief  Hides the dialog on Escape.
     *
     * @param[in]  event  The key event descriptor.
     * @throws     None
     * @note       None
     * @warning    None
     * @since      0.19.0
     * @ingroup    crash_reporter
     */
    void keyPressEvent(QKeyEvent* event) override;

  private:
    /// @brief Builds the summary / body / button layout.
    void setupUi();
    /// @brief Creates the MD3 enter/exit fade + slide animations.
    void setupAnimations();
    /// @brief Resolves theme colors, then repaints.
    void applyTheme();
    /// @brief Loads the crash JSON into the summary label and body widget.
    void loadCrash();

    /// @brief Path to the crash .json this dialog reports.
    QString crash_path_;

    /// @brief One-line summary (signal + time).
    QLabel* summary_label_{nullptr};
    /// @brief Scrollable stack + log body (read-only).
    QTextEdit* body_edit_{nullptr};
    /// @brief Copy-to-clipboard button.
    qw::widget::material::Button* copy_btn_{nullptr};
    /// @brief Dismiss button (marks seen + hides).
    qw::widget::material::Button* dismiss_btn_{nullptr};

    qw::components::material::CFMaterialFadeAnimation* enter_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* enter_slide_{nullptr};
    qw::components::material::CFMaterialFadeAnimation* exit_fade_{nullptr};
    qw::components::material::CFMaterialSlideAnimation* exit_slide_{nullptr};

    /// @brief Dialog surface fill (surface).
    QColor surface_color_;
};

} // namespace cf::desktop::desktop_component
