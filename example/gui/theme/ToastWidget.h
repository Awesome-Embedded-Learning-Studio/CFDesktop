/**
 * @file ToastWidget.h
 * @brief Toast notification widget for displaying temporary messages.
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include <QWidget>
#include <QString>

namespace cf::ui::gallery {

/**
 * @brief Toast notification widget for displaying temporary messages.
 */
class ToastWidget : public QWidget {
    Q_OBJECT
  public:
    explicit ToastWidget(const QString& message, QWidget* parent = nullptr);
    void show(int durationMs = 2000);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QString message_;
};

} // namespace cf::ui::gallery
