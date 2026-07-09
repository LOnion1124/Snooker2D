#include "CueControlViewModel.h"

namespace Snooker2D {

CueControlViewModel::CueControlViewModel(QObject* parent)
    : QObject(parent)
{
}

void CueControlViewModel::setAngle(double angle) {
    // 钳制角度到 [0, 360)
    while (angle < 0.0) angle += 360.0;
    while (angle >= 360.0) angle -= 360.0;
    if (m_angle != angle) {
        m_angle = angle;
        emit angleChanged(m_angle);
    }
}

void CueControlViewModel::setPower(double power) {
    if (power < 0.0) power = 0.0;
    if (power > 100.0) power = 100.0;
    if (m_power != power) {
        m_power = power;
        emit powerChanged(m_power);
    }
}

void CueControlViewModel::setEnglishX(double ex) {
    if (ex < -1.0) ex = -1.0;
    if (ex > 1.0) ex = 1.0;
    if (m_englishX != ex) {
        m_englishX = ex;
        emit englishChanged();
    }
}

void CueControlViewModel::setEnglishY(double ey) {
    if (ey < -1.0) ey = -1.0;
    if (ey > 1.0) ey = 1.0;
    if (m_englishY != ey) {
        m_englishY = ey;
        emit englishChanged();
    }
}

void CueControlViewModel::reset() {
    m_angle = 0.0;
    m_power = 50.0;
    m_englishX = 0.0;
    m_englishY = 0.0;
    emit angleChanged(m_angle);
    emit powerChanged(m_power);
    emit englishChanged();
}

void CueControlViewModel::setAngleAndPower(double angle, double power) {
    setAngle(angle);
    setPower(power);
}

} // namespace Snooker2D
