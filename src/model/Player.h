#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>

namespace Snooker2D {

class Player : public QObject {
    Q_OBJECT

public:
    explicit Player(const QString& name, QObject* parent = nullptr);
    ~Player() override = default;

    QString name() const { return m_name; }
    int score() const { return m_score; }
    int currentBreak() const { return m_currentBreak; }

    void setName(const QString& name);
    void addScore(int points);
    void resetBreak();
    void resetScore();

signals:
    void scoreChanged(int newScore);
    void nameChanged(const QString& newName);

private:
    QString m_name;
    int m_score = 0;
    int m_currentBreak = 0; // 当前连击得分（单杆）
};

} // namespace Snooker2D

#endif // PLAYER_H
