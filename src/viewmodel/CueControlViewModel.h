#ifndef CUECONTROLVIEWMODEL_H
#define CUECONTROLVIEWMODEL_H

#include <QObject>

namespace Snooker2D {

class CueControlViewModel : public QObject {
    Q_OBJECT

    // 暴露给 CueControl 控件
    Q_PROPERTY(double angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(double power READ power WRITE setPower NOTIFY powerChanged)
    Q_PROPERTY(double englishX READ englishX WRITE setEnglishX NOTIFY englishChanged)
    Q_PROPERTY(double englishY READ englishY WRITE setEnglishY NOTIFY englishChanged)

public:
    explicit CueControlViewModel(QObject* parent = nullptr);
    ~CueControlViewModel() override = default;

    double angle() const { return m_angle; }
    double power() const { return m_power; }
    double englishX() const { return m_englishX; }
    double englishY() const { return m_englishY; }

    void setAngle(double angle);
    void setPower(double power);
    void setEnglishX(double ex);
    void setEnglishY(double ey);

    Q_INVOKABLE void reset();
    Q_INVOKABLE void setAngleAndPower(double angle, double power);

signals:
    void angleChanged(double angle);
    void powerChanged(double power);
    void englishChanged();
    void shootRequested();

private:
    double m_angle = 0.0;       // 0-360°
    double m_power = 50.0;      // 0-100
    double m_englishX = 0.0;    // 加塞 X 偏移（-1..1，预留）
    double m_englishY = 0.0;    // 加塞 Y 偏移（-1..1，预留）
};

} // namespace Snooker2D

#endif // CUECONTROLVIEWMODEL_H
