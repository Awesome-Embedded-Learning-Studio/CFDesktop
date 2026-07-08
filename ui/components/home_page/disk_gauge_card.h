/**
 * @file    disk_gauge_card.h
 * @brief   Disk-usage card with a circular gauge (CCIMX DiskUsageCardWidget style).
 *
 * Dark-gradient card holding a GaugeWidget (animated needle) plus a multi-line
 * detail label. Polls a probe callable off the UI thread (same async pattern as
 * SystemUsageCard) so the filesystem stat never blocks the desktop.
 *
 * @author  CFDesktop Team
 * @date    2026-07-08
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup home_page
 */

#pragma once

#include "system_usage_card.h" // UsageSample + Probe alias

#include <QWidget>

class QLabel;

namespace cf::desktop::desktop_component {

class GaugeWidget;

/**
 * @brief  Disk-usage card rendered with a circular gauge.
 *
 * @note   Polls the probe off the UI thread (QtConcurrent); theme-less
 *         (hardcoded QSS, CCIMX style).
 * @warning None
 * @since  0.19.0
 * @ingroup home_page
 */
class DiskGaugeCard : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief  Constructs the disk gauge card.
     * @param[in]  title        Card title (e.g. "Disk").
     * @param[in]  probe        Callable returning the current UsageSample.
     * @param[in]  interval_ms  Poll interval in milliseconds.
     * @param[in]  parent       Owning Qt parent widget.
     * @throws     None
     * @note       Polls once at construction, then every interval_ms.
     * @warning    None
     * @since      0.19.0
     * @ingroup    home_page
     */
    DiskGaugeCard(const QString& title, SystemUsageCard::Probe probe, int interval_ms,
                  QWidget* parent = nullptr);

  private:
    /// @brief Kicks the probe off the UI thread; skips when a probe is in flight.
    void refresh();

    /// @brief Applies a fetched sample to the gauge + detail label (UI thread only).
    void applySample(const UsageSample& sample);

    QLabel* title_label_;                        ///< Card title.
    QLabel* detail_label_;                       ///< Multi-line usage detail.
    GaugeWidget* gauge_;                         ///< Animated percentage gauge.
    SystemUsageCard::Probe probe_;               ///< Sample source.
    QFutureWatcher<UsageSample>* probe_watcher_; ///< Marshals results back to the UI thread.
    bool in_flight_{false}; ///< True while a probe is pending (UI thread only).
};

} // namespace cf::desktop::desktop_component
