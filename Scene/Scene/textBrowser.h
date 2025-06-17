#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <QWidget>

class QLabel;

class ByyTextBrowser : public QWidget
{
public:
	ByyTextBrowser(QWidget *parent=NULL);
	~ByyTextBrowser();

	void setText(const QString &text);
	void setColor(const QColor &color);

protected:
	void paintEvent(QPaintEvent *event);

private:
	QColor d_color;
	QLabel *label;

};

#endif // TEXTBROWSER_H
