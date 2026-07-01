/**
 * @file    apps/system_state/system_state_panel.h
 * @brief   System State main panel (standalone app).
 *
 * Root widget of the standalone System State executable. Surveys live system
 * telemetry (CPU model/cores/frequency/usage/temperature, physical and swap
 * memory) and renders it as a scrollable Material card with QuarkWidgets MD3
 * buttons (refresh/pause). Ported from CCIMXDesktop's SystemState.
 *
 * Unlike the CCIMX original (which bundled its own platform-specific CPU and
 * memory fetchers plus QtCharts), this port reuses the mature base/ probes
 * (cf::getCPUInfo, cf::getCPUBonusInfo, cf::getCPUProfileInfo,
 * cf::getSystemMemoryInfo) and replaces the charts with plain text readouts,
 * so it depends only on cfbase + QuarkWidgets + Qt Widgets.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-01
 * @version 0.1
 * @since   0.20
 * @ingroup system_state
 */

#pragma once

#include <QWidget>

class QLabel;
class QPaintEvent;
class QTimer;

namespace cf::desktop::desktop_component {

/**
 * @brief  Root widget of the standalone System State application.
 *
 * @ingroup system_state
 */
class SystemStatePanel final : public QWidget {
    Q_OBJECT
  public:
    /**
     * @brief   Constructs the System State panel.
     *
     * @param[in] parent  Parent widget (nullptr for a top-level window).
     *
     * @throws  None
     * @since   0.20
     * @ingroup system_state
     */
    explicit SystemStatePanel(QWidget* parent = nullptr);

    /**
     * @brief   Destructs the panel.
     *
     * @throws  None
     * @since   0.20
     * @ingroup system_state
     */
    ~SystemStatePanel() override;

  protected:
    /**
     * @brief   Paints the rounded Material card background.
     *
     * @param[in] event  The paint event descriptor.
     *
     * @throws  None
     * @since   0.20
     * @ingroup system_state
     */
    void paintEvent(QPaintEvent* event) override;

  private slots:
    /// @brief Performs a one-shot refresh of all telemetry readouts.
    void refreshNow();
    /// @brief Toggles the periodic auto-refresh timer on or off.
    void toggleAutoRefresh(bool checked);

  private:
    /// @brief Builds the toolbar + readout layout.
    void setupUi();
    /// @brief Queries the base/ probes once and fills the static labels.
    void loadStaticInfo();
    /// @brief Queries the base/ probes and updates the live labels.
    void loadLiveInfo();

    QTimer* refresh_timer_{nullptr}; ///< Periodic auto-refresh timer.

    // Static (cached) CPU info labels.
    QLabel* cpu_model_label_{nullptr};        ///< CPU model name.
    QLabel* cpu_arch_label_{nullptr};         ///< CPU architecture.
    QLabel* cpu_manufacturer_label_{nullptr}; ///< CPU manufacturer.

    // Live CPU labels.
    QLabel* cpu_cores_label_{nullptr}; ///< Logical/physical core counts.
    QLabel* cpu_freq_label_{nullptr};  ///< Current/max frequency (MHz).
    QLabel* cpu_usage_label_{nullptr}; ///< Current CPU usage (%).
    QLabel* cpu_temp_label_{nullptr};  ///< CPU temperature (C).

    // Live memory labels.
    QLabel* mem_phys_total_label_{nullptr}; ///< Total physical memory.
    QLabel* mem_phys_used_label_{nullptr};  ///< Used physical memory + %.
    QLabel* mem_phys_avail_label_{nullptr}; ///< Available physical memory.
    QLabel* mem_swap_total_label_{nullptr}; ///< Total swap memory.
    QLabel* mem_swap_used_label_{nullptr};  ///< Used swap memory + %.
};

} // namespace cf::desktop::desktop_component
