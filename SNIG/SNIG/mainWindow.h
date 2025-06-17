#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../snigCore/settings.h"

class ByyApp;
class ByyModel;
class ByyTerrainEffectWidget;
class ByyEnvironmentWidget;
class ByyTerrainEffectWidget;
class ByyScreenRecordWidget;
class ByyAutoHidePanel;
class QTabWidget;

class ByyMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	ByyMainWindow(ByyApp& app,QWidget *parent = 0, Qt::WFlags flags = 0);

	~ByyMainWindow();

	void showEnviromentWidget();
	void showTerrainEffectWidget();
	void showScreenRecordWidget();

	void showViewpointCreateWidget();
	void showAnimationPathCreateWidget();
	void showTerrainEditWidget();

	void showFullScreen(bool fullScreen);

	void updateRecentFileActions();
	void addRecentFile(const QString& filename);
	
	//…Ë÷√—˘ Ω
	void setQss();

	QAction* closeScnAction;

protected slots:
	void currentSandboxChanged(bool show,const QString& desc);

	//void createGraphicsContext(bool quadBuffer);
	void changeStereo(bool stereo);

	void onScenarioChanged();

	void openRecentFile();
	void onHdoseStatusChanged(int status);

protected:
	bool eventFilter( QObject * object, QEvent * event );
	void closeEvent(QCloseEvent *event);
	virtual	bool event(QEvent* e);

private:
	void createMenuToolBars();
	void createDockWidgets();
	void cratteVerdictDockWidgets();

	QWidget* createViewWidget(osgViewer::View* view,bool quadbuffer);

signals:

private:
	ByyApp& myApp;

	QPointer<ByyEnvironmentWidget> myEnvironmentWidget;
	QPointer<ByyTerrainEffectWidget> myTerrainEffectWidget;
	QPointer<ByyScreenRecordWidget> myScreenRecordWidget;

	ByyAutoHidePanel* myAutoHidePanel;

	QWidget *central3d;

	QMenu	*recentMenu;
	QMenu   *m_verdictMenu;

	QAction *recentFileActs[MAX_REC_FILES_SIZE];
	QTabWidget* centralTab;
};

#endif // MAINWINDOW_H
