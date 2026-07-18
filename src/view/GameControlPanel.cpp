#include "GameControlPanel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace Snooker2D {

GameControlPanel::GameControlPanel(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void GameControlPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_titleLabel = new QLabel(this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);

    m_settingsButton = new QPushButton(this);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    m_settingsButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "background-color: #455a64;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 8px 12px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #546e7a; }"
        "QPushButton:pressed { background-color: #37474f; }"
    ));
    connect(m_settingsButton, &QPushButton::clicked,
            this, &GameControlPanel::openSettingsDialog);

    m_restartButton = new QPushButton(this);
    m_restartButton->setCursor(Qt::PointingHandCursor);
    m_restartButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "background-color: #3f7f5f;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 8px 12px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #4f9a73; }"
        "QPushButton:pressed { background-color: #34694f; }"
    ));
    connect(m_restartButton, &QPushButton::clicked,
            this, &GameControlPanel::restartRequested);

    buttonLayout->addWidget(m_settingsButton);
    buttonLayout->addWidget(m_restartButton);
    layout->addWidget(m_titleLabel);
    layout->addLayout(buttonLayout);
    refreshTexts();
}

void GameControlPanel::setLanguage(UiLanguage language) {
    if (m_language == language) return;
    m_language = language;
    refreshTexts();
}

void GameControlPanel::openSettingsDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle(m_language == UiLanguage::English
        ? QStringLiteral("Settings")
        : QStringLiteral("设置"));

    auto* layout = new QFormLayout(&dialog);
    auto* languageCombo = new QComboBox(&dialog);
    languageCombo->addItem(QStringLiteral("中文"), static_cast<int>(UiLanguage::Chinese));
    languageCombo->addItem(QStringLiteral("English"), static_cast<int>(UiLanguage::English));
    languageCombo->setCurrentIndex(m_language == UiLanguage::English ? 1 : 0);

    layout->addRow(m_language == UiLanguage::English
        ? QStringLiteral("Language:")
        : QStringLiteral("语言:"), languageCombo);

    auto* aimingGuideCheckBox = new QCheckBox(&dialog);
    aimingGuideCheckBox->setChecked(m_aimingGuideEnabled);
    aimingGuideCheckBox->setText(m_language == UiLanguage::English
        ? QStringLiteral("Show aiming guide")
        : QStringLiteral("显示辅助瞄准线"));
    layout->addRow(QString(), aimingGuideCheckBox);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText(m_language == UiLanguage::English
        ? QStringLiteral("OK")
        : QStringLiteral("确定"));
    buttons->button(QDialogButtonBox::Cancel)->setText(m_language == UiLanguage::English
        ? QStringLiteral("Cancel")
        : QStringLiteral("取消"));
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) return;

    const UiLanguage selectedLanguage = static_cast<UiLanguage>(languageCombo->currentData().toInt());
    const bool selectedAimingGuideEnabled = aimingGuideCheckBox->isChecked();

    if (selectedAimingGuideEnabled != m_aimingGuideEnabled) {
        m_aimingGuideEnabled = selectedAimingGuideEnabled;
        emit aimingGuideVisibilityChanged(selectedAimingGuideEnabled);
    }

    if (selectedLanguage != m_language) {
        setLanguage(selectedLanguage);
        emit languageChanged(selectedLanguage);
    }
}

void GameControlPanel::refreshTexts() {
    const bool english = m_language == UiLanguage::English;
    m_titleLabel->setText(english ? QStringLiteral("Game Controls") : QStringLiteral("游戏控制"));
    m_settingsButton->setText(english ? QStringLiteral("Settings") : QStringLiteral("设置"));
    m_settingsButton->setToolTip(english
        ? QStringLiteral("Open settings")
        : QStringLiteral("打开设置"));
    m_restartButton->setText(english ? QStringLiteral("Restart") : QStringLiteral("重启游戏"));
}

} // namespace Snooker2D
