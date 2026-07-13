#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QColor>
#include <QPointF>
#include <QString>
#include <QWidget>
#include <QVector>

#include "contracts/GameViewState.h"

class QMouseEvent;
class QPainter;
class QTimer;
class QWheelEvent;

namespace Snooker2D {

class GameUiBus;

class GameView : public QWidget {
    Q_OBJECT

public:
    explicit GameView(QWidget* parent = nullptr);
    ~GameView() override = default;

    void bind(GameUiBus* bus);

public slots:
    void refresh();

signals:
    void shotAnimationFinished();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void drawTable(QPainter& painter);
    void drawPockets(QPainter& painter);
    void drawWhiteBallPlacementGuide(QPainter& painter);
    void drawBalls(QPainter& painter);
    void drawAimingGuide(QPainter& painter);
    void drawCue(QPainter& painter);
    QPointF gameToPixel(double gameX, double gameY) const;
    QPointF pixelToGame(const QPointF& pixelPosition) const;
    QColor ballColor(int ballType) const;
    bool cueBallPixelPosition(QPointF* position) const;
    bool aimingToolsVisible() const;
    bool isInWhiteBallPlacementZone(const QPointF& gamePosition) const;
    void tryPlaceWhiteBall(const QPointF& mousePosition);
    double cueGap() const;
    void updateCueAngleFromMouse(const QPointF& mousePosition);
    void cancelShotAnimation();
    void updateShotAnimation();
    void playShotAnimation();

    void applyTableState(const TableViewState& state);

    GameUiBus* m_bus = nullptr;
    QTimer* m_shotAnimationTimer = nullptr;

    // 缓存的最新状态
    QVector<BallViewState> m_cachedBalls;
    double m_cachedCueAngle = 0.0;
    double m_cachedCuePower = 50.0;
    bool m_cachedCanAim = false;
    bool m_cachedCanShoot = false;
    bool m_cachedIsPlacingWhiteBall = false;
    bool m_cachedIsSimulating = false;
    bool m_centeredCoordinates = false;

    // 动画状态
    bool m_isShotAnimating = false;
    bool m_hideAimingTools = false;
    double m_shotAnimationGap = 0.0;
    QRectF m_tableRect;
};

} // namespace Snooker2D

#endif // GAMEVIEW_H
