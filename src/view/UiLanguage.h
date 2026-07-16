#pragma once

#include <QString>

namespace Snooker2D {

enum class UiLanguage {
    Chinese,
    English
};

QString translatedPhaseText(int phaseKind, bool isSimulating, UiLanguage language);
QString translatedMessage(const QString& message, UiLanguage language);

} // namespace Snooker2D
