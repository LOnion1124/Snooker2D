#include "Rules.h"
#include "Ball.h"

namespace Snooker2D {

Rules::Rules(QObject* parent)
    : QObject(parent)
{
}

BallType Rules::requiredBallType(bool redsRemaining, bool isColorNominationPhase) {
    if (isColorNominationPhase) {
        // 自由球阶段：可指定任意彩球代替红球
        // TODO: 返回实际指定的彩球类型
        return BallType::Red;
    }
    if (redsRemaining) {
        return BallType::Red;
    }
    // 红球清空后，按分值升序击打彩球
    // TODO: 返回当前应击的彩球类型
    return BallType::Yellow;
}

FoulResult Rules::checkFoul(const std::vector<Ball*>& balls,
                             Ball* whiteBall,
                             BallType firstHit,
                             bool anyBallHitCushion,
                             bool whitePocketed,
                             BallType requiredType) {
    FoulResult result;

    // 白球落袋
    if (whitePocketed) {
        result.isFoul = true;
        result.type = FoulType::WhitePocketed;
        result.penaltyPoints = calculatePenalty(FoulType::WhitePocketed, firstHit);
        return result;
    }

    // 空杆（未击中任何球）
    if (firstHit == BallType::White) {
        result.isFoul = true;
        result.type = FoulType::MissedAll;
        result.penaltyPoints = calculatePenalty(FoulType::MissedAll, requiredType);
        return result;
    }

    // 首先击中错误球
    if (!isLegalFirstHit(firstHit, requiredType)) {
        result.isFoul = true;
        result.type = FoulType::WrongBallFirst;
        result.penaltyPoints = calculatePenalty(FoulType::WrongBallFirst, firstHit);
        return result;
    }

    // 无球碰库（简化判定）
    // TODO: 细化"无球碰库"的判定逻辑
    if (!anyBallHitCushion) {
        result.isFoul = true;
        result.type = FoulType::NoBallHitCushion;
        result.penaltyPoints = calculatePenalty(FoulType::NoBallHitCushion, firstHit);
        return result;
    }

    // 无犯规
    result.isFoul = false;
    result.type = FoulType::None;
    result.penaltyPoints = 0;
    return result;
}

int Rules::calculatePenalty(FoulType foulType, BallType firstHitBall) {
    // 犯规罚分 = max(4, 犯规球分值) 的最低原则
    int ballValue = 4; // 默认最低罚 4 分
    switch (firstHitBall) {
        case BallType::Red:     ballValue = 1; break;
        case BallType::Yellow:  ballValue = 2; break;
        case BallType::Green:   ballValue = 3; break;
        case BallType::Brown:   ballValue = 4; break;
        case BallType::Blue:    ballValue = 5; break;
        case BallType::Pink:    ballValue = 6; break;
        case BallType::Black:   ballValue = 7; break;
        default: break;
    }
    (void)foulType;
    return (ballValue < 4) ? 4 : ballValue;
}

bool Rules::hasValidTarget(const std::vector<Ball*>& balls, BallType requiredType) {
    for (const auto* ball : balls) {
        if (!ball->isPocketed() && ball->type() == requiredType) {
            return true;
        }
    }
    return false;
}

bool Rules::isLegalFirstHit(BallType firstHit, BallType requiredType) {
    return firstHit == requiredType;
}

} // namespace Snooker2D
