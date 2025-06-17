#include "StdAfx.h"
#include "hdoseSettingsWidget.h"
#include "hdoseSettings.h"


ByyHdoseSettingsWidget::ByyHdoseSettingsWidget(QWidget *parent)
	: QWidget(parent,Qt::Window)
{
	ui.setupUi(this);

	ui.createDirEdit->setText(ByyHdoseSettings::instance().getCreateWorkDir());
	ui.createSimFileEdit->setText(ByyHdoseSettings::instance().getCreateSimFile());

	ui.joinDirEdit->setText(ByyHdoseSettings::instance().getJoinWorkDir());
	ui.joinSimFileEdit->setText(ByyHdoseSettings::instance().getJoinSimFile());

	ui.singleDirEdit->setText(ByyHdoseSettings::instance().getSingleWorkDir());
	ui.singleSimFileEdit->setText(ByyHdoseSettings::instance().getSingleSimFile());

	ui.opdFileEdit->setText(ByyHdoseSettings::instance().getOpdFile());
	ui.rsmFileEdit->setText(ByyHdoseSettings::instance().getRsmFile());

	connect(ui.createDirButton,SIGNAL(clicked()),SLOT(onCreateSelectDir()));
	connect(ui.createSimFileButton,SIGNAL(clicked()),SLOT(onCreateSelectSimFile()));

	connect(ui.joinDirButton,SIGNAL(clicked()),SLOT(onJoinSelectDir()));
	connect(ui.joinSimFileButton,SIGNAL(clicked()),SLOT(onJoinSelectSimFile()));

	connect(ui.singleDirButton,SIGNAL(clicked()),SLOT(onSingleSlectDir()));
	connect(ui.singleSimFileButton,SIGNAL(clicked()),SLOT(onSingleSelectSimFile()));

	connect(ui.opdFileButton,SIGNAL(clicked()),SLOT(onOpdSelectFile()));
	connect(ui.rsmFileButton,SIGNAL(clicked()),SLOT(onRsmSelectFile()));

	connect(ui.okButton,SIGNAL(clicked()),SLOT(onOkButtonClicked()));
	connect(ui.cancelButton,SIGNAL(clicked()),SLOT(close()));
}

ByyHdoseSettingsWidget::~ByyHdoseSettingsWidget()
{

}

void ByyHdoseSettingsWidget::onCreateSelectDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("工作目录"),
		ui.createDirEdit->text(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

	if (dir.isEmpty())
		return;

	QDir rootDir(".");

	if (dir.contains(QDir::toNativeSeparators(rootDir.absolutePath())))
	{
		dir=rootDir.relativeFilePath(dir);

		if (dir.isEmpty())
		{
			dir=".";
		}
		else
		{
			dir.push_front("./");
			dir.push_back("/");
		}
	}

	ui.createDirEdit->setText(dir);
}
void ByyHdoseSettingsWidget::onCreateSelectSimFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		ui.createDirEdit->text(),
		tr("Sim (*.sim)"));

	if (fileName.isEmpty())
		return;

	QDir rootDir(ui.joinDirEdit->text());

	if (rootDir.exists(fileName))
	{
		fileName=rootDir.relativeFilePath(fileName);
	}

	ui.createSimFileEdit->setText(fileName);
}

void ByyHdoseSettingsWidget::onJoinSelectDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("工作目录"),
		ui.joinDirEdit->text(),QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

	if (dir.isEmpty())
		return;

	QDir rootDir(".");

	if (dir.contains(QDir::toNativeSeparators(rootDir.absolutePath())))
	{
		dir=rootDir.relativeFilePath(dir);

		if (dir.isEmpty())
		{
			dir=".";
		}
		else
		{
			dir.push_front("./");
			dir.push_back("/");
		}
	}

	ui.joinDirEdit->setText(dir);
}

void ByyHdoseSettingsWidget::onJoinSelectSimFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		ui.joinDirEdit->text(),
		tr("Sim (*.sim)"));

	if (fileName.isEmpty())
		return;

	QDir rootDir(ui.joinDirEdit->text());

	if (rootDir.exists(fileName))
	{
		fileName=rootDir.relativeFilePath(fileName);
	}

	ui.joinSimFileEdit->setText(fileName);
}

void ByyHdoseSettingsWidget::onSingleSlectDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("工作目录"),
		"./hdose/dj",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

	if (dir.isEmpty())
		return;

	QDir rootDir(".");

	if (dir.contains(QDir::toNativeSeparators(rootDir.absolutePath())))
	{
		dir=rootDir.relativeFilePath(dir);

		if (dir.isEmpty())
		{
			dir=".";
		}
		else
		{
			dir.push_front("./");
			dir.push_back("/");
		}
	}

	ui.singleDirEdit->setText(dir);
}

void ByyHdoseSettingsWidget::onSingleSelectSimFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		ui.singleDirEdit->text(),
		tr("Sim (*.sim)"));

	if (fileName.isEmpty())
		return;

	QDir rootDir(ui.singleDirEdit->text());

	if (rootDir.exists(fileName))
	{
		fileName=rootDir.relativeFilePath(fileName);
	}

	ui.singleSimFileEdit->setText(fileName);
}

void ByyHdoseSettingsWidget::onOpdSelectFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		ui.opdFileEdit->text(),
		tr("Opd File(*.opd)"));

	if (fileName.isEmpty())
		return;

	QDir rootDir(".");

	if (rootDir.exists(fileName))
	{
		fileName=rootDir.relativeFilePath(fileName);
	}

	ui.opdFileEdit->setText(fileName);
}

void ByyHdoseSettingsWidget::onRsmSelectFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择文件"),
		ui.opdFileEdit->text(),
		tr("Rsm File(*.rsm)"));

	if (fileName.isEmpty())
		return;

	QDir rootDir(".");

	if (rootDir.exists(fileName))
	{
		fileName=rootDir.relativeFilePath(fileName);
	}

	ui.rsmFileEdit->setText(fileName);
}

void ByyHdoseSettingsWidget::onOkButtonClicked()
{
	ByyHdoseSettings::instance().setCreateWorkDir(ui.createDirEdit->text());
	ByyHdoseSettings::instance().setCreateSimFile(ui.createSimFileEdit->text());

	ByyHdoseSettings::instance().setJoinWorkDir(ui.joinDirEdit->text());
	ByyHdoseSettings::instance().setJoinSimFile(ui.joinSimFileEdit->text());

	ByyHdoseSettings::instance().setSingleWorkDir(ui.singleDirEdit->text());
	ByyHdoseSettings::instance().setSingleSimFile(ui.singleSimFileEdit->text());

	ByyHdoseSettings::instance().setOpdFile(ui.opdFileEdit->text());
	ByyHdoseSettings::instance().setRsmFile(ui.rsmFileEdit->text());

	close();
}
