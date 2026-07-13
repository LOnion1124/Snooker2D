#ifndef BALL_H
#define BALL_H

#include <QObject>
#include "../common/Types.h"
#include "../common/Constants.h"

namespace Snooker2D {

class Ball : public QObject {
    Q_OBJECT

public:
    explicit Ball(BallType type, QObject* parent = nullptr);
    ~Ball() override = default;

    // 属性访问
    BallType type() const { return m_type; }
    Vector2D position() const { return m_position; }
    Vector2D velocity() const { return m_velocity; }
    bool isPocketed() const { return m_pocketed; }
    bool isOnTable() const { return m_onTable; }
    int pointValue() const;

    // 状态修改
    void setPosition(const Vector2D& pos);
    void setVelocity(const Vector2D& vel);
    void setPocketed(bool pocketed);
    void setOnTable(bool onTable);
    void resetPosition(const Vector2D& initialPos);
    void respot();

signals:
    void positionChanged();
    void pocketed();
    void ballReset();

private:
    BallType m_type;
    Vector2D m_position;
    Vector2D m_velocity;
    bool m_pocketed = false;
    bool m_onTable = true;
    Vector2D m_initialPosition; // 摆球初始位置（用于复位彩球）
};

} // namespace Snooker2D

#endif // BALL_H
