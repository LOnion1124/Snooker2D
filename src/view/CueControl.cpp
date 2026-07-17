#include "CueControl.h"

#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QStringList>
#include <QVBoxLayout>

#include <cmath>

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
    layout->setSpacing(14);

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

    auto* englishLayout = new QVBoxLayout();
    m_englishLabel = new QLabel(this);
    m_englishLabel->setAlignment(Qt::AlignCenter);

    auto* englishGrid = new QGridLayout();
    englishGrid->setSpacing(4);

    const auto makeButton = [this](const QString& text, double englishX, double englishY) {
        auto* button = new QPushButton(text, this);
        button->setFixedSize(34, 28);
        button->setCursor(Qt::PointingHandCursor);
        button->setFocusPolicy(Qt::NoFocus);
        connect(button, &QPushButton::clicked, this, [this, englishX, englishY]() {
            setEnglishSelection(englishX, englishY);
        });
        return button;
    };

    m_topLeftButton = makeButton(QStringLiteral("↖"), -0.7, 0.7);
    m_topButton = makeButton(QStringLiteral("↑"), 0.0, 1.0);
    m_topRightButton = makeButton(QStringLiteral("↗"), 0.7, 0.7);
    m_leftButton = makeButton(QStringLiteral("←"), -1.0, 0.0);
    m_centerButton = makeButton(QStringLiteral("·"), 0.0, 0.0);
    m_rightButton = makeButton(QStringLiteral("→"), 1.0, 0.0);
    m_bottomLeftButton = makeButton(QStringLiteral("↙"), -0.7, -0.7);
    m_bottomButton = makeButton(QStringLiteral("↓"), 0.0, -1.0);
    m_bottomRightButton = makeButton(QStringLiteral("↘"), 0.7, -0.7);

    englishGrid->addWidget(m_topLeftButton, 0, 0);
    englishGrid->addWidget(m_topButton, 0, 1);
    englishGrid->addWidget(m_topRightButton, 0, 2);
    englishGrid->addWidget(m_leftButton, 1, 0);
    englishGrid->addWidget(m_centerButton, 1, 1);
    englishGrid->addWidget(m_rightButton, 1, 2);
    englishGrid->addWidget(m_bottomLeftButton, 2, 0);
    englishGrid->addWidget(m_bottomButton, 2, 1);
    englishGrid->addWidget(m_bottomRightButton, 2, 2);

    englishLayout->addWidget(m_englishLabel);
    englishLayout->addLayout(englishGrid);

    layout->addLayout(angleLayout);
    layout->addLayout(powerLayout);
    layout->addLayout(englishLayout);
    refreshTexts();
}

void CueControl::refreshTexts() {
    const auto englishDescription = [this]() {
        const double x = m_state.englishX;
        const double y = m_state.englishY;
        if (std::abs(x) < 0.05 && std::abs(y) < 0.05) {
            return m_language == UiLanguage::English
                ? QStringLiteral("Center")
                : QStringLiteral("中心");
        }

        QStringList parts;
        if (y > 0.05) {
            parts << (m_language == UiLanguage::English
                ? QStringLiteral("top")
                : QStringLiteral("上塞"));
        } else if (y < -0.05) {
            parts << (m_language == UiLanguage::English
                ? QStringLiteral("bottom")
                : QStringLiteral("下塞"));
        }

        if (x > 0.05) {
            parts << (m_language == UiLanguage::English
                ? QStringLiteral("right")
                : QStringLiteral("右塞"));
        } else if (x < -0.05) {
            parts << (m_language == UiLanguage::English
                ? QStringLiteral("left")
                : QStringLiteral("左塞"));
        }

        return parts.join(m_language == UiLanguage::English
            ? QStringLiteral(" ")
            : QString());
    };

    if (m_language == UiLanguage::English) {
        m_angleLabel->setText(QStringLiteral("Angle: %1°").arg(m_state.angle, 0, 'f', 1));
        m_powerLabel->setText(QStringLiteral("Power: %1%").arg(m_state.power, 0, 'f', 1));
        m_englishLabel->setText(QStringLiteral("English: %1").arg(englishDescription()));
        m_topLeftButton->setToolTip(QStringLiteral("Top-left english"));
        m_topButton->setToolTip(QStringLiteral("Top english"));
        m_topRightButton->setToolTip(QStringLiteral("Top-right english"));
        m_leftButton->setToolTip(QStringLiteral("Left english"));
        m_centerButton->setToolTip(QStringLiteral("Reset english"));
        m_rightButton->setToolTip(QStringLiteral("Right english"));
        m_bottomLeftButton->setToolTip(QStringLiteral("Bottom-left english"));
        m_bottomButton->setToolTip(QStringLiteral("Bottom english"));
        m_bottomRightButton->setToolTip(QStringLiteral("Bottom-right english"));
        refreshEnglishButtons();
        return;
    }

    m_angleLabel->setText(QStringLiteral("角度: %1°").arg(m_state.angle, 0, 'f', 1));
    m_powerLabel->setText(QStringLiteral("力度: %1%").arg(m_state.power, 0, 'f', 1));
    m_englishLabel->setText(QStringLiteral("加塞: %1").arg(englishDescription()));
    m_topLeftButton->setToolTip(QStringLiteral("上左塞"));
    m_topButton->setToolTip(QStringLiteral("上塞"));
    m_topRightButton->setToolTip(QStringLiteral("上右塞"));
    m_leftButton->setToolTip(QStringLiteral("左塞"));
    m_centerButton->setToolTip(QStringLiteral("复位加塞"));
    m_rightButton->setToolTip(QStringLiteral("右塞"));
    m_bottomLeftButton->setToolTip(QStringLiteral("下左塞"));
    m_bottomButton->setToolTip(QStringLiteral("下塞"));
    m_bottomRightButton->setToolTip(QStringLiteral("下右塞"));
    refreshEnglishButtons();
}

void CueControl::setEnglishSelection(double englishX, double englishY) {
    m_state.englishX = englishX;
    m_state.englishY = englishY;
    refreshTexts();
    emit englishChanged(englishX, englishY);
}

void CueControl::refreshEnglishButtons() {
    const double x = m_state.englishX;
    const double y = m_state.englishY;
    const bool center = std::abs(x) < 0.15 && std::abs(y) < 0.15;
    const bool top = y > 0.35;
    const bool bottom = y < -0.35;
    const bool right = x > 0.35;
    const bool left = x < -0.35;

    styleEnglishButton(m_topLeftButton, top && left);
    styleEnglishButton(m_topButton, top && !left && !right);
    styleEnglishButton(m_topRightButton, top && right);
    styleEnglishButton(m_leftButton, left && !top && !bottom);
    styleEnglishButton(m_centerButton, center);
    styleEnglishButton(m_rightButton, right && !top && !bottom);
    styleEnglishButton(m_bottomLeftButton, bottom && left);
    styleEnglishButton(m_bottomButton, bottom && !left && !right);
    styleEnglishButton(m_bottomRightButton, bottom && right);
}

void CueControl::styleEnglishButton(QPushButton* button, bool active) const {
    if (!button) return;

    button->setStyleSheet(active
        ? QStringLiteral(
            "QPushButton {"
            "background-color: #b83232;"
            "color: white;"
            "border: 1px solid #7f2424;"
            "border-radius: 4px;"
            "font-weight: bold;"
            "}"
            "QPushButton:hover { background-color: #c94747; }")
        : QStringLiteral(
            "QPushButton {"
            "background-color: #eceff1;"
            "color: #263238;"
            "border: 1px solid #b0bec5;"
            "border-radius: 4px;"
            "font-weight: bold;"
            "}"
            "QPushButton:hover { background-color: #dfe6e9; }"));
}

} // namespace Snooker2D
