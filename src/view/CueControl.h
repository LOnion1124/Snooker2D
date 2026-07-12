#ifndef CUECONTROL_H
#define CUECONTROL_H

#include <QWidget>

class QSlider;
class QLabel;

namespace Snooker2D {

class CueControlViewModel;

class CueControl : public QWidget {
    Q_OBJECT

public:
    explicit CueControl(QWidget* parent = nullptr);
    ~CueControl() override = default;

    void setViewModel(CueControlViewModel* viewModel);

private:
    void setupUI();

    CueControlViewModel* m_viewModel = nullptr;

    QSlider* m_angleSlider = nullptr;
    QSlider* m_powerSlider = nullptr;
    QLabel* m_angleLabel = nullptr;
    QLabel* m_powerLabel = nullptr;
};

} // namespace Snooker2D

#endif // CUECONTROL_H
