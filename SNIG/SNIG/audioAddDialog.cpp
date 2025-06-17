#include "stdafx.h"
#include "audioAddDialog.h"

ByyAudioAddDialog::ByyAudioAddDialog(const QString& entitytype,const QString& filename,QWidget *parent)
	: QDialog(parent,Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);

	ui.lineEdit->setText(entitytype);
	ui.lineEdit_2->setText(filename);

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(browserFile()));
}

ByyAudioAddDialog::~ByyAudioAddDialog()
{

}

void ByyAudioAddDialog::browserFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Ñ¡ÔñÎÄ¼þ"),
		"../data/audio/Sounds",
		tr("Sound File (*.wav)"));

	if (!fileName.isEmpty())
	{
		ui.lineEdit_2->setText(QDir().relativeFilePath(fileName));
		ui.pushButton_2->setEnabled(true);
	}
}

QString ByyAudioAddDialog::getEntityType()
{
	return ui.lineEdit->text();
}

QString ByyAudioAddDialog::getFileName()
{
	return ui.lineEdit_2->text();
}
