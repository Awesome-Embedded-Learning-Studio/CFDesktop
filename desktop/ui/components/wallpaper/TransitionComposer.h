/**
 * @file    desktop/ui/components/wallpaper/TransitionComposer.h
 * @brief   Pure compositing helper for wallpaper transition frames.
 *
 * Produces a single QImage per animation frame by blending the outgoing
 * and incoming wallpaper images. Backend-agnostic: any renderer that
 * consumes a QImage (QPainter shell layer or RHI backend) can drive the
 * transition by querying one frame per progress tick.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-06-30
 * @version 0.1
 * @since   0.19
 * @ingroup wallpaper
 */

#pragma once
#include <QImage>
#include <QSize>

namespace cf::desktop::wallpaper {

/**
 * @brief  Wallpaper transition (switching) mode.
 *
 * Mirrors CCIMXDesktop's SwitchingMode semantics, re-expressed for a
 * QImage-based pipeline instead of QLabel double-buffering.
 *
 * @ingroup wallpaper
 */
enum class SwitchingMode {
    Fixed,    ///< No transition; wallpaper stays static.
    Gradient, ///< Cross-fade: previous fades out, current revealed beneath.
    Movement  ///< Slide: previous exits left, current enters from right.
};

/**
 * @brief  Composes a single transition frame at the given progress.
 *
 * Both @p prev and @p cur are assumed to already be scaled to @p target
 * by the caller (the shell layer strategy). This helper only blends them.
 *
 * - @ref SwitchingMode::Fixed returns @p cur unchanged (defensive; the
 *   engine never requests a transition in Fixed mode).
 * - @ref SwitchingMode::Gradient draws @p cur fully, then @p prev with
 *   opacity @c (1-t), so the outgoing image fades out.
 * - @ref SwitchingMode::Movement slides @p prev from x=0 to x=-W and
 *   @p cur from x=W to x=0 (W = target width): old exits left, new
 *   enters from right.
 *
 * @param[in] prev    The outgoing frame (already scaled to @p target).
 * @param[in] cur     The incoming frame (already scaled to @p target).
 * @param[in] t       Transition progress; clamped to [0, 1].
 * @param[in] mode    Compositing mode.
 * @param[in] target  Output frame size.
 *
 * @return            Composited frame, or a null QImage if @p target is
 *                    empty or @p cur is null.
 *
 * @throws            None.
 *
 * @note              None.
 * @warning           None.
 * @since             0.19
 * @ingroup           wallpaper
 */
QImage composeTransitionFrame(const QImage& prev, const QImage& cur, qreal t, SwitchingMode mode,
                              const QSize& target);

/**
 * @brief  Composes one transition frame in-place into @p dst.
 *
 * Zero-allocation path for animation: reuses the caller's buffer instead of
 * allocating a new QImage each frame. @p dst, @p prev, and @p cur must share
 * the same size; prev+cur fully cover @p dst for every mode, so no clear/fill
 * is performed.
 *
 * @param[in,out] dst  Target-sized frame buffer to compose into.
 * @param[in] prev     Outgoing frame (same size as @p dst).
 * @param[in] cur      Incoming frame (same size as @p dst).
 * @param[in] t        Progress in [0, 1] (clamped).
 * @param[in] mode     Compositing mode.
 *
 * @throws             None.
 *
 * @note               None.
 * @warning            None.
 * @since              0.19
 * @ingroup            wallpaper
 */
void composeTransitionFrameInto(QImage& dst, const QImage& prev, const QImage& cur, qreal t,
                                SwitchingMode mode);

} // namespace cf::desktop::wallpaper
