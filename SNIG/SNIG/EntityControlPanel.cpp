#include "StdAfx.h"
#include "EntityControlPanel.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QTimer>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include "ByyEntityOperator.h"
#include "enums.h"
#include "ByyDescribeEntry.h"
#include "remoteEntity.h"
#include "ByySensorSR.h"
#include "ByyApp.h"
#include "ByyHdose.h"
#include "simuController.h"
#include "SensorPowerButton.h"
#include "sceneEntity.h"
#include "sensorVisualizer.h"
#include "ByyRangeOfVisibilityVisualizer.h"

EntityControlPanel::EntityControlPanel(ByyEntityOperator* op, QWidget* parent)
    : QWidget(parent), m_operator(op), m_lastUpdateTime(0) {
    // 创建参数显示控件
    QGridLayout* gridLayout = new QGridLayout;
    gridLayout->setSpacing(3);

    // 名称
    QHBoxLayout* nameLayout = new QHBoxLayout;
    QLabel* nameLabel = new QLabel(QString::fromLocal8Bit("名称"), this);
    nameEdit = new QLineEdit(this);
    nameEdit->setAlignment(Qt::AlignCenter);
    nameEdit->setReadOnly(true);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    // 兵力
    QHBoxLayout* forceLayout = new QHBoxLayout;
    QLabel* forceLabel = new QLabel(QString::fromLocal8Bit("兵力"), this);
    forceEdit = new QLineEdit(this);
    forceEdit->setAlignment(Qt::AlignCenter);
    forceEdit->setReadOnly(true);
    forceLayout->addWidget(forceLabel);
    forceLayout->addWidget(forceEdit);

    // 上级
    QHBoxLayout* superiorLayout = new QHBoxLayout;
    QLabel* superiorLabel = new QLabel(QString::fromLocal8Bit("上级"), this);
    superiorEdit = new QLineEdit(this);
    superiorEdit->setAlignment(Qt::AlignCenter);
    superiorEdit->setReadOnly(true);
    superiorLayout->addWidget(superiorLabel);
    superiorLayout->addWidget(superiorEdit);

    // 航向
    QHBoxLayout* headingLayout = new QHBoxLayout;
    QLabel* headingLabel = new QLabel(QString::fromLocal8Bit("航向"), this);
    headingEdit = new QLineEdit(this);
    headingEdit->setAlignment(Qt::AlignCenter);
    headingEdit->setReadOnly(true);
    headingLayout->addWidget(headingLabel);
    headingLayout->addWidget(headingEdit);

    // 位置
    QHBoxLayout* positionLayout = new QHBoxLayout;
    QLabel* positionLabel = new QLabel(QString::fromLocal8Bit("位置"), this);
    positionEdit = new QLineEdit(this);
    positionEdit->setAlignment(Qt::AlignCenter);
    positionEdit->setReadOnly(true);
    positionLayout->addWidget(positionLabel);
    positionLayout->addWidget(positionEdit);

    // 高度
    QHBoxLayout* altitudeLayout = new QHBoxLayout;
    QLabel* altitudeLabel = new QLabel(QString::fromLocal8Bit("高度"), this);
    altitudeEdit = new QLineEdit(this);
    altitudeEdit->setAlignment(Qt::AlignCenter);
    altitudeEdit->setReadOnly(true);
    altitudeLayout->addWidget(altitudeLabel);
    altitudeLayout->addWidget(altitudeEdit);

    // 速度
    QHBoxLayout* speedLayout = new QHBoxLayout;
    QLabel* speedLabel = new QLabel(QString::fromLocal8Bit("速度"), this);
    speedEdit = new QLineEdit(this);
    speedEdit->setAlignment(Qt::AlignCenter);
    speedEdit->setReadOnly(true);
    speedLayout->addWidget(speedLabel);
    speedLayout->addWidget(speedEdit);

    // 添加到网格布局
    gridLayout->addLayout(nameLayout, 0, 0);
    gridLayout->addLayout(forceLayout, 0, 1);
    gridLayout->addLayout(superiorLayout, 1, 0);
    gridLayout->addLayout(headingLayout, 1, 1);
    gridLayout->addLayout(positionLayout, 2, 0);
    gridLayout->addLayout(altitudeLayout, 2, 1);
    gridLayout->addLayout(speedLayout, 3, 0);

    // 创建方向控制按钮
    btnW = new QPushButton("W", this);
    btnA = new QPushButton("A", this);
    btnS = new QPushButton("S", this);
    btnD = new QPushButton("D", this);
    btnW->setFixedSize(40, 40);
    btnA->setFixedSize(40, 40);
    btnS->setFixedSize(40, 40);
    btnD->setFixedSize(40, 40);

    // 创建传感器表格
    sensorTableWidget = new QTableWidget(this);
    sensorTableWidget->setColumnCount(3);
    QStringList headers;
    headers << QString::fromLocal8Bit("传感器") 
            << QString::fromLocal8Bit("操作")
            << QString::fromLocal8Bit("显示");
    sensorTableWidget->setHorizontalHeaderLabels(headers);
    sensorTableWidget->setFrameShape(QFrame::NoFrame);
    sensorTableWidget->horizontalHeader()->setStretchLastSection(true);
    sensorTableWidget->verticalHeader()->setVisible(false);
    sensorTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    sensorTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(sensorTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), 
            this, SLOT(onSensorItemClicked(QTableWidgetItem*)));

    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(1, 1, 1, 1);
    mainLayout->addLayout(gridLayout);

    QVBoxLayout* vLayout = new QVBoxLayout;
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(btnA);
    hLayout->addWidget(btnS);
    hLayout->addWidget(btnD);
    vLayout->addWidget(btnW, 0, Qt::AlignHCenter);
    vLayout->addLayout(hLayout);
    
    mainLayout->addLayout(vLayout);
    mainLayout->addWidget(sensorTableWidget);
    setLayout(mainLayout);

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

    // 创建更新定时器
    QTimer* updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateParam()));
    updateTimer->start(200); // 200ms更新一次

    // 初始化传感器表格
    updateSensorTable();
}

EntityControlPanel::~EntityControlPanel() {}

void EntityControlPanel::setButtonState(QPushButton* btn, bool pressed) {
    if (pressed) {
        btn->setStyleSheet("background-color: #87ceeb;");
    } else {
        btn->setStyleSheet("");
    }
}

void EntityControlPanel::onSensorPowerClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString device = btn->property("device").toString();
    ByySensorSR* sensorSR = (ByySensorSR*)btn->property("sensorSR").value<void*>();
    ByyEntityObject* entity = m_operator->getSelectionEntity();
    if (!sensorSR || !entity || !m_operator) return;

    bool newState = sensorSR->switchPower(device);
    btn->setText(QString::fromLocal8Bit(newState ? "关机" : "开机"));

    // 发送传感器操作消息
    m_operator->myApp->hdose().simuController().sendSenorOper(entity->id(), device, newState);
}

void EntityControlPanel::updateSensorTable() {
    // 清空表格
    while (sensorTableWidget->rowCount()) {
        sensorTableWidget->removeRow(0);
    }

    if (!m_operator || !m_operator->getSelectionEntity()) {
        return;
    }

    ByyEntityObject* entity = m_operator->getSelectionEntity();
    sensorTableWidget->setColumnHidden(1, entity->isLocal());

    ByyDescribeEntry* descEntry = entity->describeEntry();
    if (!descEntry) {
        return;
    }

    const ByyDescribeEntry::ComponentList& components = descEntry->getComponent("sensor");
    ByyRemoteEntity* remoteEnt = 0;
    ByySensorSR* sensorSR = 0;

    if (!entity->isLocal()) {
        remoteEnt = dynamic_cast<ByyRemoteEntity*>(entity);
        if (remoteEnt) {
            sensorSR = remoteEnt->getOrCreateSR<ByySensorSR>();
        }
    }

    for (int i = 0; i < components.size(); ++i) {
        int row = i;
        sensorTableWidget->insertRow(row);

        QTableWidgetItem* item = new QTableWidgetItem;
        item->setText(components[i].getTag());
        item->setTextAlignment(Qt::AlignCenter);

        if (row % 2 != 0) {
            item->setBackground(Qt::SolidPattern);
            item->setBackgroundColor(QColor(249,249,249));
        }

        sensorTableWidget->setItem(row, 0, item);

        if (sensorSR) {
            QString powerText = QString::fromLocal8Bit(sensorSR->getDevicePower(components[i].getTag()) ? "关机" : "开机");
            QPushButton* btn = new QPushButton(powerText, this);
            btn->setProperty("device", components[i].getTag());
            btn->setProperty("sensorSR", qVariantFromValue((void*)sensorSR));
            connect(btn, SIGNAL(clicked()), this, SLOT(onSensorPowerClicked()));
            sensorTableWidget->setCellWidget(row, 1, btn);
        }

        item = new QTableWidgetItem;
        item->setTextAlignment(Qt::AlignCenter);
        item->setCheckState(Qt::Unchecked);
        sensorTableWidget->setItem(row, 2, item);
    }

    sensorTableWidget->resizeColumnsToContents();
}

void EntityControlPanel::onSensorItemClicked(QTableWidgetItem* item) {
    if (!item || item->column() != 2 || !m_operator || !m_operator->getSelectionEntity()) {
        return;
    }

    QString tag = sensorTableWidget->item(item->row(), 0)->text();
    ByyEntityObject* entity = m_operator->getSelectionEntity();
    if (!entity) return;

    ByySceneEntity* sceneObj = m_operator->myApp->ig().sceneObjectManager()->findEntity(entity);
    if (!sceneObj) return;

    QList<ByyStateVisualizer*>& stateVisualizers = sceneObj->getStateVisualizerList();
    bool isVisible = item->checkState() == Qt::Checked;

    // 设置传感器可视化器的可见性
    for (int i = 0; i < stateVisualizers.size(); ++i) {
        ByySensorVisualizer* sensorVisualizer = dynamic_cast<ByySensorVisualizer*>(stateVisualizers[i]);
        if (!sensorVisualizer) continue;

        if (sensorVisualizer->getSensorTag() == tag) {
            sensorVisualizer->setVisible(isVisible);
            break;
        }
    }

    // 设置可视范围可视化器的可见性
    for (int i = 0; i < stateVisualizers.size(); ++i) {
        ByyRangeOfVisibilityVisualizer* rangeVisualizer = 
            dynamic_cast<ByyRangeOfVisibilityVisualizer*>(stateVisualizers[i]);
        if (!rangeVisualizer) continue;

        if (rangeVisualizer->getSensorTag() == tag) {
            rangeVisualizer->setVisible(isVisible);
            break;
        }
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

void EntityControlPanel::updateParam() {
    if (!m_operator || !m_operator->getSelectionEntity() || !isVisible() || 
        osg::Timer::instance()->time_s() - m_lastUpdateTime < 0.2) {
        return;
    }

    m_lastUpdateTime = osg::Timer::instance()->time_s();
    ByyEntityObject* entity = m_operator->getSelectionEntity();

    nameEdit->setText(entity->name());
    forceEdit->setText(forceTypeString(entity->forceType()));
    superiorEdit->setText(entity->superiorName());

    osg::Vec3d pos = entity->position();
    positionEdit->setText(QString("%1,%2").arg(pos.x(), 0, 'f', 4).arg(pos.y(), 0, 'f', 4));
    altitudeEdit->setText(QString::number(pos.z(), 'f', 2));

    osg::Vec3d ori = entity->orientation();
    headingEdit->setText(QString::number(ori.x(), 'f', 2));
    speedEdit->setText(QString::number(entity->speed(), 'f', 2));
} 