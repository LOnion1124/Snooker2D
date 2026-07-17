#pragma once

#include <QWidget>
#include "contracts/GameViewState.h"
#include "UiLanguage.h"

class QSlider;
class QLabel;
class QPushButton;

namespace Snooker2D {

class CueControl : public QWidget {
    Q_OBJECT

public:
    explicit CueControl(QWidget* parent = nullptr);
    ~CueControl() override = default;

public slots:
    void applyCueState(const CueViewState& state);
    void setLanguage(UiLanguage language);

signals:
    void englishChanged(double englishX, double englishY);

private:
    void setupUI();
    void refreshTexts();
    void setEnglishSelection(double englishX, double englishY);
    void refreshEnglishButtons();
    void styleEnglishButton(QPushButton* button, bool active) const;

    QSlider* m_angleSlider = nullptr;
    QSlider* m_powerSlider = nullptr;
    QLabel* m_angleLabel = nullptr;
    QLabel* m_powerLabel = nullptr;
    QLabel* m_englishLabel = nullptr;
    QPushButton* m_topLeftButton = nullptr;
    QPushButton* m_topButton = nullptr;
    QPushButton* m_topRightButton = nullptr;
    QPushButton* m_leftButton = nullptr;
    QPushButton* m_centerButton = nullptr;
    QPushButton* m_rightButton = nullptr;
    QPushButton* m_bottomLeftButton = nullptr;
    QPushButton* m_bottomButton = nullptr;
    QPushButton* m_bottomRightButton = nullptr;
    CueViewState m_state;
    UiLanguage m_language = UiLanguage::Chinese;
};

} // namespace Snooker2D
