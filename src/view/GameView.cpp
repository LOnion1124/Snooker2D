#include "GameView.h"
#include "../viewmodel/GameViewModel.h"
#include "../common/Constants.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QVariantMap>

#include <array>
#include <cmath>

namespace Snooker2D {

namespace {

QPointF gameToPixel(const QRectF& tableRect, double gameX, double gameY, bool centeredCoordinates = false) {
    const double scaleX = tableRect.width() / TABLE_WIDTH;
    const double scaleY = tableRect.height() / TABLE_HEIGHT;
    const double tableX = centeredCoordinates ? gameX + TABLE_WIDTH / 2.0 : gameX;
    const double tableY = centeredCoordinates ? gameY + TABLE_HEIGHT / 2.0 : gameY;

    return QPointF(tableRect.left() + tableX * scaleX,
                   tableRect.top() + tableY * scaleY);
}

QColor ballColor(int ballType) {
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

} // namespace

GameView::GameView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GameView::setViewModel(GameViewModel* viewModel) {
    if (m_viewModel) {
        disconnect(m_viewModel, nullptr, this, nullptr);
    }
    m_viewModel = viewModel;
    if (m_viewModel) {
        connect(m_viewModel, &GameViewModel::ballPositionsChanged,
                this, &GameView::refresh);
        // 瞄准线随角度/力度实时更新
        connect(m_viewModel, &GameViewModel::cueAngleChanged,
                this, [this]() { update(); });
        connect(m_viewModel, &GameViewModel::cuePowerChanged,
                this, [this]() { update(); });
        refresh();
    }
}

void GameView::refresh() {
    if (m_viewModel) {
        m_cachedBallPositions = m_viewModel->ballPositions();

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

void GameView::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    painter.fillRect(rect(), QColor(40, 40, 40));

    // 绘制顺序：台面 → 袋口 → 球 → 瞄准线
    drawTable(painter);
    drawPockets(painter);
    drawBalls(painter);
    drawAimingGuide(painter);
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
    const double pocketRadius = POCKET_RADIUS * scaleX;
    const std::array<QPointF, 6> pocketPositions = {
        gameToPixel(m_tableRect, 0.0, 0.0),
        gameToPixel(m_tableRect, TABLE_WIDTH, 0.0),
        gameToPixel(m_tableRect, 0.0, TABLE_HEIGHT),
        gameToPixel(m_tableRect, TABLE_WIDTH, TABLE_HEIGHT),
        gameToPixel(m_tableRect, TABLE_WIDTH / 2.0, 0.0),
        gameToPixel(m_tableRect, TABLE_WIDTH / 2.0, TABLE_HEIGHT)
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
        const QPointF center = gameToPixel(m_tableRect, x, y, m_centeredCoordinates);
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
    if (!m_viewModel || m_cachedBallPositions.isEmpty() || m_tableRect.isEmpty()) {
        return;
    }

    // 找到白球
    double whiteX = 0.0, whiteY = 0.0;
    bool found = false;
    for (const QVariant& item : m_cachedBallPositions) {
        const QVariantMap ballData = item.toMap();
        if (ballData.value("type").toInt() == 0 && ballData.value("onTable").toBool()) {
            whiteX = ballData.value("x").toDouble();
            whiteY = ballData.value("y").toDouble();
            found = true;
            break;
        }
    }
    if (!found) return;

    double angle = m_viewModel->cueAngle();
    double power = m_viewModel->cuePower();

    // 方向向量（与 performShot 中一致的转换）
    double rad = angle * 3.14159265358979323846 / 180.0;
    double dx = std::cos(rad);
    double dy = -std::sin(rad); // 屏幕 Y 轴向下，取反

    // 长度正比于力度，最大约一半台宽
    double length = (power / 100.0) * TABLE_WIDTH * 0.45;

    double endX = whiteX + dx * length;
    double endY = whiteY + dy * length;

    QPointF startPx = gameToPixel(m_tableRect, whiteX, whiteY, m_centeredCoordinates);
    QPointF endPx = gameToPixel(m_tableRect, endX, endY, m_centeredCoordinates);

    painter.save();

    // 虚线
    QPen linePen(QColor(255, 220, 0), 2.5, Qt::DashLine);
    painter.setPen(linePen);
    painter.drawLine(startPx, endPx);

    // 箭头三角
    double arrowSize = 12.0;
    double arrowAngle = std::atan2(endPx.y() - startPx.y(), endPx.x() - startPx.x());
    QPointF arrowP1 = endPx - QPointF(std::cos(arrowAngle - 0.45) * arrowSize,
                                        std::sin(arrowAngle - 0.45) * arrowSize);
    QPointF arrowP2 = endPx - QPointF(std::cos(arrowAngle + 0.45) * arrowSize,
                                        std::sin(arrowAngle + 0.45) * arrowSize);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 220, 0));
    QPolygonF arrowHead;
    arrowHead << endPx << arrowP1 << arrowP2;
    painter.drawPolygon(arrowHead);

    painter.restore();
}

} // namespace Snooker2D
