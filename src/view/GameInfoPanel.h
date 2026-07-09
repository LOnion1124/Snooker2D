#ifndef GAMEINFOPANEL_H
#define GAMEINFOPANEL_H

#include <QWidget>

class QLabel;

namespace Snooker2D {

class GameInfoPanel : public QWidget {
    Q_OBJECT

public:
    explicit GameInfoPanel(QWidget* parent = nullptr);
    ~GameInfoPanel() override = default;

    void setCurrentPlayer(int playerNumber);   // 1 或 2
    void setPhase(const QString& phaseText);
    void setMessage(const QString& message);

private:
    void setupUI();

    QLabel* m_playerIndicator = nullptr;
    QLabel* m_phaseLabel = nullptr;
    QLabel* m_messageLabel = nullptr;
};

} // namespace Snooker2D

#endif // GAMEINFOPANEL_H
