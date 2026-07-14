#pragma once

#include <QString>
#include <QWidget>
#include "contracts/GameViewState.h"

class QLabel;
class QPushButton;

namespace Snooker2D {

class GameInfoPanel : public QWidget {
    Q_OBJECT

public:
    explicit GameInfoPanel(QWidget* parent = nullptr);
    ~GameInfoPanel() override = default;

public slots:
    void applyGameInfoState(const GameInfoViewState& state);

signals:
    void restartRequested();

private:
    void setupUI();
    QString phaseStyleSheet(int phaseKind) const;

    QLabel* m_playerIndicator = nullptr;
    QLabel* m_phaseLabel = nullptr;
    QLabel* m_placementHintLabel = nullptr;
    QLabel* m_messageLabel = nullptr;
    QPushButton* m_restartButton = nullptr;
};

} // namespace Snooker2D
