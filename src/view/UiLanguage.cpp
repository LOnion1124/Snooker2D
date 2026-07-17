#include "UiLanguage.h"
#include "../common/Types.h"

namespace Snooker2D {

QString translatedPhaseText(int phaseKind, bool isSimulating, UiLanguage language) {
    if (language == UiLanguage::Chinese) {
        switch (static_cast<GamePhase>(phaseKind)) {
            case GamePhase::NotStarted: return QStringLiteral("未开始");
            case GamePhase::RedBall:    return QStringLiteral("请击红球");
            case GamePhase::ColorBall:  return QStringLiteral("请击彩球");
            case GamePhase::FreeBall:   return QStringLiteral("自由球");
            case GamePhase::Foul:       return QStringLiteral("犯规");
            case GamePhase::GameOver:   return QStringLiteral("比赛结束");
        }
        return QString();
    }

    QString text;
    switch (static_cast<GamePhase>(phaseKind)) {
        case GamePhase::NotStarted: text = QStringLiteral("Not started"); break;
        case GamePhase::RedBall:    text = QStringLiteral("Aim for a red"); break;
        case GamePhase::ColorBall:  text = QStringLiteral("Aim for a color"); break;
        case GamePhase::FreeBall:   text = QStringLiteral("Free ball"); break;
        case GamePhase::Foul:       text = QStringLiteral("Foul"); break;
        case GamePhase::GameOver:   text = QStringLiteral("Game over"); break;
    }
    if (isSimulating && !text.isEmpty()) {
        text += QStringLiteral(" (simulating...)");
    }
    return text;
}

QString translatedMessage(const QString& message, UiLanguage language) {
    (void)language;
    return message;
}

QString translatedFoulText(int foulType, int penaltyPoints, UiLanguage language) {
    const auto type = static_cast<FoulType>(foulType);
    const bool english = language == UiLanguage::English;

    QString reason;
    if (english) {
        switch (type) {
            case FoulType::MissedAll:        reason = QStringLiteral("Missed all balls"); break;
            case FoulType::WrongBallFirst:  reason = QStringLiteral("Wrong ball hit first"); break;
            case FoulType::WhitePocketed:   reason = QStringLiteral("Cue ball potted"); break;
            case FoulType::NoBallHitCushion: reason = QStringLiteral("No ball hit a cushion"); break;
            case FoulType::ColorPocketed:   reason = QStringLiteral("Color ball potted"); break;
            case FoulType::BallOffTable:    reason = QStringLiteral("Ball off the table"); break;
            case FoulType::PushShot:        reason = QStringLiteral("Push shot"); break;
            case FoulType::Other:           reason = QStringLiteral("Foul"); break;
            case FoulType::None:            return QString();
        }
        return penaltyPoints > 0
            ? QStringLiteral("%1. Penalty: %2 points").arg(reason).arg(penaltyPoints)
            : reason;
    }

    switch (type) {
        case FoulType::MissedAll:        reason = QStringLiteral("空杆"); break;
        case FoulType::WrongBallFirst:  reason = QStringLiteral("先击中错误球"); break;
        case FoulType::WhitePocketed:   reason = QStringLiteral("白球落袋"); break;
        case FoulType::NoBallHitCushion: reason = QStringLiteral("无球碰库"); break;
        case FoulType::ColorPocketed:   reason = QStringLiteral("彩球落袋"); break;
        case FoulType::BallOffTable:    reason = QStringLiteral("球离开台面"); break;
        case FoulType::PushShot:        reason = QStringLiteral("推杆"); break;
        case FoulType::Other:           reason = QStringLiteral("犯规"); break;
        case FoulType::None:            return QString();
    }
    return penaltyPoints > 0
        ? QStringLiteral("%1！罚 %2 分").arg(reason).arg(penaltyPoints)
        : reason;
}

} // namespace Snooker2D
