#pragma once

#include <QWidget>
#include "contracts/GameViewState.h"

class QLabel;

namespace Snooker2D {

class ScoreBoard : public QWidget {
    Q_OBJECT

public:
    explicit ScoreBoard(QWidget* parent = nullptr);
    ~ScoreBoard() override = default;

public slots:
    void applyScoreState(const ScoreViewState& state);

private:
    void setupUI();

    QLabel* m_titleLabel = nullptr;
    QLabel* m_player1NameLabel = nullptr;
    QLabel* m_player1ScoreLabel = nullptr;
    QLabel* m_player1BreakLabel = nullptr;
    QLabel* m_player2NameLabel = nullptr;
    QLabel* m_player2ScoreLabel = nullptr;
    QLabel* m_player2BreakLabel = nullptr;
    QLabel* m_foulLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
};

} // namespace Snooker2D
