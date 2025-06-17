#pragma once
#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include "scene_global.h"
class SCENE_EXPORT GridSetDlg : public QDialog
{
	Q_OBJECT
public:
	GridSetDlg(void);
	~GridSetDlg(void);

	static void	doSetting();
public slots:
	void	ok();
protected:
	QCheckBox*	gridOpen;
	QCheckBox*	clampToTerrain;
	QLineEdit*	openScale;
	QLineEdit*	lonStart;
	QLineEdit*	lonEnd;
	QLineEdit*	latStart;
	QLineEdit*	latEnd;
	QLineEdit*	resolution;
};

