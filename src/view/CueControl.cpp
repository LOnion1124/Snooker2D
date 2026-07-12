#include "CueControl.h"
#include "../viewmodel/CueControlViewModel.h"

#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Snooker2D {

CueControl::CueControl(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void CueControl::setViewModel(CueControlViewModel* viewModel) {
    m_viewModel = viewModel;
    if (!m_viewModel) return;

    // 绑定 ViewModel 属性 → 控件
    connect(m_viewModel, &CueControlViewModel::angleChanged, this, [this](double angle) {
        m_angleSlider->blockSignals(true);
        m_angleSlider->setValue(static_cast<int>(angle));
        m_angleSlider->blockSignals(false);
        m_angleLabel->setText(QString("角度: %1°").arg(angle, 0, 'f', 1));
    });
    connect(m_viewModel, &CueControlViewModel::powerChanged, this, [this](double power) {
        m_powerSlider->blockSignals(true);
        m_powerSlider->setValue(static_cast<int>(power));
        m_powerSlider->blockSignals(false);
        m_powerLabel->setText(QString("力度: %1%").arg(power, 0, 'f', 1));
    });
}

void CueControl::setupUI() {
    auto* layout = new QHBoxLayout(this);

    // 角度控制
    auto* angleLayout = new QVBoxLayout();
    m_angleLabel = new QLabel("角度: 0.0°", this);
    m_angleSlider = new QSlider(Qt::Horizontal, this);
    m_angleSlider->setRange(0, 359);
    m_angleSlider->setValue(0);
    m_angleSlider->setEnabled(false);
    m_angleSlider->setFocusPolicy(Qt::NoFocus);
    angleLayout->addWidget(m_angleLabel);
    angleLayout->addWidget(m_angleSlider);

    // 力度控制
    auto* powerLayout = new QVBoxLayout();
    m_powerLabel = new QLabel("力度: 50.0%", this);
    m_powerSlider = new QSlider(Qt::Horizontal, this);
    m_powerSlider->setRange(0, 100);
    m_powerSlider->setValue(50);
    m_powerSlider->setEnabled(false);
    m_powerSlider->setFocusPolicy(Qt::NoFocus);
    powerLayout->addWidget(m_powerLabel);
    powerLayout->addWidget(m_powerSlider);

    layout->addLayout(angleLayout);
    layout->addLayout(powerLayout);
}

} // namespace Snooker2D
