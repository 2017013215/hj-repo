#pragma once
#include <QWidget>
#include <QPushButton>

class ByyEntityOperator;

class EntityControlPanel : public QWidget {
    Q_OBJECT
public:
    EntityControlPanel(ByyEntityOperator* op, QWidget* parent = 0);
    ~EntityControlPanel();

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    void setButtonState(QPushButton* btn, bool pressed);
    QPushButton *btnW, *btnA, *btnS, *btnD;
    ByyEntityOperator* m_operator;

private slots:
    void onWClicked();
    void onAClicked();
    void onSClicked();
    void onDClicked();
}; 