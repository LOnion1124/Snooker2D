#pragma once

#include <QWidget>
#include "contracts/GameViewState.h"
#include "UiLanguage.h"

class QLabel;
class QPushButton;

namespace Snooker2D {

class EnglishBallSelector;

class EnglishControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit EnglishControlPanel(QWidget* parent = nullptr);
    ~EnglishControlPanel() override = default;

public slots:
    void applyCueState(const CueViewState& state);
    void setLanguage(UiLanguage language);

signals:
    void englishChanged(double englishX, double englishY);

private:
    void setupUI();
    void refreshTexts();
    void setEnglishSelection(double englishX, double englishY);
    QString englishDescription() const;

    QLabel* m_titleLabel = nullptr;
    QLabel* m_valueLabel = nullptr;
    EnglishBallSelector* m_selector = nullptr;
    QPushButton* m_resetButton = nullptr;
    CueViewState m_state;
    UiLanguage m_language = UiLanguage::Chinese;
};

} // namespace Snooker2D

