#include "UiLanguage.h"
#include "../common/Types.h"

#include <QRegularExpression>

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
    if (language == UiLanguage::Chinese || message.isEmpty()) {
        return message;
    }

    static const QRegularExpression penaltyPattern(QStringLiteral("罚 (\\d+) 分"));
    const QRegularExpressionMatch penaltyMatch = penaltyPattern.match(message);
    const QString penalty = penaltyMatch.hasMatch()
        ? penaltyMatch.captured(1)
        : QString();

    if (message.startsWith(QStringLiteral("空杆"))) {
        return penalty.isEmpty()
            ? QStringLiteral("Missed all balls.")
            : QStringLiteral("Missed all balls. Penalty: %1 points").arg(penalty);
    }
    if (message.startsWith(QStringLiteral("先击中错误球"))) {
        return penalty.isEmpty()
            ? QStringLiteral("Wrong ball hit first.")
            : QStringLiteral("Wrong ball hit first. Penalty: %1 points").arg(penalty);
    }
    if (message.startsWith(QStringLiteral("彩球落袋"))) {
        return penalty.isEmpty()
            ? QStringLiteral("Color ball potted.")
            : QStringLiteral("Color ball potted. Penalty: %1 points").arg(penalty);
    }
    if (message.startsWith(QStringLiteral("无球碰库"))) {
        return penalty.isEmpty()
            ? QStringLiteral("No ball hit a cushion.")
            : QStringLiteral("No ball hit a cushion. Penalty: %1 points").arg(penalty);
    }

    return message;
}

} // namespace Snooker2D
