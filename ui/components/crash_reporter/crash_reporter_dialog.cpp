/**
 * @file    crash_reporter_dialog.cpp
 * @brief   Implementation of CrashReporterDialog.
 *
 * @author  CFDesktop Team
 * @date    2026-07-10
 * @version 0.19.0
 * @since   0.19.0
 * @ingroup crash_reporter
 */

#include "crash_reporter_dialog.h"

#include "seen_marker.h"

#include "components/material/cfmaterial_fade_animation.h"
#include "components/material/cfmaterial_slide_animation.h"
#include "core/theme_manager.h"
#include "core/token/material_scheme/cfmaterial_token_literals.h"
#include "ui/widget/material/widget/button/button.h"

#include <QClipboard>
#include <QDateTime>
#include <QFile>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QTextEdit>
#include <QVBoxLayout>

#include <algorithm>

namespace cf::desktop::desktop_component {

using namespace qw::core::token::literals;

namespace {
constexpr int kDialogWidth = 600;  ///< Dialog width (px).
constexpr int kDialogHeight = 440; ///< Dialog height (px).
constexpr qreal kRadius = 16.0;    ///< Corner radius (px).
constexpr int kEnterSlidePx = 24;  ///< Enter/exit slide distance (px).
} // namespace

CrashReporterDialog::CrashReporterDialog(const QString& crash_json_path, QWidget* parent)
    : QWidget(parent), crash_path_(crash_json_path) {
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);

    setupUi();
    loadCrash();
    applyTheme();
    setupAnimations();
    hide();

    try {
        connect(&qw::core::ThemeManager::instance(), &qw::core::ThemeManager::themeChanged, this,
                [this](const qw::core::ICFTheme&) { applyTheme(); });
    } catch (...) {
        // No theme registered yet; applyTheme() falls back below.
    }
}

CrashReporterDialog::~CrashReporterDialog() = default;

void CrashReporterDialog::setupUi() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(10);

    summary_label_ = new QLabel(this);
    QFont title_font = summary_label_->font();
    title_font.setBold(true);
    title_font.setPixelSize(16);
    summary_label_->setFont(title_font);
    root->addWidget(summary_label_);

    body_edit_ = new QTextEdit(this);
    body_edit_->setReadOnly(true);
    body_edit_->setAttribute(Qt::WA_TranslucentBackground, true);
    root->addWidget(body_edit_, 1);

    auto* btns = new QWidget(this);
    auto* bh = new QHBoxLayout(btns);
    bh->setContentsMargins(0, 0, 0, 0);
    using qw::widget::material::Button;
    copy_btn_ = new Button(QStringLiteral("Copy report"), Button::ButtonVariant::Tonal, btns);
    dismiss_btn_ =
        new Button(QStringLiteral("Don't show again"), Button::ButtonVariant::Filled, btns);
    bh->addWidget(copy_btn_);
    bh->addStretch(1);
    bh->addWidget(dismiss_btn_);
    root->addWidget(btns);

    connect(copy_btn_, &QPushButton::clicked, this,
            [this]() { QGuiApplication::clipboard()->setText(body_edit_->toPlainText()); });
    connect(dismiss_btn_, &QPushButton::clicked, this, [this]() {
        markCrashSeen(crash_path_);
        emit dismissed();
        hidePanel();
    });
}

void CrashReporterDialog::loadCrash() {
    QFile f(crash_path_);
    if (!f.open(QIODevice::ReadOnly)) {
        summary_label_->setText(QStringLiteral("Crash report (unreadable)"));
        body_edit_->setPlainText(QStringLiteral("Could not read ") + crash_path_);
        return;
    }
    const QJsonObject o = QJsonDocument::fromJson(f.readAll()).object();
    const QString sig = o.value("signal_name").toString("UNKNOWN");
    const qint64 ts = o.value("timestamp").toVariant().toLongLong();
    const QString when = QDateTime::fromSecsSinceEpoch(ts).toString("yyyy-MM-dd HH:mm:ss");
    summary_label_->setText(QStringLiteral("Crash: %1  at  %2").arg(sig, when));

    QString body;
    body += QStringLiteral("Stack:\n");
    const QJsonArray resolved = o.value("resolved_frames").toArray();
    if (!resolved.isEmpty()) {
        for (const auto& v : resolved) {
            const QJsonObject rf = v.toObject();
            body += QStringLiteral("  %1   (%2:%3)\n")
                        .arg(rf.value("function").toString(), rf.value("file").toString(),
                             rf.value("line").toString());
        }
    } else {
        const QJsonArray raw = o.value("raw_frames").toArray();
        for (const auto& v : raw) {
            body += QStringLiteral("  %1\n").arg(v.toString());
        }
        body += QStringLiteral("  (runtime addresses — full symbolization deferred; see docs)\n");
    }
    body += QStringLiteral("\nLast logs:\n");
    const QJsonArray logs = o.value("last_logs").toArray();
    for (const auto& v : logs) {
        body += v.toString() + '\n';
    }
    body_edit_->setPlainText(body);
}

void CrashReporterDialog::popup(const QRect& available) {
    QRect avail = available;
    if (!avail.isValid() || avail.width() <= 0 || avail.height() <= 0) {
        if (const auto* screen = QGuiApplication::primaryScreen()) {
            avail = screen->availableGeometry();
        }
    }
    const int w = kDialogWidth;
    const int h = kDialogHeight;
    setGeometry(avail.center().x() - w / 2, avail.center().y() - h / 2, w, h);

    enter_slide_->start();
    enter_fade_->start();
    show();
    raise();
}

void CrashReporterDialog::hidePanel() {
    if (!isVisible()) {
        hide();
        return;
    }
    exit_slide_->start();
    exit_fade_->start();
}

bool CrashReporterDialog::isShowing() const noexcept {
    return isVisible();
}

void CrashReporterDialog::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath surface;
    surface.addRoundedRect(QRectF(rect()), kRadius, kRadius);
    p.fillPath(surface, surface_color_);
}

void CrashReporterDialog::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        hidePanel();
        return;
    }
    QWidget::keyPressEvent(event);
}

void CrashReporterDialog::applyTheme() {
    try {
        auto& tm = qw::core::ThemeManager::instance();
        surface_color_ = tm.theme(tm.currentThemeName()).color_scheme().queryColor(SURFACE);
    } catch (...) {
        surface_color_ = QColor(0xF7, 0xF5, 0xF3);
    }
    update();
}

void CrashReporterDialog::setupAnimations() {
    qw::core::IMotionSpec* spec = nullptr;
    try {
        auto& tm = qw::core::ThemeManager::instance();
        spec = &tm.theme(tm.currentThemeName()).motion_spec();
    } catch (...) {
        // No theme registered yet; animations fall back to default timing.
    }

    using qw::components::material::CFMaterialFadeAnimation;
    using qw::components::material::CFMaterialSlideAnimation;
    using qw::components::material::SlideDirection;

    enter_fade_ = new CFMaterialFadeAnimation(spec, this);
    enter_fade_->setRange(0.0f, 1.0f);
    enter_fade_->setMotionToken("mediumEnter");
    enter_fade_->setTargetWidget(this);

    enter_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Up, this);
    enter_slide_->setRange(static_cast<float>(-kEnterSlidePx), 0.0f);
    enter_slide_->setMotionToken("mediumEnter");
    enter_slide_->setTargetWidget(this);

    exit_fade_ = new CFMaterialFadeAnimation(spec, this);
    exit_fade_->setRange(1.0f, 0.0f);
    exit_fade_->setMotionToken("shortExit");
    exit_fade_->setTargetWidget(this);
    connect(exit_fade_, &qw::components::ICFAbstractAnimation::finished, this,
            [this]() { hide(); });

    exit_slide_ = new CFMaterialSlideAnimation(spec, SlideDirection::Down, this);
    exit_slide_->setRange(0.0f, static_cast<float>(kEnterSlidePx));
    exit_slide_->setMotionToken("shortExit");
    exit_slide_->setTargetWidget(this);
}

} // namespace cf::desktop::desktop_component
