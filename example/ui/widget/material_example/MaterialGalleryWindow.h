/**
 * @file MaterialGalleryWindow.h
 * @brief Material Design 3 Widget Gallery Main Window
 */

#pragma once

#include <QHBoxLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QStackedWidget>
#include <QWidget>

namespace cf::ui::example {

class MaterialGalleryWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MaterialGalleryWindow(QWidget* parent = nullptr);
    ~MaterialGalleryWindow() override;

    QStackedWidget* contentStack() { return contentStack_; }

  private:
    void setupUI();
    void createNavigation();
    void registerDemos();

    QWidget* centralWidget_;
    QHBoxLayout* mainLayout_;
    QListWidget* navList_;
    QStackedWidget* contentStack_;
    QSplitter* splitter_;
};

} // namespace cf::ui::example
