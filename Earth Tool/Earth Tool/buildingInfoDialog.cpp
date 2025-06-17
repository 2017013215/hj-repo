#include "stdafx.h"
#include "buildingInfoDialog.h"
#include "model.h"
#include "textedit.h"

ByyBuildingInfoDialog::ByyBuildingInfoDialog( QWidget *parent /*= 0*/ ) 
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.label_15->installEventFilter(this);

	connect(ui.lineEdit,SIGNAL(textChanged(const QString&)),SLOT(setTitle(const QString&)));
	connect(ui.lineEdit_5,SIGNAL(textChanged(const QString&)),SLOT(setWindowSize()));
	connect(ui.lineEdit_6,SIGNAL(textChanged(const QString&)),SLOT(setWindowSize()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(changeBackgroundColor()));
}

ByyBuildingInfoDialog::~ByyBuildingInfoDialog()
{

}

void ByyBuildingInfoDialog::setCurrentModel( ByyModel* model )
{
	d_model=model;

	ui.lineEdit->blockSignals(true);
	ui.label_15->blockSignals(true);
	ui.lineEdit_5->blockSignals(true);
	ui.lineEdit_6->blockSignals(true);

	ui.lineEdit->setText(model->title());
	ui.label_15->setText(model->content());
	ui.lineEdit_5->setText(QString::number(model->windowWidth()));
	ui.lineEdit_6->setText(QString::number(model->windowHeight()));

	updateColor();

	ui.lineEdit->blockSignals(false);
	ui.label_15->blockSignals(false);
	ui.lineEdit_5->blockSignals(false);
	ui.lineEdit_6->blockSignals(false);
}

void ByyBuildingInfoDialog::setTitle( const QString& title )
{
	d_model->setTitle(title);
	emit modified();
}

void ByyBuildingInfoDialog::setContent( const QString& content )
{
	d_model->setContent(content);
	emit modified();
}

void ByyBuildingInfoDialog::setWindowSize()
{
	int _width=ui.lineEdit_5->text().toInt();

	if (_width==0)
	{
		_width=400;
	}

	int _height=ui.lineEdit_6->text().toInt();

	if (_height==0)
	{
		_height=300;
	}


	d_model->setWindowSize(ui.lineEdit_5->text().toInt(),ui.lineEdit_6->text().toInt());
	emit modified();
}

bool ByyBuildingInfoDialog::eventFilter( QObject *obj, QEvent *event )
{
	if (obj==ui.label_15&&event->type()==QEvent::MouseButtonDblClick)
	{
		TextEdit *textEdit=new TextEdit(ui.label_15->text(),0,Qt::WindowSystemMenuHint|Qt::WindowStaysOnTopHint);

		connect(textEdit,SIGNAL(editFinish()),SLOT(contextEditFinished()));

		setEnabled(false);

		textEdit->show();

		return true;
	}
	else return QWidget::eventFilter(obj,event);
}

void ByyBuildingInfoDialog::contextEditFinished()
{
	TextEdit *edit=static_cast<TextEdit*>(sender());
	if (edit->isChanged())
	{
		ui.label_15->setText(edit->getText());
		d_model->setContent(edit->getText());
		emit modified();
	}
	edit->deleteLater();
	setEnabled(true);
}

void ByyBuildingInfoDialog::changeBackgroundColor()
{

	QColor color=QColorDialog::getColor(/*QColor(ui.pushButton_2->text())*/d_model->backgroundColor(),this,QString::fromLocal8Bit("Ñ¡Ôñ±³¾°É«"),QColorDialog::ShowAlphaChannel);

	if(color.isValid())
	{
		d_model->setBackgroundColor(color);

		updateColor();
		emit modified();
	}
}

void ByyBuildingInfoDialog::updateColor()
{
	ui.pushButton_2->setText(/*d_model->backgroundColor().name()*/d_model->backgroundColorName());

	QString _styleSheet=QString("background-color: rgba(%1, %2, %3, %4);").arg(d_model->backgroundColor().red())
		                                                                  .arg(d_model->backgroundColor().green())
																		  .arg (d_model->backgroundColor().blue())
																		  .arg(d_model->backgroundColor().alpha());

	ui.label_15->setStyleSheet(_styleSheet);
	
	ui.pushButton_2->setStyleSheet(_styleSheet);
}
