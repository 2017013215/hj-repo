#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <osgEarthQt/ViewerWidget>

class ByySceneController;
class QMenu;
class QAction;
class ByyImageLayerWidget;
class ByyElevationLayerWidget;
class ByyRoadWidget;
class ByyModelWidget;
class ByyMaskWidget;

class ByyMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	ByyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ByyMainWindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void newFile();
	void open();
	void closeFile();
	bool save();
	bool saveAs();
	void about();
	void openRecentFile();
	void earthToolModified();

	void addImageLayer();
	void addElevationLayer();
	void addViewpoint();
	void addModel();
	void addRoad();
	void addMask();
	void setHudTitle();

	void addRoadFinished(int result);

	void editModel(bool checked);
	void editVideo(bool checked);
	void editBuildingInfo(bool checked);

private:
	void createWidgets();
	void createActions();
	void createMenus();
	void createToolBars();
	void readSettings();
	void writeSettings();
	bool okToContinue();
	bool loadFile(const QString &fileName);
	bool saveFile(const QString &fileName);
	void setCurrentFile(const QString &fileName);
	void updateRecentFileActions();
	QString strippedName(const QString &fullFileName);
	void enableWidgets(bool enable);

private:
	osgEarth::QtGui::ViewerWidget* viewerWidget;
	ByyImageLayerWidget* imageLayerWidget;
	ByyElevationLayerWidget *elevationLayerWidget;
	//ByyViewpointWidget *viewpointWidget;
	ByyRoadWidget *roadsWidget;
	ByyModelWidget *modelWidget;
	ByyMaskWidget *maskWidget;
	ByySceneController *sceneController;
	QString curFile;
	QStringList recentFiles;

	QDockWidget *modelDockWidgt;

	enum { MaxRecentFiles = 5 };
	QAction *recentFileActions[MaxRecentFiles];
	QAction *separatorAction;

	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *toolBarMenu;
	QMenu *projectMenu;
	QMenu *helpMenu;
	QToolBar *fileToolBar;
	QToolBar *projectToolBar;
	QAction *newAction;
	QAction *openAction;
	QAction *closeAction;
	QAction *saveAction;
	QAction *saveAsAction;
	QAction *exitAction;
	QAction *aboutAction;

	QAction *imageAction;
	QAction *elevationAction;
	//QAction *viewpointAction;
	QAction *modelAction;
	QAction *roadAction;
	QAction *maskAction;
	QAction *hudTitleAction;


	QAction* editModelAction;
	QAction* editVideoAction;
	QAction* editBuildingInfoAction;

	QString lastFile;
};

#endif // MAINWINDOW_H
