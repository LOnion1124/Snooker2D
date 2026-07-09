#ifndef SCOREVIEWMODEL_H
#define SCOREVIEWMODEL_H

#include <QObject>
#include <QString>

namespace Snooker2D {

class ScoreViewModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(int player1Score READ player1Score NOTIFY player1ScoreChanged)
    Q_PROPERTY(int player2Score READ player2Score NOTIFY player2ScoreChanged)
    Q_PROPERTY(int player1Break READ player1Break NOTIFY player1BreakChanged)
    Q_PROPERTY(int player2Break READ player2Break NOTIFY player2BreakChanged)
    Q_PROPERTY(QString foulMessage READ foulMessage NOTIFY foulMessageChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)

public:
    explicit ScoreViewModel(QObject* parent = nullptr);
    ~ScoreViewModel() override = default;

    int player1Score() const { return m_player1Score; }
    int player2Score() const { return m_player2Score; }
    int player1Break() const { return m_player1Break; }
    int player2Break() const { return m_player2Break; }
    QString foulMessage() const { return m_foulMessage; }
    QString statusMessage() const { return m_statusMessage; }

    void setPlayer1Score(int score);
    void setPlayer2Score(int score);
    void setFoulMessage(const QString& msg);
    void setStatusMessage(const QString& msg);

signals:
    void player1ScoreChanged();
    void player2ScoreChanged();
    void player1BreakChanged();
    void player2BreakChanged();
    void foulMessageChanged();
    void statusMessageChanged();

private:
    int m_player1Score = 0;
    int m_player2Score = 0;
    int m_player1Break = 0;
    int m_player2Break = 0;
    QString m_foulMessage;
    QString m_statusMessage;
};

} // namespace Snooker2D

#endif // SCOREVIEWMODEL_H
