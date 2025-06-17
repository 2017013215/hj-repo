#ifndef AUTOHIDEPANELITEM_H
#define AUTOHIDEPANELITEM_H

#include <QWidget>

class ByyAutoHidePanelItem : public QWidget
{
	Q_OBJECT

public:
	ByyAutoHidePanelItem(QWidget *parent=0);
	~ByyAutoHidePanelItem();

	void setIcon(const QIcon& icon);
	void setText(const QString& text);

public slots:
	void setChecked(bool checked=false);

protected:
	void mouseReleaseEvent(QMouseEvent *e);
	void paintEvent(QPaintEvent *e);
	void refreshSize();

signals:
	void clicked(bool checked);

private:
	QIcon myIcon;
	QString myText;
	bool myChecked;
	QLinearGradient myCheckedGradient;
};

#endif // AUTOHIDEPANELITEM_H
