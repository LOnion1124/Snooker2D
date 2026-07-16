#pragma once

#include <QWidget>
#include "contracts/GameViewState.h"
#include "UiLanguage.h"

class QSlider;
class QLabel;

namespace Snooker2D {

class CueControl : public QWidget {
    Q_OBJECT

public:
    explicit CueControl(QWidget* parent = nullptr);
    ~CueControl() override = default;

public slots:
    void applyCueState(const CueViewState& state);
    void setLanguage(UiLanguage language);

private:
    void setupUI();
    void refreshTexts();

    QSlider* m_angleSlider = nullptr;
    QSlider* m_powerSlider = nullptr;
    QLabel* m_angleLabel = nullptr;
    QLabel* m_powerLabel = nullptr;
    CueViewState m_state;
    UiLanguage m_language = UiLanguage::Chinese;
};

} // namespace Snooker2D
