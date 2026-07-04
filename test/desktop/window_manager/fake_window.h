/**
 * @file    fake_window.h
 * @brief   Fake IWindow for WindowManager unit tests.
 *
 * FakeWindow implements the IWindow contract entirely in-memory: every query
 * returns canned data and every operation increments a counter. This lets the
 * WindowManager state-machine tests assert behavior with no platform backend
 * and no display. It inherits IWindow's Q_OBJECT meta-object (so QObject::
 * destroyed fires normally) without declaring its own Q_OBJECT.
 *
 * @author  Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @date    2026-07-05
 * @version 0.1
 * @since   0.21
 * @ingroup components
 */

#pragma once

#include "IWindow.h"

#include <QRect>
#include <QString>

namespace cf::desktop::test {

/// @brief In-memory IWindow double that records every operation.
class FakeWindow : public IWindow {
  public:
    FakeWindow(QString id, QString title, QRect geo, qint64 pid, QObject* parent = nullptr)
        : IWindow(parent), id_(std::move(id)), title_(std::move(title)), geo_(geo), pid_(pid) {}

    win_id_t windowID() const override { return id_; }
    QString title() const override { return title_; }
    QRect geometry() const override { return geo_; }
    void set_geometry(const QRect& r) override { geo_ = r; }
    void requestClose() override { ++close_calls; }
    void raise() override { ++raise_calls; }
    void minimize() override { ++minimize_calls; }
    void maximize() override { ++maximize_calls; }
    void restore() override { ++restore_calls; }
    qint64 pid() const override { return pid_; }

    int close_calls = 0;
    int raise_calls = 0;
    int minimize_calls = 0;
    int maximize_calls = 0;
    int restore_calls = 0;

  private:
    QString id_;
    QString title_;
    QRect geo_;
    qint64 pid_;
};

} // namespace cf::desktop::test
