/**
 * @file    apps/calendar/calendar_panel.h
 * @brief   Calendar main panel (standalone app).
 *
 * Root widget of the standalone Calendar executable. Renders a month calendar
 * (Qt QCalendarWidget, MVP) with a side panel that shows and edits notes bound
 * to the selected date. Notes are kept in an in-memory map for the MVP;
 * persistence is deferred (see TODO in the .cpp). Ported from CCIMXDesktop's
 * CCCalendar, with the original QMainWindow + .ui layout rewritten as a plain
 * QWidget using QuarkWidgets MD3 buttons.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup calendar
 */

#pragma once

#include <QDate>
#include <QMap>
#include <QString>
#include <QWidget>

class QCalendarWidget;
class QLabel;
class QPaintEvent;
class QTextEdit;

namespace qw::widget::material {
class Button;
}

namespace cf::desktop::desktop_component {

/**
 * @brief  Root widget of the standalone Calendar application.
 *
 * @ingroup calendar
 */
class CalendarPanel final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the Calendar panel.
     *
     * @param[in] parent  Parent widget (nullptr for a top-level window).
     *
     * @throws  None
     * @since   0.20
     * @ingroup calendar
     */
    explicit CalendarPanel(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the panel.
     *
     * @throws  None
     * @since   0.20
     * @ingroup calendar
     */
    ~CalendarPanel() override;

  protected:
    /**
     * @brief   Paints the rounded Material card background.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws  None
     * @since   0.20
     * @ingroup calendar
     */
    void paintEvent(QPaintEvent* event) override;

  private slots:
    /// @brief Loads the selected date's note into the editor and updates the
    ///        date description label.
    void onSelectionChanged();
    /// @brief Commits the editor text back into the note store for the current
    ///        date.
    void onSaveNote();
    /// @brief Removes the current date's note and clears the editor.
    void onDeleteNote();

  private:
    /// @brief Builds the calendar + editor layout.
    void setupUi();
    /// @brief Refreshes the date description label for the selected date.
    void refreshDateDescription();
    /// @brief Returns a human-readable description of @p date.
    static QString describeDate(const QDate& date);

    QCalendarWidget* calendar_{nullptr}; ///< Month calendar (MVP).
    QTextEdit* editor_{nullptr};         ///< Note editor for the selected date.
    QLabel* date_label_{nullptr};        ///< Selected-date read-out / description.

    /// @brief In-memory note store: selected date -> note text.
    ///
    /// TODO: replace with persistent storage (JSON file under the user data
    /// dir, or desktop ConfigStore) in a follow-up. Lost on app exit today.
    QMap<QDate, QString> notes_;
};

} // namespace cf::desktop::desktop_component
