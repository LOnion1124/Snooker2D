#pragma once

#include <QWidget>
#include "contracts/GameViewState.h"
#include "UiLanguage.h"

class QLabel;
class QTimer;

namespace Snooker2D {

class ScoreBoard : public QWidget {
    Q_OBJECT

public:
    explicit ScoreBoard(QWidget* parent = nullptr);
    ~ScoreBoard() override = default;

public slots:
    void applyScoreState(const ScoreViewState& state);
    void setLanguage(UiLanguage language);

private slots:
    void onFoulTimerTimeout();

private:
    void setupUI();
    void refreshTexts();

    QLabel* m_titleLabel = nullptr;
    QLabel* m_player1NameLabel = nullptr;
    QLabel* m_player1ScoreLabel = nullptr;
    QLabel* m_player1BreakLabel = nullptr;
    QLabel* m_player2NameLabel = nullptr;
    QLabel* m_player2ScoreLabel = nullptr;
    QLabel* m_player2BreakLabel = nullptr;
    QLabel* m_foulLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QTimer* m_foulTimer = nullptr;
    ScoreViewState m_state;
    UiLanguage m_language = UiLanguage::Chinese;
    bool m_hasState = false;
    bool m_foulVisible = false;
};

} // namespace Snooker2D
