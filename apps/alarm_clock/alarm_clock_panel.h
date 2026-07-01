/**
 * @file    apps/alarm_clock/alarm_clock_panel.h
 * @brief   Alarm Clock main panel (standalone app).
 *
 * Root widget of the standalone Alarm Clock executable. Provides a live
 * digital clock readout, a time editor (hour/minute spinboxes with +/- MD3
 * buttons), a note field, an add/remove control row, and a list of armed
 * alarms. Each armed alarm is checked against the wall clock once per second;
 * when its time arrives the alarm fires and a Material message box shows the
 * note. Ported from CCIMXDesktop's AlarmyClock — the original QPainter analog
 * dial, QMainWindow shell, .ui files, and broadcaster/processor event bus are
 * replaced by a single QuarkWidgets MD3 panel built in code.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup alarm_clock
 */

#pragma once

#include <QMetaType>
#include <QTime>
#include <QWidget>

class QListWidget;
class QListWidgetItem;
class QPaintEvent;
class QSpinBox;
class QTextEdit;
class QTimer;

namespace qw::widget::material {
class Button;
class Label;
} // namespace qw::widget::material

namespace cf::desktop::desktop_component {

/**
 * @brief  Represents one armed alarm (target time + user note).
 *
 * @ingroup alarm_clock
 */
struct AlarmEntry {
    QTime time;   ///< Target wall-clock time (hh:mm, seconds ignored).
    QString note; ///< Reminder text shown when the alarm fires.
};

/**
 * @brief  Root widget of the standalone Alarm Clock application.
 *
 * @ingroup alarm_clock
 */
class AlarmClockPanel final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the Alarm Clock panel.
     *
     * @param[in] parent  Parent widget (nullptr for a top-level window).
     *
     * @throws  None
     * @since   0.20
     * @ingroup alarm_clock
     */
    explicit AlarmClockPanel(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the panel.
     *
     * @throws  None
     * @since   0.20
     * @ingroup alarm_clock
     */
    ~AlarmClockPanel() override;

  protected:
    /**
     * @brief   Paints the rounded Material card background.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws  None
     * @since   0.20
     * @ingroup alarm_clock
     */
    void paintEvent(QPaintEvent* event) override;

  private slots:
    /// @brief Refreshes the live clock label from the wall clock.
    void onTick();
    /// @brief Arms a new alarm from the editor fields.
    void onAddAlarm();
    /// @brief Removes the currently selected armed alarm.
    void onRemoveAlarm();

  private:
    /**
     * @brief   Builds the editor + armed-alarms layout.
     *
     * @throws  None
     * @since   0.20
     * @ingroup alarm_clock
     */
    void setupUi();

    /**
     * @brief   Fires the alarm identified by @p item.
     *
     * Shows a Material message box with the alarm note and, if the alarm is
     * non-repeating, disarms it. Audible ringing is a TODO (see @ref onTick).
     *
     * @param[in] item  The list item whose alarm time has arrived.
     *
     * @throws  None
     * @since   0.20
     * @ingroup alarm_clock
     */
    void fireAlarm(QListWidgetItem* item);

    /// @brief Live digital clock readout.
    qw::widget::material::Label* clock_label_{nullptr};
    QSpinBox* hour_spin_{nullptr};     ///< Hour editor (0-23).
    QSpinBox* minute_spin_{nullptr};   ///< Minute editor (0-59).
    QTextEdit* note_edit_{nullptr};    ///< Reminder note editor.
    QListWidget* alarm_list_{nullptr}; ///< Armed alarms list.
    QTimer* ticker_{nullptr};          ///< Per-second wall-clock poll.
};

} // namespace cf::desktop::desktop_component

/// @brief Enables storing AlarmEntry in a QVariant (QListWidgetItem data).
Q_DECLARE_METATYPE(cf::desktop::desktop_component::AlarmEntry)
