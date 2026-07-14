#include "CueControl.h"

#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Snooker2D {

CueControl::CueControl(QWidget* parent) : QWidget(parent) { setupUI(); }

void CueControl::applyCueState(const CueViewState& state) {
    m_angleSlider->blockSignals(true);
    m_angleSlider->setValue(static_cast<int>(state.angle));
    m_angleSlider->blockSignals(false);
    m_angleLabel->setText(QString("角度: %1°").arg(state.angle, 0, 'f', 1));

    m_powerSlider->blockSignals(true);
    m_powerSlider->setValue(static_cast<int>(state.power));
    m_powerSlider->blockSignals(false);
    m_powerLabel->setText(QString("力度: %1%").arg(state.power, 0, 'f', 1));
}

void CueControl::setupUI() {
    auto* layout = new QHBoxLayout(this);

    auto* angleLayout = new QVBoxLayout();
    m_angleLabel = new QLabel("角度: 0.0°", this);
    m_angleSlider = new QSlider(Qt::Horizontal, this);
    m_angleSlider->setRange(0, 359);
    m_angleSlider->setEnabled(false);
    angleLayout->addWidget(m_angleLabel);
    angleLayout->addWidget(m_angleSlider);

    auto* powerLayout = new QVBoxLayout();
    m_powerLabel = new QLabel("力度: 50.0%", this);
    m_powerSlider = new QSlider(Qt::Horizontal, this);
    m_powerSlider->setRange(0, 100);
    m_powerSlider->setEnabled(false);
    powerLayout->addWidget(m_powerLabel);
    powerLayout->addWidget(m_powerSlider);

    layout->addLayout(angleLayout);
    layout->addLayout(powerLayout);
}

} // namespace Snooker2D
