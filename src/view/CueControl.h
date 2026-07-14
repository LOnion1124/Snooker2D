#pragma once

#include <QWidget>
#include "contracts/GameViewState.h"

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

private:
    void setupUI();

    QSlider* m_angleSlider = nullptr;
    QSlider* m_powerSlider = nullptr;
    QLabel* m_angleLabel = nullptr;
    QLabel* m_powerLabel = nullptr;
};

} // namespace Snooker2D
