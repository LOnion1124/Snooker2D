#include "Ball.h"

namespace Snooker2D {

Ball::Ball(BallType type, QObject* parent)
    : QObject(parent)
    , m_type(type)
{
}

int Ball::pointValue() const {
    switch (m_type) {
        case BallType::Red:     return 1;
        case BallType::Yellow:  return 2;
        case BallType::Green:   return 3;
        case BallType::Brown:   return 4;
        case BallType::Blue:    return 5;
        case BallType::Pink:    return 6;
        case BallType::Black:   return 7;
        case BallType::White:   return 0; // 白球无分值
        default:                return 0;
    }
}

void Ball::setPosition(const Vector2D& pos) {
    m_position = pos;
    emit positionChanged();
}

void Ball::setVelocity(const Vector2D& vel) {
    m_velocity = vel;
}

void Ball::setAngularVelocity(double w) {
    m_angularVelocity = w;
}

void Ball::setPocketed(bool isPocketed) {
    m_pocketed = isPocketed;
    if (isPocketed) {
        m_onTable = false;
        m_velocity = Vector2D(0.0, 0.0);
        m_angularVelocity = 0.0;
        emit pocketed();
    }
}

void Ball::setOnTable(bool onTable) {
    m_onTable = onTable;
}

void Ball::resetPosition(const Vector2D& initialPos) {
    m_position = initialPos;
    m_initialPosition = initialPos;
    m_velocity = Vector2D(0.0, 0.0);
    m_angularVelocity = 0.0;
    m_pocketed = false;
    m_onTable = true;
    emit ballReset();
}

void Ball::respot() {
    m_position = m_initialPosition;
    m_velocity = Vector2D(0.0, 0.0);
    m_angularVelocity = 0.0;
    m_pocketed = false;
    m_onTable = true;
    emit ballReset();
}

} // namespace Snooker2D
