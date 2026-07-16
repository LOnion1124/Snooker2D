#include "CueControl.h"

#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Snooker2D {

CueControl::CueControl(QWidget* parent) : QWidget(parent) { setupUI(); }

void CueControl::applyCueState(const CueViewState& state) {
    m_state = state;
    m_angleSlider->blockSignals(true);
    m_angleSlider->setValue(static_cast<int>(state.angle));
    m_angleSlider->blockSignals(false);

    m_powerSlider->blockSignals(true);
    m_powerSlider->setValue(static_cast<int>(state.power));
    m_powerSlider->blockSignals(false);
    refreshTexts();
}

void CueControl::setLanguage(UiLanguage language) {
    if (m_language == language) return;
    m_language = language;
    refreshTexts();
}

void CueControl::setupUI() {
    auto* layout = new QHBoxLayout(this);

    auto* angleLayout = new QVBoxLayout();
    m_angleLabel = new QLabel(this);
    m_angleSlider = new QSlider(Qt::Horizontal, this);
    m_angleSlider->setRange(0, 359);
    m_angleSlider->setEnabled(false);
    angleLayout->addWidget(m_angleLabel);
    angleLayout->addWidget(m_angleSlider);

    auto* powerLayout = new QVBoxLayout();
    m_powerLabel = new QLabel(this);
    m_powerSlider = new QSlider(Qt::Horizontal, this);
    m_powerSlider->setRange(0, 100);
    m_powerSlider->setEnabled(false);
    powerLayout->addWidget(m_powerLabel);
    powerLayout->addWidget(m_powerSlider);

    layout->addLayout(angleLayout);
    layout->addLayout(powerLayout);
    refreshTexts();
}

void CueControl::refreshTexts() {
    if (m_language == UiLanguage::English) {
        m_angleLabel->setText(QStringLiteral("Angle: %1°").arg(m_state.angle, 0, 'f', 1));
        m_powerLabel->setText(QStringLiteral("Power: %1%").arg(m_state.power, 0, 'f', 1));
        return;
    }

    m_angleLabel->setText(QStringLiteral("角度: %1°").arg(m_state.angle, 0, 'f', 1));
    m_powerLabel->setText(QStringLiteral("力度: %1%").arg(m_state.power, 0, 'f', 1));
}

} // namespace Snooker2D
