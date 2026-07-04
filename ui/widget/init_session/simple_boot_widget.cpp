/**
 * @file    simple_boot_widget.cpp
 * @brief   Simple Boot Widget implementation for CFDesktop startup screen.
 *
 * @author  CFDesktop Team
 * @date    2025-03-25
 * @version 0.1
 * @since   0.1
 * @ingroup desktop_ui_widget_init_session
 */

#include "simple_boot_widget.h"
#include "autogen/metainfo/project_info.h"
#include "meta_adapts.h"
#include "step_dot_widget.h"
#include "ui_simple_boot_widget.h"
#include <QColor>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QTimer>
#include <QtCore/QtGlobal>

struct SimpleBootWidgetResourceInitializer {
    SimpleBootWidgetResourceInitializer() { Q_INIT_RESOURCE(resources); }
};
static SimpleBootWidgetResourceInitializer s_resourceInitializer;

namespace cf::desktop::init_session {

// Constants for step indicator
namespace {
constexpr int STEP_DOT_SIZE = 10;                ///< Diameter of step dots in pixels
constexpr int STEP_DOT_SPACING = 60;             ///< Spacing between step dots
constexpr int STEP_CONNECTOR_HEIGHT = 2;         ///< Height of connector lines
constexpr int ANIMATION_INTERVAL = 50;           ///< Background animation interval in ms
constexpr int PROGRESS_ANIMATION_DURATION = 300; ///< Progress bar animation duration in ms

// Color constants for light theme
inline const QColor STEP_INACTIVE_COLOR = 0xFFE0E0E0;  ///< Light gray for inactive steps
inline const QColor STEP_ACTIVE_COLOR = 0xFF6751A4;    ///< Brand color for active step
inline const QColor STEP_COMPLETED_COLOR = 0xFF6751A4; ///< Brand color for completed steps
inline const QColor CONNECTOR_INACTIVE = 0xFFE8E2E8;   ///< Light gray for inactive connectors
inline const QColor CONNECTOR_ACTIVE = 0xFF6751A4;     ///< Brand color for active connectors
} // namespace

// ============================================================================
// SimpleBootWidget Implementation
// ============================================================================

SimpleBootWidget::SimpleBootWidget(QWidget* parent)
    : BootProgressWidget(parent), ui(new Ui::SimpleBootWidget()), m_currentStepIndex(0),
      m_animationTimer(nullptr), m_animationProgress(0.0f), m_backgroundAnimationEnabled(true) {

    setup_ui();
    load_logo();

    ui->versionLabel->setText(autogen::metainfo::project_version);
    ui->copyrightLabel->setText(autogen::metainfo::project_copyright);
    ui->appNameLabel->setText(autogen::metainfo::project_name);

    // Start background animation timer
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, [this]() {
        m_animationProgress += 0.01f;
        if (m_animationProgress > 1.0f) {
            m_animationProgress = 0.0f;
        }
        update(); // Trigger repaint for animation
    });
    m_animationTimer->start(ANIMATION_INTERVAL);
}

SimpleBootWidget::~SimpleBootWidget() {
    if (m_animationTimer) {
        m_animationTimer->stop();
    }
    delete ui;
}

void SimpleBootWidget::set_slogan(const QString& slogan) {
    if (ui && ui->sloganLabel) {
        ui->sloganLabel->setText(slogan);
    }
}

void SimpleBootWidget::set_background_animation_enabled(bool enabled) {
    m_backgroundAnimationEnabled = enabled;
    if (m_animationTimer) {
        if (enabled) {
            m_animationTimer->start();
        } else {
            m_animationTimer->stop();
        }
    }
    update();
}

// ----------------------------------------------------------------------------
// Protected Methods
// ----------------------------------------------------------------------------

void SimpleBootWidget::stage_start(const BootStageInfo& bs_info) {
    // Lazy initialization: populate m_steps on first call
    if (m_steps.isEmpty() && stage_to_ > 0) {
        m_steps.clear();
        for (size_t i = 0; i < stage_to_; ++i) {
            m_steps.append(StepInfo(QString("Stage %1").arg(i + 1)));
        }
        create_step_indicator();
    }

    // Boundary check
    if (bs_info.stage_index < 0 || bs_info.stage_index >= m_steps.size()) {
        return;
    }

    // Mark previous steps as completed
    for (int i = 0; i < bs_info.stage_index; ++i) {
        m_steps[i].completed = true;
        m_steps[i].active = false;
    }

    // Mark current step as active
    m_steps[bs_info.stage_index].active = true;
    m_steps[bs_info.stage_index].name = bs_info.info;

    // Update current step index
    m_currentStepIndex = bs_info.stage_index;

    // Update UI
    update_step_visuals();
    ui->currentStepLabel->setText(bs_info.info);

    // Calculate and update progress percentage
    int progress = static_cast<int>((bs_info.stage_index + 1) * 100 / static_cast<int>(stage_to_));
    int clampedProgress = qBound(0, progress, 100);
    ui->percentageLabel->setText(QString("%1%").arg(clampedProgress));
    ui->progressBar->setValue(clampedProgress);
}

// ----------------------------------------------------------------------------
// Event Handlers
// ----------------------------------------------------------------------------

void SimpleBootWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // === Draw connector lines between step dots ===
    // This ensures lines connect dot centers perfectly, no layout misalignment
    if (ui && ui->stepDotsContainer) {
        QList<StepDotWidget*> dots = ui->stepDotsContainer->findChildren<StepDotWidget*>();

        if (dots.size() >= 2) {
            for (int i = 0; i < dots.size() - 1; ++i) {
                // Get dot centers in widget coordinates
                QPoint p1 = dots[i]->mapTo(this, dots[i]->rect().center());
                QPoint p2 = dots[i + 1]->mapTo(this, dots[i + 1]->rect().center());

                // Connector is active if this segment is completed or current step is past it
                bool isActive = (i < m_currentStepIndex);
                QColor lineColor = isActive ? QColor(CONNECTOR_ACTIVE) : QColor(CONNECTOR_INACTIVE);

                // Draw the connector line
                QPen pen(lineColor, STEP_CONNECTOR_HEIGHT);
                pen.setCapStyle(Qt::RoundCap);
                painter.setPen(pen);
                painter.drawLine(p1, p2);
            }
        }
    }

    // === Draw animated background ===
    if (!m_backgroundAnimationEnabled) {
        return;
    }

    // Draw subtle animated gradient background
    QRectF bgRect = rect();

    // Create a subtle gradient that shifts with animation progress
    QColor color1(245, 245, 245); // #F5F5F5
    QColor color2(250, 250, 250); // #FAFAFA
    QColor color3(240, 240, 240); // #F0F0F0

    // Interpolate colors based on animation progress
    float shift = std::sin(m_animationProgress * 6.28318f) * 0.5f + 0.5f;

    QLinearGradient gradient(
        bgRect.topLeft() + QPointF(bgRect.width() * shift * 0.1f, bgRect.height() * shift * 0.1f),
        bgRect.bottomRight());
    gradient.setColorAt(0.0, color1);
    gradient.setColorAt(0.5 + shift * 0.1f, color2);
    gradient.setColorAt(1.0, color3);

    painter.fillRect(bgRect, gradient);

    // Draw logo watermark (very subtle, in the background)
    if (!m_logoPixmap.isNull()) {
        painter.setOpacity(0.03); // Very subtle watermark
        QPixmap watermark = m_logoPixmap.scaled(width() / 2, height() / 2, Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);

        // Center the watermark
        QPointF watermarkPos((width() - watermark.width()) / 2.0,
                             (height() - watermark.height()) / 2.0);
        painter.drawPixmap(watermarkPos, watermark);
        painter.setOpacity(1.0);
    }
}

// ----------------------------------------------------------------------------
// Private Methods
// ----------------------------------------------------------------------------

void SimpleBootWidget::setup_ui() {
    ui->setupUi(this);

    // Set window flags for splash-like appearance
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);

    // Enable automatic show/hide for empty content
    if (ui->currentStepLabel) {
        ui->currentStepLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    }
}

void SimpleBootWidget::create_step_indicator() {
    if (!ui || !ui->stepDotsContainer || !ui->stepLabelsContainer) {
        return;
    }

    // Clear existing widgets from both containers
    qDeleteAll(
        ui->stepDotsContainer->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
    qDeleteAll(
        ui->stepLabelsContainer->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));

    // Delete existing layouts
    if (auto* oldLayout = ui->stepDotsContainer->layout()) {
        delete oldLayout;
    }
    if (auto* oldLayout = ui->stepLabelsContainer->layout()) {
        delete oldLayout;
    }

    // === SIMPLIFIED: Use same spacing for both layouts ===
    // Connector lines are now drawn in paintEvent, not as widgets
    auto* dotsLayout = new QHBoxLayout(ui->stepDotsContainer);
    dotsLayout->setSpacing(STEP_DOT_SPACING);
    dotsLayout->setContentsMargins(0, 0, 0, 0);
    dotsLayout->setAlignment(Qt::AlignCenter);

    auto* labelsLayout = new QHBoxLayout(ui->stepLabelsContainer);
    labelsLayout->setSpacing(STEP_DOT_SPACING);
    labelsLayout->setContentsMargins(0, 0, 0, 0);
    labelsLayout->setAlignment(Qt::AlignCenter);

    // Create step dots and labels - one-to-one mapping
    for (int i = 0; i < m_steps.size(); ++i) {
        // Step dot widget
        auto* dotWidget = new StepDotWidget(i, m_steps[i].name, ui->stepDotsContainer);
        dotsLayout->addWidget(dotWidget);

        // Step label widget (mirrors dot position)
        auto* label = new QLabel(m_steps[i].name, ui->stepLabelsContainer);
        label->setAlignment(Qt::AlignCenter);
        label->setProperty("stepIndex", i);
        label->setProperty("class", "stepLabel");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        labelsLayout->addWidget(label);
    }

    update_step_visuals();
}

void SimpleBootWidget::update_step_visuals() {
    if (!ui) {
        return;
    }

    // Update step dots
    QList<StepDotWidget*> dots = ui->stepDotsContainer->findChildren<StepDotWidget*>();
    for (auto* dot : dots) {
        if (dot) {
            int index = dot->stepIndex();
            if (index >= 0 && index < m_steps.size()) {
                const StepInfo& step = m_steps[index];
                dot->setState(step.completed ? StepDotWidget::Completed
                              : step.active  ? StepDotWidget::Active
                                             : StepDotWidget::Inactive);
            }
        }
    }

    // Update labels
    QList<QLabel*> labels = ui->stepLabelsContainer->findChildren<QLabel*>();
    for (auto* label : labels) {
        if (label && label->property("stepIndex").isValid()) {
            int index = label->property("stepIndex").toInt();
            if (index >= 0 && index < m_steps.size()) {
                const StepInfo& step = m_steps[index];
                QString styleClass = "stepLabel";

                if (step.active) {
                    styleClass += " active";
                    label->setStyleSheet("color: #6751A4; font-weight: bold;");
                } else if (step.completed) {
                    styleClass += " completed";
                    label->setStyleSheet("color: #6751A4;");
                } else {
                    label->setStyleSheet("color: #999999;");
                }
            }
        }
    }

    // Trigger repaint to update connector lines (drawn in paintEvent)
    update();
}

void SimpleBootWidget::load_logo() {
    QPixmap logo(":/early_gui_welcome/CFDesktop.logo.png");

    if (!logo.isNull()) {
        m_logoPixmap = logo;

        ui->logoLabel->setScaledContents(true);
        ui->logoLabel->setAlignment(Qt::AlignCenter);
        ui->logoLabel->setPixmap(m_logoPixmap);
    }
}

} // namespace cf::desktop::init_session
