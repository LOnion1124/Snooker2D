#include "Player.h"

namespace Snooker2D {

Player::Player(const QString& name, QObject* parent)
    : QObject(parent)
    , m_name(name)
{
}

void Player::setName(const QString& name) {
    m_name = name;
    emit nameChanged(m_name);
}

void Player::addScore(int points) {
    m_score += points;
    m_currentBreak += points;
    emit scoreChanged(m_score);
}

void Player::resetBreak() {
    m_currentBreak = 0;
}

void Player::resetScore() {
    m_score = 0;
    m_currentBreak = 0;
    emit scoreChanged(m_score);
}

} // namespace Snooker2D
