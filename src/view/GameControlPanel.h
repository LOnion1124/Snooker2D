#pragma once

#include <QWidget>
#include "UiLanguage.h"

class QPushButton;
class QLabel;

namespace Snooker2D {

class GameControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit GameControlPanel(QWidget* parent = nullptr);
    ~GameControlPanel() override = default;

signals:
    void restartRequested();
    void languageChanged(UiLanguage language);

public slots:
    void setLanguage(UiLanguage language);

private:
    void setupUI();
    void openSettingsDialog();
    void refreshTexts();

    QLabel* m_titleLabel = nullptr;
    QPushButton* m_settingsButton = nullptr;
    QPushButton* m_restartButton = nullptr;
    UiLanguage m_language = UiLanguage::Chinese;
};

} // namespace Snooker2D
