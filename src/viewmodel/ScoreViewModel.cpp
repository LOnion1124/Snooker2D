#include "ScoreViewModel.h"

namespace Snooker2D {

ScoreViewModel::ScoreViewModel(QObject* parent)
    : QObject(parent)
{
}

void ScoreViewModel::setPlayer1Score(int score) {
    if (m_player1Score != score) {
        m_player1Score = score;
        emit player1ScoreChanged();
    }
}

void ScoreViewModel::setPlayer2Score(int score) {
    if (m_player2Score != score) {
        m_player2Score = score;
        emit player2ScoreChanged();
    }
}

void ScoreViewModel::setFoulMessage(const QString& msg) {
    if (m_foulMessage != msg) {
        m_foulMessage = msg;
        emit foulMessageChanged();
    }
}

void ScoreViewModel::setStatusMessage(const QString& msg) {
    if (m_statusMessage != msg) {
        m_statusMessage = msg;
        emit statusMessageChanged();
    }
}

} // namespace Snooker2D
