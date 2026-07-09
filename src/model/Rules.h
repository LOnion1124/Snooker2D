#ifndef RULES_H
#define RULES_H

#include <QObject>
#include <vector>
#include "../common/Types.h"

namespace Snooker2D {

class Ball;
class Player;

// 犯规判定结果
struct FoulResult {
    bool isFoul = false;
    FoulType type = FoulType::None;
    int penaltyPoints = 0;
    QString description;
};

class Rules : public QObject {
    Q_OBJECT

public:
    explicit Rules(QObject* parent = nullptr);
    ~Rules() override = default;

    // 判定当前应击打的球种
    static BallType requiredBallType(bool redsRemaining, bool isColorNominationPhase);

    // 犯规检测
    FoulResult checkFoul(const std::vector<Ball*>& balls,
                         Ball* whiteBall,
                         BallType firstHit,
                         bool anyBallHitCushion,
                         bool whitePocketed,
                         BallType requiredType);

    // 罚分计算
    static int calculatePenalty(FoulType foulType, BallType firstHitBall);

    // 检查是否有合法目标球
    static bool hasValidTarget(const std::vector<Ball*>& balls, BallType requiredType);

private:
    // 判定首先击中是否合法
    bool isLegalFirstHit(BallType firstHit, BallType requiredType);
};

} // namespace Snooker2D

#endif // RULES_H
