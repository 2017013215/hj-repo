#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class ByyApp;
class ByyModel;
class ByyTerrainEffectWidget;
class ByyEnvironmentWidget;
class ByyTerrainEffectWidget;
class ByyScreenRecordWidget;
class ByyAutoHidePanel;

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

protected slots:
	void currentSandboxChanged(bool show,const QString& desc);

	//void createGraphicsContext(bool quadBuffer);
	void changeStereo(bool stereo);

	void onScenarioChanged();

	void openRecentFile();

protected:
	bool eventFilter( QObject * object, QEvent * event );
	void closeEvent(QCloseEvent *event);

	int SbInit();

	bool winEvent(MSG *message, long *result);

private:
	void createMenuToolBars();
	void createDockWidgets();

	QWidget* createViewWidget(osgViewer::View* view,bool quadbuffer);

signals:

private:
	ByyApp& myApp;

	QPointer<ByyEnvironmentWidget> myEnvironmentWidget;
	QPointer<ByyTerrainEffectWidget> myTerrainEffectWidget;
	QPointer<ByyScreenRecordWidget> myScreenRecordWidget;

	ByyAutoHidePanel* myAutoHidePanel;


	enum { MaxRecentFiles = 5 };
	QMenu	*recentMenu;
	QAction *recentFileActs[MaxRecentFiles];
};

#endif // MAINWINDOW_H
