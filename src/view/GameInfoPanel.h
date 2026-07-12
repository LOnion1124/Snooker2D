#ifndef GAMEINFOPANEL_H
#define GAMEINFOPANEL_H

#include <QString>
#include <QWidget>

class QLabel;
class QPushButton;

namespace Snooker2D {

class GameViewModel;

class GameInfoPanel : public QWidget {
    Q_OBJECT

public:
    explicit GameInfoPanel(QWidget* parent = nullptr);
    ~GameInfoPanel() override = default;

    void setViewModel(GameViewModel* viewModel);

private:
    void setupUI();
    void updateCurrentPlayer();
    void updatePhase();
    void updateMessage(const QString& message);
    void updateWhiteBallPlacementHint();
    void restartGame();
    QString phaseStyleSheet(const QString& phaseText) const;

    GameViewModel* m_viewModel = nullptr;

    QLabel* m_playerIndicator = nullptr;
    QLabel* m_phaseLabel = nullptr;
    QLabel* m_placementHintLabel = nullptr;
    QLabel* m_messageLabel = nullptr;
    QPushButton* m_restartButton = nullptr;
};

} // namespace Snooker2D

#endif // GAMEINFOPANEL_H
