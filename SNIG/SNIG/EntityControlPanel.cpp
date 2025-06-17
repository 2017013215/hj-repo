#include "StdAfx.h"
#include "EntityControlPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QTimer>
#include "ByyEntityOperator.h"

EntityControlPanel::EntityControlPanel(ByyEntityOperator* op, QWidget* parent)
    : QWidget(parent), m_operator(op) {
    btnW = new QPushButton("W", this);
    btnA = new QPushButton("A", this);
    btnS = new QPushButton("S", this);
    btnD = new QPushButton("D", this);
    btnW->setFixedSize(40, 40);
    btnA->setFixedSize(40, 40);
    btnS->setFixedSize(40, 40);
    btnD->setFixedSize(40, 40);
    QVBoxLayout* vLayout = new QVBoxLayout;
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(btnA);
    hLayout->addWidget(btnS);
    hLayout->addWidget(btnD);
    vLayout->addWidget(btnW, 0, Qt::AlignHCenter);
    vLayout->addLayout(hLayout);
    setLayout(vLayout);
    connect(btnW, SIGNAL(pressed()), this, SLOT(onWClicked()));
    connect(btnA, SIGNAL(pressed()), this, SLOT(onAClicked()));
    connect(btnS, SIGNAL(pressed()), this, SLOT(onSClicked()));
    connect(btnD, SIGNAL(pressed()), this, SLOT(onDClicked()));
    connect(btnW, SIGNAL(released()), this, SLOT(onWClicked()));
    connect(btnA, SIGNAL(released()), this, SLOT(onAClicked()));
    connect(btnS, SIGNAL(released()), this, SLOT(onSClicked()));
    connect(btnD, SIGNAL(released()), this, SLOT(onDClicked()));

    // 设置焦点策略，使面板能接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
}

EntityControlPanel::~EntityControlPanel() {}

void EntityControlPanel::setButtonState(QPushButton* btn, bool pressed) {
    if (pressed) {
        btn->setStyleSheet("background-color: #87ceeb;");
    } else {
        btn->setStyleSheet("");
    }
}

void EntityControlPanel::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    if (!m_operator || !m_operator->getSelectionEntity()) {
        QWidget::keyPressEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_W:
        setButtonState(btnW, true);
        if (m_operator) { m_operator->mKeyW = true; }
        break;
    case Qt::Key_A:
        setButtonState(btnA, true);
        if (m_operator) { m_operator->mKeyA = true; }
        break;
    case Qt::Key_S:
        setButtonState(btnS, true);
        if (m_operator) { m_operator->mKeyS = true; }
        break;
    case Qt::Key_D:
        setButtonState(btnD, true);
        if (m_operator) { m_operator->mKeyD = true; }
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
    
    if (m_operator) {
        m_operator->updateMovement();
    }
}

void EntityControlPanel::keyReleaseEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        event->ignore();
        return;
    }

    if (!m_operator || !m_operator->getSelectionEntity()) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    switch (event->key()) {
    case Qt::Key_W:
        setButtonState(btnW, false);
        if (m_operator) { m_operator->mKeyW = false; }
        break;
    case Qt::Key_A:
        setButtonState(btnA, false);
        if (m_operator) { m_operator->mKeyA = false; }
        break;
    case Qt::Key_S:
        setButtonState(btnS, false);
        if (m_operator) { m_operator->mKeyS = false; }
        break;
    case Qt::Key_D:
        setButtonState(btnD, false);
        if (m_operator) { m_operator->mKeyD = false; }
        break;
    default:
        QWidget::keyReleaseEvent(event);
        return;
    }
    
    if (m_operator) {
        m_operator->updateMovement();
    }
}

void EntityControlPanel::onWClicked() {
    bool pressed = btnW->isDown();
    setButtonState(btnW, pressed);
    if (m_operator) { 
        m_operator->mKeyW = pressed; 
        m_operator->updateMovement(); 
    }
}

void EntityControlPanel::onAClicked() {
    bool pressed = btnA->isDown();
    setButtonState(btnA, pressed);
    if (m_operator) { 
        m_operator->mKeyA = pressed; 
        m_operator->updateMovement(); 
    }
}

void EntityControlPanel::onSClicked() {
    bool pressed = btnS->isDown();
    setButtonState(btnS, pressed);
    if (m_operator) { 
        m_operator->mKeyS = pressed; 
        m_operator->updateMovement(); 
    }
}

void EntityControlPanel::onDClicked() {
    bool pressed = btnD->isDown();
    setButtonState(btnD, pressed);
    if (m_operator) { 
        m_operator->mKeyD = pressed; 
        m_operator->updateMovement(); 
    }
} 