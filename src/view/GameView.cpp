#include "GameView.h"
#include "../viewmodel/GameViewModel.h"

#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

namespace Snooker2D {

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
    }
}

void GameView::refresh() {
    if (m_viewModel) {
        m_cachedBallPositions = m_viewModel->ballPositions();
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
    // TODO: 绘制 6 个袋口
    (void)painter;
}

void GameView::drawBalls(QPainter& painter) {
    // TODO: 根据 m_cachedBallPositions 绘制所有球
    (void)painter;
}

void GameView::drawAimingGuide(QPainter& painter) {
    // TODO: 从白球出发沿当前角度画虚线瞄准线
    (void)painter;
}

} // namespace Snooker2D
