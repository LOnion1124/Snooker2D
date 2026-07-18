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
    void aimingGuideVisibilityChanged(bool enabled);

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
    bool m_aimingGuideEnabled = true;
};

} // namespace Snooker2D
