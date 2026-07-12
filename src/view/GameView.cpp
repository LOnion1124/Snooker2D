#include "GameView.h"
#include "../viewmodel/GameViewModel.h"
#include "../common/Constants.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QVariantMap>
#include <QWheelEvent>

#include <array>
#include <cmath>

namespace Snooker2D {

GameView::GameView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);

    m_shotAnimationTimer = new QTimer(this);
    m_shotAnimationTimer->setInterval(16);
    connect(m_shotAnimationTimer, &QTimer::timeout,
            this, &GameView::updateShotAnimation);
}

void GameView::setViewModel(GameViewModel* viewModel) {
    if (m_viewModel) {
        disconnect(m_viewModel, nullptr, this, nullptr);
    }
    m_viewModel = viewModel;
    if (m_viewModel) {
        connect(m_viewModel, &GameViewModel::ballPositionsChanged,
                this, &GameView::refresh);
        connect(m_viewModel, &GameViewModel::cueAngleChanged,
                this, &GameView::refresh);
        connect(m_viewModel, &GameViewModel::gamePhaseChanged,
                this, &GameView::refresh);
        connect(m_viewModel, &GameViewModel::cuePowerChanged,
                this, &GameView::refresh);
        refresh();
    }
}

void GameView::refresh() {
    if (m_viewModel) {
        m_cachedBallPositions = m_viewModel->ballPositions();
        m_cachedCueAngle = m_viewModel->cueAngle();
        m_cachedCuePower = m_viewModel->cuePower();
        m_cachedGamePhase = m_viewModel->gamePhase();

        const bool playablePhase = !m_cachedGamePhase.isEmpty()
            && m_cachedGamePhase != QStringLiteral("未开始")
            && m_cachedGamePhase != QStringLiteral("比赛结束")
            && !m_cachedGamePhase.contains(QStringLiteral("模拟中"));
        if (playablePhase && !m_isShotAnimating) {
            m_hideAimingTools = false;
            m_shotAnimationGap = cueGap();
        }

        // 统一检测坐标系：所有球中有任何球出现负坐标或超出 [0, TABLE_*] 即为中心坐标系
        m_centeredCoordinates = false;
        for (const QVariant& item : m_cachedBallPositions) {
            const QVariantMap ballData = item.toMap();
            const double x = ballData.value("x").toDouble();
            const double y = ballData.value("y").toDouble();
            if (x < 0.0 || y < 0.0 || x > TABLE_WIDTH || y > TABLE_HEIGHT) {
                m_centeredCoordinates = true;
                break;
            }
        }
    }
    update(); // 触发 repaint
}

void GameView::playShotAnimation() {
    if (!m_viewModel || m_isShotAnimating || !aimingToolsVisible()) {
        return;
    }

    m_isDragging = false;
    m_isShotAnimating = true;
    m_hideAimingTools = false;
    m_shotAnimationGap = cueGap();
    m_shotAnimationTimer->start();
    update();
}

void GameView::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    painter.fillRect(rect(), QColor(40, 40, 40));

    // 绘制顺序：台面 → 袋口 → 球 → 瞄准线 → 球杆
    drawTable(painter);
    drawPockets(painter);
    drawBalls(painter);
    drawAimingGuide(painter);
    drawCue(painter);
}

void GameView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    // 重新计算球桌绘制区域
    double aspect = 2.0; // 球桌宽高比 2:1
    QSize newSize = event->size();
    double viewW = newSize.width();
    double viewH = newSize.height();
    double viewAspect = viewW / viewH;

    double tableW, tableH;
    if (viewAspect > aspect) {
        tableH = viewH * 0.9;
        tableW = tableH * aspect;
    } else {
        tableW = viewW * 0.9;
        tableH = tableW / aspect;
    }

    double x = (viewW - tableW) / 2.0;
    double y = (viewH - tableH) / 2.0;
    m_tableRect = QRectF(x, y, tableW, tableH);
}

void GameView::mousePressEvent(QMouseEvent* event) {
    if (!m_isShotAnimating && aimingToolsVisible()
        && event->button() == Qt::LeftButton && m_tableRect.contains(event->position())) {
        m_isDragging = true;
        m_dragStartPos = event->position();
        updateCueAngleFromMouse(event->position());
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging && !m_isShotAnimating && aimingToolsVisible()) {
        updateCueAngleFromMouse(event->position());
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void GameView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void GameView::wheelEvent(QWheelEvent* event) {
    if (!m_viewModel || m_isShotAnimating || !aimingToolsVisible()) {
        QWidget::wheelEvent(event);
        return;
    }

    const int wheelSteps = event->angleDelta().y() / 120;
    if (wheelSteps == 0) {
        QWidget::wheelEvent(event);
        return;
    }

    m_viewModel->setPower(m_cachedCuePower - wheelSteps * 5.0);
    event->accept();
}

// ----------------------------------------------------------------------------
// 绘制辅助函数
// ----------------------------------------------------------------------------

void GameView::drawTable(QPainter& painter) {
    // 台面（绿色）
    painter.setBrush(QColor(34, 139, 34)); // ForestGreen
    painter.setPen(Qt::NoPen);
    painter.drawRect(m_tableRect);

    // 库边（棕色边框）
    painter.setPen(QPen(QColor(101, 67, 33), 6));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_tableRect.adjusted(-3, -3, 3, 3));
}

void GameView::drawPockets(QPainter& painter) {
    if (m_tableRect.isEmpty()) {
        return;
    }

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double scaleY = m_tableRect.height() / TABLE_HEIGHT;
    const double pocketRadius = POCKET_RADIUS * scaleX;
    const auto tableToPixel = [this, scaleX, scaleY](double tableX, double tableY) {
        return QPointF(m_tableRect.left() + tableX * scaleX,
                       m_tableRect.top() + tableY * scaleY);
    };
    const std::array<QPointF, 6> pocketPositions = {
        tableToPixel(0.0, 0.0),
        tableToPixel(TABLE_WIDTH, 0.0),
        tableToPixel(0.0, TABLE_HEIGHT),
        tableToPixel(TABLE_WIDTH, TABLE_HEIGHT),
        tableToPixel(TABLE_WIDTH / 2.0, 0.0),
        tableToPixel(TABLE_WIDTH / 2.0, TABLE_HEIGHT)
    };

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0));

    for (const QPointF& center : pocketPositions) {
        painter.drawEllipse(center, pocketRadius, pocketRadius);
    }

    painter.restore();
}

void GameView::drawBalls(QPainter& painter) {
    if (m_tableRect.isEmpty()) {
        return;
    }

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double ballRadius = BALL_RADIUS * scaleX;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (const QVariant& item : m_cachedBallPositions) {
        const QVariantMap ballData = item.toMap();
        if (!ballData.value("onTable").toBool()) {
            continue;
        }

        const double x = ballData.value("x").toDouble();
        const double y = ballData.value("y").toDouble();
        const int type = ballData.value("type").toInt();
        const QPointF center = gameToPixel(x, y);
        const QColor fillColor = ballColor(type);
        const QColor penColor = type == 0 ? QColor(130, 130, 130)
                                          : (type == 7 ? QColor(240, 240, 240)
                                                       : fillColor.darker(140));

        painter.setPen(QPen(penColor, 1.5));
        painter.setBrush(fillColor);
        painter.drawEllipse(center, ballRadius, ballRadius);

        const QPointF highlightCenter(center.x() - ballRadius * 0.35,
                                      center.y() - ballRadius * 0.35);
        const double highlightRadius = ballRadius * 0.35;
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 120));
        painter.drawEllipse(highlightCenter, highlightRadius, highlightRadius);
    }

    painter.restore();
}

void GameView::drawAimingGuide(QPainter& painter) {
    if (!aimingToolsVisible()) {
        return;
    }

    QPointF startPx;
    if (!cueBallPixelPosition(&startPx)) {
        return;
    }

    const double rad = m_cachedCueAngle * 3.14159265358979323846 / 180.0;
    const QPointF direction(std::cos(rad), -std::sin(rad));
    const QPointF endPx = startPx + direction * 200.0;

    painter.save();
    painter.setPen(QPen(QColor(255, 255, 255, 100), 2, Qt::DashLine));
    painter.drawLine(startPx, endPx);
    painter.restore();
}

void GameView::drawCue(QPainter& painter) {
    if (!aimingToolsVisible() || m_tableRect.isEmpty()) {
        return;
    }

    QPointF cueBallPosition;
    if (!cueBallPixelPosition(&cueBallPosition)) {
        return;
    }

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double ballRadius = BALL_RADIUS * scaleX;
    const double cueLength = qMax(140.0, m_tableRect.width() * 0.28);
    const double cueWidth = qMax(4.0, ballRadius * 0.45);
    const double gap = m_isShotAnimating ? m_shotAnimationGap : cueGap();

    painter.save();
    painter.translate(cueBallPosition);
    painter.rotate(-m_cachedCueAngle);

    const QRectF shaftRect(-(ballRadius + gap + cueLength),
                           -cueWidth / 2.0,
                           cueLength,
                           cueWidth);
    QLinearGradient shaftGradient(shaftRect.left(), 0.0, shaftRect.right(), 0.0);
    shaftGradient.setColorAt(0.0, QColor(92, 53, 24));
    shaftGradient.setColorAt(0.65, QColor(158, 100, 48));
    shaftGradient.setColorAt(1.0, QColor(218, 184, 120));

    painter.setPen(QPen(QColor(70, 40, 18), 1.0));
    painter.setBrush(shaftGradient);
    painter.drawRect(shaftRect);

    const QRectF tipRect(-(ballRadius + gap + 8.0),
                         -cueWidth / 2.0,
                         8.0,
                         cueWidth);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(235, 218, 166));
    painter.drawRect(tipRect);
    painter.restore();
}

QPointF GameView::gameToPixel(double gameX, double gameY) const {
    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double scaleY = m_tableRect.height() / TABLE_HEIGHT;
    const double tableX = m_centeredCoordinates ? gameX + TABLE_WIDTH / 2.0 : gameX;
    const double tableY = m_centeredCoordinates ? gameY + TABLE_HEIGHT / 2.0 : gameY;

    return QPointF(m_tableRect.left() + tableX * scaleX,
                   m_tableRect.top() + tableY * scaleY);
}

QColor GameView::ballColor(int ballType) const {
    switch (ballType) {
        case 0: return QColor(255, 255, 255); // White
        case 1: return QColor(220, 20, 60);   // Red
        case 2: return QColor(255, 215, 0);   // Yellow
        case 3: return QColor(0, 128, 0);     // Green
        case 4: return QColor(139, 69, 19);   // Brown
        case 5: return QColor(0, 0, 255);     // Blue
        case 6: return QColor(255, 105, 180); // Pink
        case 7: return QColor(30, 30, 30);    // Black
        default: return QColor(180, 180, 180);
    }
}

bool GameView::cueBallPixelPosition(QPointF* position) const {
    if (!position) {
        return false;
    }

    for (const QVariant& item : m_cachedBallPositions) {
        const QVariantMap ballData = item.toMap();
        if (ballData.value("type").toInt() == 0 && ballData.value("onTable").toBool()) {
            *position = gameToPixel(ballData.value("x").toDouble(),
                                    ballData.value("y").toDouble());
            return true;
        }
    }

    return false;
}

bool GameView::aimingToolsVisible() const {
    if (m_hideAimingTools || m_isShotAnimating) {
        return m_isShotAnimating;
    }

    return !m_cachedBallPositions.isEmpty()
        && !m_tableRect.isEmpty()
        && m_cachedGamePhase != QStringLiteral("未开始")
        && m_cachedGamePhase != QStringLiteral("比赛结束")
        && !m_cachedGamePhase.contains(QStringLiteral("模拟中"));
}

double GameView::cueGap() const {
    const double powerRatio = qBound(0.0, m_cachedCuePower, 100.0) / 100.0;
    return powerRatio * 72.0;
}

void GameView::updateCueAngleFromMouse(const QPointF& mousePosition) {
    if (!m_viewModel) {
        return;
    }

    QPointF cueBallPosition;
    if (!cueBallPixelPosition(&cueBallPosition)) {
        return;
    }

    const double dx = cueBallPosition.x() - mousePosition.x();
    const double dy = cueBallPosition.y() - mousePosition.y();
    if (std::hypot(dx, dy) < 1.0) {
        return;
    }

    double angle = std::atan2(-dy, dx) * 180.0 / 3.14159265358979323846;
    if (angle < 0.0) {
        angle += 360.0;
    }

    m_viewModel->setAngle(angle);
}

void GameView::updateShotAnimation() {
    if (!m_isShotAnimating) {
        m_shotAnimationTimer->stop();
        return;
    }

    m_shotAnimationGap = qMax(0.0, m_shotAnimationGap - 10.0);
    if (m_shotAnimationGap <= 0.0) {
        m_shotAnimationTimer->stop();
        m_isShotAnimating = false;
        m_hideAimingTools = true;
        update();
        emit shotAnimationFinished();
        return;
    }

    update();
}

} // namespace Snooker2D
