#include "EnglishControlPanel.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

#include <cmath>
#include <functional>
#include <utility>

namespace Snooker2D {

class EnglishBallSelector : public QWidget {
public:
    explicit EnglishBallSelector(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMinimumSize(112, 112);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setCursor(Qt::CrossCursor);
    }

    void setValue(double englishX, double englishY) {
        m_englishX = qBound(-1.0, englishX, 1.0);
        m_englishY = qBound(-1.0, englishY, 1.0);

        const double length = std::hypot(m_englishX, m_englishY);
        if (length > 1.0) {
            m_englishX /= length;
            m_englishY /= length;
        }
        update();
    }

    void setValueChangedCallback(std::function<void(double, double)> callback) {
        m_valueChanged = std::move(callback);
    }

protected:
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QRectF bounds = ballRect();
        QRadialGradient gradient(bounds.center(), bounds.width() * 0.55,
                                 bounds.center() - QPointF(bounds.width() * 0.2,
                                                           bounds.height() * 0.2));
        gradient.setColorAt(0.0, QColor(255, 255, 255));
        gradient.setColorAt(0.65, QColor(238, 242, 244));
        gradient.setColorAt(1.0, QColor(196, 205, 210));

        painter.setPen(QPen(QColor(120, 130, 135), 1.2));
        painter.setBrush(gradient);
        painter.drawEllipse(bounds);

        painter.setPen(QPen(QColor(120, 130, 135, 110), 1.0));
        painter.drawLine(QPointF(bounds.center().x(), bounds.top() + 10.0),
                         QPointF(bounds.center().x(), bounds.bottom() - 10.0));
        painter.drawLine(QPointF(bounds.left() + 10.0, bounds.center().y()),
                         QPointF(bounds.right() - 10.0, bounds.center().y()));

        const QPointF marker = valueToPoint();
        painter.setPen(QPen(QColor(120, 0, 0), 1.0));
        painter.setBrush(QColor(220, 30, 30));
        painter.drawEllipse(marker, 5.0, 5.0);
    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() != Qt::LeftButton) {
            QWidget::mousePressEvent(event);
            return;
        }
        updateValueFromPosition(event->position());
        event->accept();
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (!(event->buttons() & Qt::LeftButton)) {
            QWidget::mouseMoveEvent(event);
            return;
        }
        updateValueFromPosition(event->position());
        event->accept();
    }

private:
    QRectF ballRect() const {
        const double size = qMin(width(), height()) - 8.0;
        return QRectF((width() - size) / 2.0, (height() - size) / 2.0, size, size);
    }

    QPointF valueToPoint() const {
        const QRectF bounds = ballRect();
        const double radius = bounds.width() / 2.0;
        return QPointF(bounds.center().x() + m_englishX * radius,
                       bounds.center().y() - m_englishY * radius);
    }

    void updateValueFromPosition(const QPointF& position) {
        const QRectF bounds = ballRect();
        const QPointF center = bounds.center();
        const double radius = bounds.width() / 2.0;
        double x = (position.x() - center.x()) / radius;
        double y = (center.y() - position.y()) / radius;

        const double length = std::hypot(x, y);
        if (length > 1.0) {
            x /= length;
            y /= length;
        }

        setValue(x, y);
        if (m_valueChanged) {
            m_valueChanged(m_englishX, m_englishY);
        }
    }

    double m_englishX = 0.0;
    double m_englishY = 0.0;
    std::function<void(double, double)> m_valueChanged;
};

EnglishControlPanel::EnglishControlPanel(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void EnglishControlPanel::applyCueState(const CueViewState& state) {
    m_state = state;
    m_selector->setValue(state.englishX, state.englishY);
    refreshTexts();
}

void EnglishControlPanel::setLanguage(UiLanguage language) {
    if (m_language == language) return;
    m_language = language;
    refreshTexts();
}

void EnglishControlPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    m_titleLabel = new QLabel(this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_selector = new EnglishBallSelector(this);
    m_selector->setValueChangedCallback([this](double englishX, double englishY) {
        setEnglishSelection(englishX, englishY);
    });

    m_valueLabel = new QLabel(this);
    m_valueLabel->setAlignment(Qt::AlignCenter);
    m_valueLabel->setMinimumHeight(20);

    m_resetButton = new QPushButton(this);
    m_resetButton->setCursor(Qt::PointingHandCursor);
    m_resetButton->setStyleSheet(QStringLiteral(
        "QPushButton {"
        "background-color: #455a64;"
        "color: white;"
        "border: none;"
        "border-radius: 4px;"
        "padding: 7px 10px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #546e7a; }"
        "QPushButton:pressed { background-color: #37474f; }"
    ));
    connect(m_resetButton, &QPushButton::clicked, this, [this]() {
        setEnglishSelection(0.0, 0.0);
    });

    auto* buttonRow = new QHBoxLayout();
    buttonRow->addStretch();
    buttonRow->addWidget(m_resetButton);
    buttonRow->addStretch();

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_selector, 0, Qt::AlignHCenter);
    layout->addWidget(m_valueLabel);
    layout->addLayout(buttonRow);
    refreshTexts();
}

void EnglishControlPanel::refreshTexts() {
    const bool english = m_language == UiLanguage::English;
    m_titleLabel->setText(english ? QStringLiteral("English") : QStringLiteral("加塞"));
    m_valueLabel->setText(englishDescription());
    m_resetButton->setText(english ? QStringLiteral("Reset") : QStringLiteral("重置"));
    m_resetButton->setToolTip(english
        ? QStringLiteral("Reset english to center")
        : QStringLiteral("将加塞重置到中心"));
    m_selector->setToolTip(english
        ? QStringLiteral("Click or drag on the cue ball to set english")
        : QStringLiteral("在白球上点击或拖拽设置加塞"));
}

void EnglishControlPanel::setEnglishSelection(double englishX, double englishY) {
    m_state.englishX = englishX;
    m_state.englishY = englishY;
    m_selector->setValue(englishX, englishY);
    refreshTexts();
    emit englishChanged(englishX, englishY);
}

QString EnglishControlPanel::englishDescription() const {
    const double x = m_state.englishX;
    const double y = m_state.englishY;
    const bool english = m_language == UiLanguage::English;

    if (std::abs(x) < 0.05 && std::abs(y) < 0.05) {
        return english ? QStringLiteral("Center") : QStringLiteral("中心");
    }

    QStringList parts;
    if (y > 0.05) {
        parts << (english ? QStringLiteral("top") : QStringLiteral("上塞"));
    } else if (y < -0.05) {
        parts << (english ? QStringLiteral("bottom") : QStringLiteral("下塞"));
    }

    if (x > 0.05) {
        parts << (english ? QStringLiteral("right") : QStringLiteral("右塞"));
    } else if (x < -0.05) {
        parts << (english ? QStringLiteral("left") : QStringLiteral("左塞"));
    }

    return parts.join(english ? QStringLiteral(" ") : QString());
}

} // namespace Snooker2D

