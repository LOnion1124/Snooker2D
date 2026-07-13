#ifndef GAMEINFOPANEL_H
#define GAMEINFOPANEL_H

#include <QString>
#include <QWidget>

class QLabel;
class QPushButton;

namespace Snooker2D {

class GameUiBus;

class GameInfoPanel : public QWidget {
    Q_OBJECT

public:
    explicit GameInfoPanel(QWidget* parent = nullptr);
    ~GameInfoPanel() override = default;

    void bind(GameUiBus* bus);

private:
    void setupUI();
    void applyGameInfoState(const struct GameInfoViewState& state);
    QString phaseStyleSheet(int phaseKind) const;

    GameUiBus* m_bus = nullptr;

    QLabel* m_playerIndicator = nullptr;
    QLabel* m_phaseLabel = nullptr;
    QLabel* m_placementHintLabel = nullptr;
    QLabel* m_messageLabel = nullptr;
    QPushButton* m_restartButton = nullptr;
};

} // namespace Snooker2D

#endif // GAMEINFOPANEL_H
