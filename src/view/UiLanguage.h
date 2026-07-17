#pragma once

#include <QString>

namespace Snooker2D {

enum class UiLanguage {
    Chinese,
    English
};

QString translatedPhaseText(int phaseKind, bool isSimulating, UiLanguage language);
QString translatedMessage(const QString& message, UiLanguage language);
QString translatedFoulText(int foulType, int penaltyPoints, UiLanguage language);

} // namespace Snooker2D
