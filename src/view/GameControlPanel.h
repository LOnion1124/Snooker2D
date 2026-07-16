#pragma once

#include <QWidget>

class QPushButton;

namespace Snooker2D {

class GameControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit GameControlPanel(QWidget* parent = nullptr);
    ~GameControlPanel() override = default;

signals:
    void restartRequested();

private:
    void setupUI();

    QPushButton* m_settingsButton = nullptr;
    QPushButton* m_restartButton = nullptr;
};

} // namespace Snooker2D
