/**
 * @file    system_usage_card.h
 * @brief   Reusable usage card (title + percentage + progress bar + detail).
 *
 * A dark-gradient card (CCIMX MemoryUsageCard / DiskUsageCardWidget style)
 * parameterized by a probe callable and a poll interval, so Memory / CPU /
 * Disk share one implementation. Probe returns {percentage, detail line}.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include <QString>
#include <QWidget>

#include <functional>

class QLabel;
class QProgressBar;
class QTimer;

namespace cf::desktop::desktop_component {

/// @brief A single usage sample returned by a SystemUsageCard probe.
struct UsageSample {
    int pct;        ///< Usage percentage (0-100).
    QString detail; ///< Human-readable line (e.g. "7.9 / 12.7 GB").
};

/**
 * @brief  Reusable system-usage card driven by a probe callable.
 *
 * @note   Polls the probe on a coarse timer; the card itself is theme-less
 *         (hardcoded QSS, CCIMX style).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class SystemUsageCard : public QWidget {
    Q_OBJECT

  public:
    /// @brief Callable returning the current UsageSample.
    using Probe = std::function<UsageSample()>;

    /**
     * @brief  Constructs the usage card.
     * @param[in]  title        Card title (e.g. "Memory").
     * @param[in]  probe        Callable returning the current sample.
     * @param[in]  interval_ms  Poll interval in milliseconds.
     * @param[in]  parent       Owning Qt parent widget.
     * @throws     None
     * @note       Polls once at construction, then every interval_ms.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    SystemUsageCard(const QString& title, Probe probe, int interval_ms, QWidget* parent = nullptr);

  private:
    /// @brief Runs the probe and updates the labels + bar.
    void refresh();

    QLabel* title_label_;  ///< Card title.
    QLabel* value_label_;  ///< Percentage text.
    QProgressBar* bar_;    ///< Progress bar.
    QLabel* detail_label_; ///< Detail line.
    Probe probe_;          ///< Sample source.
    QTimer* timer_;        ///< Poll timer.
};

} // namespace cf::desktop::desktop_component
