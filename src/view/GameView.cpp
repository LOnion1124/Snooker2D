#include "GameView.h"
#include "../common/Constants.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QWheelEvent>
#include <QPainterPath>

#include <array>
#include <cmath>

namespace Snooker2D {

namespace {

constexpr double AIMING_GUIDE_LENGTH = 200.0;
constexpr double COLLISION_GUIDE_LENGTH = 160.0;

double pointLength(const QPointF& point) {
    return std::hypot(point.x(), point.y());
}

double dotProduct(const QPointF& a, const QPointF& b) {
    return a.x() * b.x() + a.y() * b.y();
}

QPointF normalizedPoint(const QPointF& point) {
    const double length = pointLength(point);
    if (length <= 0.0001) return QPointF();
    return QPointF(point.x() / length, point.y() / length);
}

} // namespace

GameView::GameView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_shotAnimationTimer = new QTimer(this);
    m_shotAnimationTimer->setInterval(16);
    connect(m_shotAnimationTimer, &QTimer::timeout,
            this, &GameView::updateShotAnimation);
}

void GameView::applyTableState(const TableViewState& state) {
    m_cachedBalls = state.balls;
    m_cachedCueAngle = state.cueAngle;
    m_cachedCuePower = state.cuePower;
    m_cachedCanAim = state.canAim;
    m_cachedCanShoot = state.canShoot;
    m_cachedIsPlacingWhiteBall = state.isPlacingWhiteBall;
    m_cachedIsSimulating = state.isSimulating;
    m_centeredCoordinates = state.centeredCoordinates;

    if (!m_isShotAnimating && !m_hideAimingTools) {
        m_shotAnimationGap = cueGap();
    }

    if (!state.isSimulating && state.canAim && m_hideAimingTools) {
        m_hideAimingTools = false;
    }

    update();
}

void GameView::cancelShotAnimation() {
    m_shotAnimationTimer->stop();
    m_isShotAnimating = false;
    m_hideAimingTools = false;
    m_shotAnimationGap = cueGap();
    update();
}

void GameView::setAimingGuideEnabled(bool enabled) {
    if (m_aimingGuideEnabled == enabled) return;
    m_aimingGuideEnabled = enabled;
    update();
}

void GameView::playShotAnimation() {
    if (m_isShotAnimating || !aimingToolsVisible()) return;
    m_isShotAnimating = true;
    m_hideAimingTools = false;
    m_shotAnimationGap = cueGap();
    m_shotAnimationTimer->start();
    update();
}

void GameView::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(40, 40, 40));
    drawTable(painter);
    drawWhiteBallPlacementGuide(painter);
    drawPockets(painter);
    drawBalls(painter);
    drawAimingGuide(painter);
    drawCue(painter);
}

void GameView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    double aspect = 2.0;
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
    if (m_cachedIsPlacingWhiteBall && event->button() == Qt::LeftButton) {
        tryPlaceWhiteBall(event->position());
        event->accept();
        return;
    }

    if (!m_isShotAnimating && aimingToolsVisible() && event->button() == Qt::LeftButton) {
        updateCueAngleFromMouse(event->position());
        playShotAnimation();
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent* event) {
    if (!m_isShotAnimating && aimingToolsVisible()) {
        updateCueAngleFromMouse(event->position());
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void GameView::wheelEvent(QWheelEvent* event) {
    if (m_isShotAnimating || !aimingToolsVisible()) {
        QWidget::wheelEvent(event);
        return;
    }

    const int wheelSteps = event->angleDelta().y() / 120;
    if (wheelSteps == 0) { QWidget::wheelEvent(event); return; }

    const double newPower = m_cachedCuePower - wheelSteps * 5.0;
    emit powerChanged(newPower);
    event->accept();
}

// 绘制辅助函数

void GameView::drawTable(QPainter& painter) {
    painter.setBrush(QColor(34, 139, 34));
    painter.setPen(Qt::NoPen);
    painter.drawRect(m_tableRect);

    painter.setPen(QPen(QColor(101, 67, 33), 6));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m_tableRect.adjusted(-3, -3, 3, 3));
}

void GameView::drawWhiteBallPlacementGuide(QPainter& painter) {
    if (!m_cachedIsPlacingWhiteBall || m_tableRect.isEmpty()) return;

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double scaleY = m_tableRect.height() / TABLE_HEIGHT;
    const QPointF dCenter = gameToPixel(BAULK_LINE_X, 0.0);
    const double radiusX = D_RADIUS * scaleX;
    const double radiusY = D_RADIUS * scaleY;
    const QRectF dRect(dCenter.x() - radiusX, dCenter.y() - radiusY,
                       radiusX * 2.0, radiusY * 2.0);

    QPainterPath dZonePath;
    dZonePath.moveTo(dCenter.x(), dCenter.y() - radiusY);
    dZonePath.arcTo(dRect, 90.0, 180.0);
    dZonePath.lineTo(dCenter.x(), dCenter.y() + radiusY);
    dZonePath.closeSubpath();

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 255, 0, 45));
    painter.drawPath(dZonePath);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(180, 255, 180, 180), 2.0));
    painter.drawPath(dZonePath);
    painter.drawLine(QPointF(dCenter.x(), m_tableRect.top()),
                     QPointF(dCenter.x(), m_tableRect.bottom()));
    painter.restore();
}

void GameView::drawPockets(QPainter& painter) {
    if (m_tableRect.isEmpty()) return;

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double scaleY = m_tableRect.height() / TABLE_HEIGHT;
    const double pocketRadius = POCKET_RADIUS * scaleX;
    const auto tableToPixel = [this, scaleX, scaleY](double tableX, double tableY) {
        return QPointF(m_tableRect.left() + tableX * scaleX,
                       m_tableRect.top() + tableY * scaleY);
    };
    const std::array<QPointF, 6> pocketPositions = {
        tableToPixel(0.0, 0.0), tableToPixel(TABLE_WIDTH, 0.0),
        tableToPixel(0.0, TABLE_HEIGHT), tableToPixel(TABLE_WIDTH, TABLE_HEIGHT),
        tableToPixel(TABLE_WIDTH / 2.0, 0.0), tableToPixel(TABLE_WIDTH / 2.0, TABLE_HEIGHT)
    };

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0));
    for (const QPointF& center : pocketPositions)
        painter.drawEllipse(center, pocketRadius, pocketRadius);
    painter.restore();
}

void GameView::drawBalls(QPainter& painter) {
    if (m_tableRect.isEmpty()) return;

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double ballRadius = BALL_RADIUS * scaleX;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (const BallViewState& ball : m_cachedBalls) {
        if (!ball.onTable) continue;

        const QPointF center = gameToPixel(ball.x, ball.y);
        const QColor fillColor = ballColor(ball.type);
        const QColor penColor = ball.type == 0 ? QColor(130, 130, 130)
            : (ball.type == 7 ? QColor(240, 240, 240) : fillColor.darker(140));

        painter.setPen(QPen(penColor, 1.5));
        painter.setBrush(fillColor);
        painter.drawEllipse(center, ballRadius, ballRadius);

        const QPointF highlightCenter(center.x() - ballRadius * 0.35,
                                      center.y() - ballRadius * 0.35);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 120));
        painter.drawEllipse(highlightCenter, ballRadius * 0.35, ballRadius * 0.35);
    }
    painter.restore();
}

void GameView::drawAimingGuide(QPainter& painter) {
    if (!m_aimingGuideEnabled || !aimingToolsVisible()) return;
    QPointF startPx;
    if (!cueBallPixelPosition(&startPx)) return;
    if (m_tableRect.isEmpty()) return;

    const double rad = m_cachedCueAngle * 3.14159265358979323846 / 180.0;
    const QPointF direction(std::cos(rad), -std::sin(rad));
    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double ballRadius = BALL_RADIUS * scaleX;
    const AimingCollision collision =
        findAimingCollision(startPx, direction, AIMING_GUIDE_LENGTH, ballRadius);
    const CushionCollision cushion =
        findCushionCollision(startPx, direction, AIMING_GUIDE_LENGTH, ballRadius);
    const double ballCollisionDistance = collision.hasHit
        ? pointLength(collision.cueCenter - startPx)
        : AIMING_GUIDE_LENGTH + 1.0;
    const bool drawBallCollision = collision.hasHit
        && (!cushion.hasHit || ballCollisionDistance <= cushion.distance);
    const bool drawCushionCollision =
        cushion.hasHit && (!drawBallCollision
            || cushion.distance < ballCollisionDistance);

    painter.save();
    QPen guidePen(QColor(255, 255, 255, 120), 2, Qt::DashLine);
    guidePen.setCapStyle(Qt::RoundCap);
    painter.setPen(guidePen);

    if (drawBallCollision) {
        painter.drawLine(startPx, collision.cueCenter);
        painter.drawEllipse(collision.cueCenter, ballRadius, ballRadius);

        drawCushionAwareGuideLine(painter, collision.objectCenter,
                                  collision.objectDirection, COLLISION_GUIDE_LENGTH,
                                  ballRadius);

        if (collision.hasCueRebound) {
            drawCushionAwareGuideLine(painter, collision.cueCenter,
                                      collision.cueReboundDirection,
                                      COLLISION_GUIDE_LENGTH, ballRadius);
        }
    } else if (drawCushionCollision) {
        drawCushionAwareGuideLine(painter, startPx, direction,
                                  AIMING_GUIDE_LENGTH, ballRadius);
    } else {
        painter.drawLine(startPx, startPx + direction * AIMING_GUIDE_LENGTH);
    }

    painter.restore();
}

void GameView::drawCue(QPainter& painter) {
    if (!aimingToolsVisible() || m_tableRect.isEmpty()) return;

    QPointF cueBallPosition;
    if (!cueBallPixelPosition(&cueBallPosition)) return;

    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double ballRadius = BALL_RADIUS * scaleX;
    const double cueLength = qMax(140.0, m_tableRect.width() * 0.28);
    const double cueWidth = qMax(4.0, ballRadius * 0.45);
    const double gap = m_isShotAnimating ? m_shotAnimationGap : cueGap();

    painter.save();
    painter.translate(cueBallPosition);
    painter.rotate(-m_cachedCueAngle);

    const QRectF shaftRect(-(ballRadius + gap + cueLength), -cueWidth / 2.0,
                           cueLength, cueWidth);
    QLinearGradient shaftGradient(shaftRect.left(), 0.0, shaftRect.right(), 0.0);
    shaftGradient.setColorAt(0.0, QColor(92, 53, 24));
    shaftGradient.setColorAt(0.65, QColor(158, 100, 48));
    shaftGradient.setColorAt(1.0, QColor(218, 184, 120));

    painter.setPen(QPen(QColor(70, 40, 18), 1.0));
    painter.setBrush(shaftGradient);
    painter.drawRect(shaftRect);

    const QRectF tipRect(-(ballRadius + gap + 8.0), -cueWidth / 2.0, 8.0, cueWidth);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(235, 218, 166));
    painter.drawRect(tipRect);
    painter.restore();
}

void GameView::drawCushionAwareGuideLine(QPainter& painter, const QPointF& startPx,
                                         const QPointF& direction, double guideLength,
                                         double ballRadius) const {
    const CushionCollision cushion =
        findCushionCollision(startPx, direction, guideLength, ballRadius);
    const QPointF endPx = cushion.hasHit
        ? cushion.cueCenter
        : startPx + direction * guideLength;
    painter.drawLine(startPx, endPx);

    if (!cushion.hasHit) return;

    const double reboundLength = guideLength - cushion.distance;
    if (reboundLength <= 0.0) return;

    const CushionCollision nextCushion =
        findCushionCollision(cushion.cueCenter, cushion.reboundDirection,
                             reboundLength, ballRadius);
    const QPointF reboundEnd = nextCushion.hasHit
        ? nextCushion.cueCenter
        : cushion.cueCenter + cushion.reboundDirection * reboundLength;
    painter.drawLine(cushion.cueCenter, reboundEnd);
}

GameView::AimingCollision GameView::findAimingCollision(
        const QPointF& startPx,
        const QPointF& direction,
        double guideLength,
        double ballRadius) const {
    AimingCollision result;
    const double contactDistance = ballRadius * 2.0;
    const double contactDistanceSquared = contactDistance * contactDistance;
    double nearestDistance = guideLength;

    for (const BallViewState& ball : m_cachedBalls) {
        if (!ball.onTable || ball.type == 0) continue;

        const QPointF objectCenter = gameToPixel(ball.x, ball.y);
        const QPointF offset = objectCenter - startPx;
        const double projection = dotProduct(offset, direction);
        if (projection <= 0.0) continue;

        const double offsetDistanceSquared = dotProduct(offset, offset);
        const double perpendicularSquared = offsetDistanceSquared - projection * projection;
        if (perpendicularSquared > contactDistanceSquared) continue;

        const double backDistance =
            std::sqrt(qMax(0.0, contactDistanceSquared - perpendicularSquared));
        const double hitDistance = projection - backDistance;
        if (hitDistance < 0.0 || hitDistance > nearestDistance) continue;

        const QPointF cueCenter = startPx + direction * hitDistance;
        const QPointF objectDirection = normalizedPoint(objectCenter - cueCenter);
        if (pointLength(objectDirection) <= 0.0001) continue;

        const QPointF cueTangent =
            direction - objectDirection * dotProduct(direction, objectDirection);
        const QPointF cueReboundDirection = normalizedPoint(cueTangent);

        nearestDistance = hitDistance;
        result.hasHit = true;
        result.hasCueRebound = pointLength(cueReboundDirection) > 0.0001;
        result.cueCenter = cueCenter;
        result.objectCenter = objectCenter;
        result.objectDirection = objectDirection;
        result.cueReboundDirection = cueReboundDirection;
    }

    return result;
}

GameView::CushionCollision GameView::findCushionCollision(
        const QPointF& startPx,
        const QPointF& direction,
        double guideLength,
        double ballRadius) const {
    CushionCollision result;
    const QRectF bounds = m_tableRect.adjusted(ballRadius, ballRadius, -ballRadius, -ballRadius);
    if (bounds.isEmpty()) return result;

    constexpr double EPS = 0.0001;
    double nearestDistance = guideLength + EPS;
    bool hitVertical = false;
    bool hitHorizontal = false;

    const auto considerHit = [&](double distance, bool vertical) {
        if (distance <= EPS || distance > guideLength + EPS) return;
        const QPointF point = startPx + direction * distance;
        if (point.x() < bounds.left() - EPS || point.x() > bounds.right() + EPS ||
            point.y() < bounds.top() - EPS || point.y() > bounds.bottom() + EPS) {
            return;
        }

        if (distance < nearestDistance - EPS) {
            nearestDistance = distance;
            hitVertical = vertical;
            hitHorizontal = !vertical;
        } else if (std::abs(distance - nearestDistance) <= EPS) {
            hitVertical = hitVertical || vertical;
            hitHorizontal = hitHorizontal || !vertical;
        }
    };

    if (direction.x() > EPS) {
        considerHit((bounds.right() - startPx.x()) / direction.x(), true);
    } else if (direction.x() < -EPS) {
        considerHit((bounds.left() - startPx.x()) / direction.x(), true);
    }

    if (direction.y() > EPS) {
        considerHit((bounds.bottom() - startPx.y()) / direction.y(), false);
    } else if (direction.y() < -EPS) {
        considerHit((bounds.top() - startPx.y()) / direction.y(), false);
    }

    if (nearestDistance > guideLength) return result;

    QPointF reboundDirection = direction;
    if (hitVertical) reboundDirection.setX(-reboundDirection.x());
    if (hitHorizontal) reboundDirection.setY(-reboundDirection.y());

    QPointF cueCenter = startPx + direction * nearestDistance;
    cueCenter.setX(qBound(bounds.left(), cueCenter.x(), bounds.right()));
    cueCenter.setY(qBound(bounds.top(), cueCenter.y(), bounds.bottom()));

    result.hasHit = true;
    result.cueCenter = cueCenter;
    result.reboundDirection = reboundDirection;
    result.distance = nearestDistance;
    return result;
}

QPointF GameView::gameToPixel(double gameX, double gameY) const {
    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double scaleY = m_tableRect.height() / TABLE_HEIGHT;
    const double tableX = m_centeredCoordinates ? gameX + TABLE_WIDTH / 2.0 : gameX;
    const double tableY = m_centeredCoordinates ? gameY + TABLE_HEIGHT / 2.0 : gameY;
    return QPointF(m_tableRect.left() + tableX * scaleX,
                   m_tableRect.top() + tableY * scaleY);
}

QPointF GameView::pixelToGame(const QPointF& pixelPosition) const {
    if (m_tableRect.isEmpty()) return QPointF();
    const double scaleX = m_tableRect.width() / TABLE_WIDTH;
    const double scaleY = m_tableRect.height() / TABLE_HEIGHT;
    const double tableX = (pixelPosition.x() - m_tableRect.left()) / scaleX;
    const double tableY = (pixelPosition.y() - m_tableRect.top()) / scaleY;
    const double gameX = m_centeredCoordinates ? tableX - TABLE_WIDTH / 2.0 : tableX;
    const double gameY = m_centeredCoordinates ? tableY - TABLE_HEIGHT / 2.0 : tableY;
    return QPointF(gameX, gameY);
}

QColor GameView::ballColor(int ballType) const {
    switch (ballType) {
        case 0: return QColor(255, 255, 255);
        case 1: return QColor(220, 20, 60);
        case 2: return QColor(255, 215, 0);
        case 3: return QColor(0, 128, 0);
        case 4: return QColor(139, 69, 19);
        case 5: return QColor(0, 0, 255);
        case 6: return QColor(255, 105, 180);
        case 7: return QColor(30, 30, 30);
        default: return QColor(180, 180, 180);
    }
}

bool GameView::cueBallPixelPosition(QPointF* position) const {
    if (!position) return false;
    for (const BallViewState& ball : m_cachedBalls) {
        if (ball.type == 0 && ball.onTable) {
            *position = gameToPixel(ball.x, ball.y);
            return true;
        }
    }
    return false;
}

bool GameView::aimingToolsVisible() const {
    if (m_cachedIsPlacingWhiteBall) return false;
    if (m_hideAimingTools || m_isShotAnimating) return m_isShotAnimating;
    return !m_cachedBalls.isEmpty() && !m_tableRect.isEmpty() && m_cachedCanAim;
}

bool GameView::isInWhiteBallPlacementZone(const QPointF& gamePosition) const {
    const double dx = gamePosition.x() - BAULK_LINE_X;
    const double dy = gamePosition.y();
    return gamePosition.x() <= BAULK_LINE_X
        && dx * dx + dy * dy <= D_RADIUS * D_RADIUS;
}

void GameView::tryPlaceWhiteBall(const QPointF& mousePosition) {
    if (m_tableRect.isEmpty() || !m_tableRect.contains(mousePosition)) return;
    const QPointF gamePosition = pixelToGame(mousePosition);
    if (!isInWhiteBallPlacementZone(gamePosition)) return;
    emit whiteBallPlacementRequested(gamePosition.x(), gamePosition.y());
}

double GameView::cueGap() const {
    return qBound(0.0, m_cachedCuePower, 100.0) / 100.0 * 72.0;
}

void GameView::updateCueAngleFromMouse(const QPointF& mousePosition) {
    QPointF cueBallPosition;
    if (!cueBallPixelPosition(&cueBallPosition)) return;

    const double dx = cueBallPosition.x() - mousePosition.x();
    const double dy = cueBallPosition.y() - mousePosition.y();
    if (std::hypot(dx, dy) < 1.0) return;

    double angle = std::atan2(-dy, dx) * 180.0 / 3.14159265358979323846;
    if (angle < 0.0) angle += 360.0;
    emit angleChanged(angle);
}

void GameView::updateShotAnimation() {
    if (!m_isShotAnimating) { m_shotAnimationTimer->stop(); return; }

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
