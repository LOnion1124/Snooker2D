#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QColor>
#include <QPointF>
#include <QString>
#include <QWidget>
#include <QVariantList>

class QMouseEvent;
class QPainter;
class QTimer;
class QWheelEvent;

namespace Snooker2D {

class GameViewModel;

class GameView : public QWidget {
    Q_OBJECT

public:
    explicit GameView(QWidget* parent = nullptr);
    ~GameView() override = default;

    void setViewModel(GameViewModel* viewModel);

public slots:
    void refresh();
    void playShotAnimation();

signals:
    void shotAnimationFinished();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void drawTable(QPainter& painter);
    void drawPockets(QPainter& painter);
    void drawBalls(QPainter& painter);
    void drawAimingGuide(QPainter& painter);
    void drawCue(QPainter& painter);
    QPointF gameToPixel(double gameX, double gameY) const;
    QColor ballColor(int ballType) const;
    bool cueBallPixelPosition(QPointF* position) const;
    bool aimingToolsVisible() const;
    double cueGap() const;
    void updateCueAngleFromMouse(const QPointF& mousePosition);
    void updateShotAnimation();

    GameViewModel* m_viewModel = nullptr;
    QTimer* m_shotAnimationTimer = nullptr;
    QVariantList m_cachedBallPositions;
    double m_cachedCueAngle = 0.0;
    double m_cachedCuePower = 50.0;
    QString m_cachedGamePhase;
    bool m_centeredCoordinates = false; // 坐标系检测（refresh 时统一算出）
    bool m_isDragging = false;
    bool m_isShotAnimating = false;
    bool m_hideAimingTools = false;
    double m_shotAnimationGap = 0.0;
    QPointF m_dragStartPos;
    QRectF m_tableRect; // 球桌绘制区域
};

} // namespace Snooker2D

#endif // GAMEVIEW_H
