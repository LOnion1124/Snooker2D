#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <QWidget>

class QLabel;

namespace Snooker2D {

class ScoreViewModel;

class ScoreBoard : public QWidget {
    Q_OBJECT

public:
    explicit ScoreBoard(QWidget* parent = nullptr);
    ~ScoreBoard() override = default;

    void setViewModel(ScoreViewModel* viewModel);

private slots:
    void refresh();

private:
    void setupUI();

    ScoreViewModel* m_viewModel = nullptr;

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

#endif // SCOREBOARD_H
