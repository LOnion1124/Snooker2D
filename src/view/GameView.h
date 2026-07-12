#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>
#include <QVariantList>

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

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void drawTable(QPainter& painter);
    void drawPockets(QPainter& painter);
    void drawBalls(QPainter& painter);
    void drawAimingGuide(QPainter& painter);

    GameViewModel* m_viewModel = nullptr;
    QVariantList m_cachedBallPositions;
    bool m_centeredCoordinates = false; // 坐标系检测（refresh 时统一算出）
    QRectF m_tableRect; // 球桌绘制区域
};

} // namespace Snooker2D

#endif // GAMEVIEW_H
