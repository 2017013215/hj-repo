#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class ByyEntityOperator;
class QTableWidget;
class ByySensorSR;
class ByyEntityObject;
class QTableWidgetItem;

class EntityControlPanel : public QWidget {
    Q_OBJECT
public:
    EntityControlPanel(ByyEntityOperator* op, QWidget* parent = 0);
    ~EntityControlPanel();

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void updateSensorTable();

private:
    void setButtonState(QPushButton* btn, bool pressed);
    QPushButton *btnW, *btnA, *btnS, *btnD;
    ByyEntityOperator* m_operator;
    QTableWidget* sensorTableWidget;

    // 参数显示控件
    QLineEdit* nameEdit;
    QLineEdit* forceEdit;
    QLineEdit* superiorEdit;
    QLineEdit* positionEdit;
    QLineEdit* altitudeEdit;
    QLineEdit* headingEdit;
    QLineEdit* speedEdit;

    double m_lastUpdateTime;

private slots:
    void onWClicked();
    void onAClicked();
    void onSClicked();
    void onDClicked();
    void onSensorItemClicked(QTableWidgetItem* item);
    void onSensorPowerClicked();
    void updateParam();
}; 