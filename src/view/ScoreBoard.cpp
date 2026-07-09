#include "ScoreBoard.h"
#include "../viewmodel/ScoreViewModel.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

namespace Snooker2D {

ScoreBoard::ScoreBoard(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void ScoreBoard::setViewModel(ScoreViewModel* viewModel) {
    m_viewModel = viewModel;
    if (!m_viewModel) return;

    connect(m_viewModel, &ScoreViewModel::player1ScoreChanged,
            this, &ScoreBoard::refresh);
    connect(m_viewModel, &ScoreViewModel::player2ScoreChanged,
            this, &ScoreBoard::refresh);
    connect(m_viewModel, &ScoreViewModel::foulMessageChanged,
            this, &ScoreBoard::refresh);
    connect(m_viewModel, &ScoreViewModel::statusMessageChanged,
            this, &ScoreBoard::refresh);

    refresh();
}

void ScoreBoard::setupUI() {
    auto* layout = new QVBoxLayout(this);

    m_titleLabel = new QLabel("计分板", this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_player1NameLabel = new QLabel("玩家 1", this);
    m_player1ScoreLabel = new QLabel("0", this);
    QFont scoreFont = m_player1ScoreLabel->font();
    scoreFont.setPointSize(24);
    scoreFont.setBold(true);
    m_player1ScoreLabel->setFont(scoreFont);
    m_player1BreakLabel = new QLabel("单杆: 0", this);

    m_player2NameLabel = new QLabel("玩家 2", this);
    m_player2ScoreLabel = new QLabel("0", this);
    m_player2ScoreLabel->setFont(scoreFont);
    m_player2BreakLabel = new QLabel("单杆: 0", this);

    m_foulLabel = new QLabel("", this);
    m_foulLabel->setStyleSheet("color: red; font-weight: bold;");
    m_foulLabel->setAlignment(Qt::AlignCenter);

    m_statusLabel = new QLabel("等待开始...", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_titleLabel);
    layout->addSpacing(10);
    layout->addWidget(m_player1NameLabel);
    layout->addWidget(m_player1ScoreLabel);
    layout->addWidget(m_player1BreakLabel);
    layout->addSpacing(15);
    layout->addWidget(m_player2NameLabel);
    layout->addWidget(m_player2ScoreLabel);
    layout->addWidget(m_player2BreakLabel);
    layout->addSpacing(15);
    layout->addWidget(m_foulLabel);
    layout->addWidget(m_statusLabel);
    layout->addStretch();
}

void ScoreBoard::refresh() {
    if (!m_viewModel) return;

    m_player1ScoreLabel->setText(QString::number(m_viewModel->player1Score()));
    m_player2ScoreLabel->setText(QString::number(m_viewModel->player2Score()));
    m_player1BreakLabel->setText(QString("单杆: %1").arg(m_viewModel->player1Break()));
    m_player2BreakLabel->setText(QString("单杆: %1").arg(m_viewModel->player2Break()));
    m_foulLabel->setText(m_viewModel->foulMessage());
    m_statusLabel->setText(m_viewModel->statusMessage());
}

} // namespace Snooker2D
