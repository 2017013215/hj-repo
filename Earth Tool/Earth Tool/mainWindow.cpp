#include "stdafx.h"
#include "mainWindow.h"
#include "sceneController.h"
#include "imageLayerWidget.h"
#include "elevationLayerWidget.h"
#include "waitDialog.h"
#include "roadWidget.h"
#include "modelWidget.h"
#include "maskWidget.h"
#include "addPathDialog.h"
#include "hudTitleDialog.h"



ByyMainWindow::ByyMainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	sceneController=new ByySceneController;

	createActions();
	createMenus();
	createToolBars();
	createWidgets();

	readSettings();
	setWindowIcon(QIcon("../data/icons/earthtool/earthtool.png"));
	setWindowTitle("Earth Tool");
}

ByyMainWindow::~ByyMainWindow()
{
	delete sceneController;
}

void ByyMainWindow::closeEvent( QCloseEvent *event )
{
	if (okToContinue()) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

void ByyMainWindow::newFile()
{
	if (okToContinue()) {
		loadFile("../data/earthfile/default.earth");
		setCurrentFile("");
	}
}

void ByyMainWindow::open()
{
	if (okToContinue()) {
		QString fileName = QFileDialog::getOpenFileName(this,
			"Earth Tool", "../data/earthfile",
			tr("Earth files (*.earth)"));
		if (!fileName.isEmpty())
		{
			ByyWaitDialog msg(QString::fromLocal8Bit("加载.earth文件,请等待..."));
			msg.show();
			msg.repaint();
			loadFile(fileName);
			msg.hide();
		}
	}
}

void ByyMainWindow::closeFile()
{
	if (okToContinue()) {
		sceneController->clear();
		enableWidgets(false);
		setWindowTitle("Earth Tool");
		imageLayerWidget->release();
		elevationLayerWidget->release();
//		viewpointWidget->release();
		modelWidget->release();
		roadsWidget->release();
		maskWidget->release();
		setWindowModified(false);
	}
}

bool ByyMainWindow::save()
{
	if (curFile.isEmpty()) {
		return saveAs();
	} else {
		return saveFile(curFile);
	}
}

bool ByyMainWindow::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this,
		"Earth Tool", "./../data/earthFile",
		tr("Earth files (*.earth)"));
	if (fileName.isEmpty())
		return false;

	return saveFile(fileName);
}

void ByyMainWindow::about()
{
	QMessageBox::about(this, QString::fromLocal8Bit("关于Earth Tool"),
		QString::fromLocal8Bit("<h2>Earth Tool 1.1</h2>"
		"<p>Copyright &copy; 2014 Software Inc."
		"<p>Earth Tool 是用于.earth文件 "
		"的编辑工具"));
}

void ByyMainWindow::openRecentFile()
{
	if (okToContinue()) {
		QAction *action = qobject_cast<QAction *>(sender());
		if (action)
			loadFile(action->data().toString());
	}
}

void ByyMainWindow::earthToolModified()
{
	setWindowModified(true);
}

void ByyMainWindow::createWidgets()
{
	
	viewerWidget= new osgEarth::QtGui::ViewerWidget(sceneController->getViewer());
	setCentralWidget(viewerWidget);

	imageLayerWidget=new ByyImageLayerWidget(sceneController,this);
	imageLayerWidget->setEnabled(false);
	connect(imageLayerWidget,SIGNAL(modified()),this,SLOT(earthToolModified()));
	
	QDockWidget *imageLayerDockWidgt=new QDockWidget(QString::fromLocal8Bit("图像"));
	imageLayerDockWidgt->setWidget(imageLayerWidget);
	addDockWidget(Qt::LeftDockWidgetArea,imageLayerDockWidgt);
	viewMenu->addAction(imageLayerDockWidgt->toggleViewAction());
	imageLayerDockWidgt->setVisible(false);

	elevationLayerWidget=new ByyElevationLayerWidget(sceneController,this);
	elevationLayerWidget->setEnabled(false);
	connect(elevationLayerWidget,SIGNAL(modified()),this,SLOT(earthToolModified()));

	QDockWidget *elevationLayerDockWidgt=new QDockWidget(QString::fromLocal8Bit("高程"));
	elevationLayerDockWidgt->setWidget(elevationLayerWidget);
	addDockWidget(Qt::LeftDockWidgetArea,elevationLayerDockWidgt);
	viewMenu->addAction(elevationLayerDockWidgt->toggleViewAction());
	elevationLayerDockWidgt->setVisible(false);

	/*viewpointWidget=new ByyViewpointWidget(sceneController,this);
	viewpointWidget->setEnabled(false);
	connect(viewpointWidget,SIGNAL(modified()),this,SLOT(earthToolModified()));

	QDockWidget *viewpointDockWidgt=new QDockWidget(QString::fromLocal8Bit("视点"));
	viewpointDockWidgt->setWidget(viewpointWidget);
	addDockWidget(Qt::LeftDockWidgetArea,viewpointDockWidgt);
	viewMenu->addAction(viewpointDockWidgt->toggleViewAction());
	viewpointDockWidgt->setVisible(false);*/

	modelWidget=new ByyModelWidget(sceneController);
	modelWidget->setEnabled(false);
	connect(modelWidget,SIGNAL(modified()),this,SLOT(earthToolModified()));
	modelDockWidgt=new QDockWidget(QString::fromLocal8Bit("模型"));
	modelDockWidgt->setWidget(modelWidget);
	addDockWidget(Qt::RightDockWidgetArea,modelDockWidgt);
	//viewMenu->addAction(modelDockWidgt->toggleViewAction());
	modelDockWidgt->setVisible(false);
	viewMenu->addAction(editModelAction);
	viewMenu->addAction(editVideoAction);
	viewMenu->addAction(editBuildingInfoAction);

	roadsWidget=new ByyRoadWidget(sceneController);
	roadsWidget->setEnabled(false);
	connect(roadsWidget,SIGNAL(modified()),this,SLOT(earthToolModified()));
	QDockWidget *roadsDockWidgt=new QDockWidget(QString::fromLocal8Bit("道路"));
	roadsDockWidgt->setWidget(roadsWidget);
	addDockWidget(Qt::RightDockWidgetArea,roadsDockWidgt);
	//viewMenu->addAction(roadsDockWidgt->toggleViewAction());
	roadsDockWidgt->setVisible(false);

	maskWidget=new ByyMaskWidget(sceneController);
	maskWidget->setEnabled(false);
	connect(maskWidget,SIGNAL(modified()),this,SLOT(earthToolModified()));
	QDockWidget *areaDockWidgt=new QDockWidget(QString::fromLocal8Bit("挖洞"));
	areaDockWidgt->setWidget(maskWidget);
	addDockWidget(Qt::RightDockWidgetArea,areaDockWidgt);
	viewMenu->addAction(areaDockWidgt->toggleViewAction());
	areaDockWidgt->setVisible(false);
}

void ByyMainWindow::createActions()
{
	newAction = new QAction(QString::fromLocal8Bit("新建(&N).."), this);
	newAction->setIcon(QIcon("../data/icons/earthtool/new.png"));
	newAction->setShortcut(QKeySequence::New);
	connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	openAction = new QAction(QString::fromLocal8Bit("打开(&O).."), this);
	openAction->setIcon(QIcon("../data/icons/earthtool/open.png"));
	openAction->setShortcut(QKeySequence::Open);
	connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

	closeAction = new QAction(QString::fromLocal8Bit("关闭(&C)"), this);
	closeAction->setShortcut(QKeySequence::Close);
	connect(closeAction, SIGNAL(triggered()), this, SLOT(closeFile()));

	saveAction = new QAction(QString::fromLocal8Bit("保存(&S)"), this);
	saveAction->setIcon(QIcon("../data/icons/earthtool/save.png"));
	saveAction->setShortcut(QKeySequence::Save);
	saveAction->setEnabled(false);
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

	saveAsAction = new QAction(QString::fromLocal8Bit("另存为(&A).."), this);
	saveAsAction->setEnabled(false);
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile()));
	}

	exitAction = new QAction(QString::fromLocal8Bit("退出(&E)"), this);
	exitAction->setShortcut(tr("Ctrl+Q"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	aboutAction = new QAction(QString::fromLocal8Bit("关于(&A)"), this);
	aboutAction->setIcon(QIcon("../data/icons/information.png"));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

	imageAction = new QAction(QString::fromLocal8Bit("添加图像(&I)"), this);
	imageAction->setIcon(QIcon("../data/icons/earthtool/add_image.png"));
	imageAction->setShortcut(QKeySequence("Ctrl+I"));
	imageAction->setEnabled(false);
	connect(imageAction, SIGNAL(triggered()), this, SLOT(addImageLayer()));

	elevationAction = new QAction(QString::fromLocal8Bit("添加高程(&E)"), this);
	elevationAction->setIcon(QIcon("../data/icons/earthtool/add_elevation.png"));
	elevationAction->setShortcut(QKeySequence("Ctrl+E"));
	elevationAction->setEnabled(false);
	connect(elevationAction, SIGNAL(triggered()), this, SLOT(addElevationLayer()));

/*	viewpointAction = new QAction(QString::fromLocal8Bit("添加视点(&V)"), this);
	viewpointAction->setIcon(QIcon("../data/icons/earthtool/add_viewpoint.png"));
	viewpointAction->setShortcut(QKeySequence("Ctrl+V"));
	viewpointAction->setEnabled(false);
	connect(viewpointAction, SIGNAL(triggered()), this, SLOT(addViewpoint()));*/

	modelAction = new QAction(QString::fromLocal8Bit("添加模型(&M)"), this);
	modelAction->setIcon(QIcon("../data/icons/earthtool/add_simple_model.png"));
	modelAction->setShortcut(QKeySequence("Ctrl+M"));
	modelAction->setEnabled(false);
	connect(modelAction, SIGNAL(triggered()), this, SLOT(addModel()));

	roadAction = new QAction(QString::fromLocal8Bit("添加道路(&R)"), this);
	roadAction->setIcon(QIcon("../data/icons/earthtool/Road_Loose-Light.png"));
	roadAction->setShortcut(QKeySequence("Ctrl+R"));
	roadAction->setEnabled(false);
	connect(roadAction, SIGNAL(triggered()), this, SLOT(addRoad()));

	maskAction = new QAction(QString::fromLocal8Bit("添加挖洞(&A)"), this);
	maskAction->setIcon(QIcon("../data/icons/earthtool/shape_handles.png"));
	maskAction->setShortcut(QKeySequence("Ctrl+A"));
	maskAction->setEnabled(false);
	connect(maskAction, SIGNAL(triggered()), this, SLOT(addMask()));

	hudTitleAction = new QAction(QString::fromLocal8Bit("设置标题(&S)"), this);
	hudTitleAction->setIcon(QIcon("../data/icons/earthtool/blackboard.png"));
	hudTitleAction->setShortcut(QKeySequence("Ctrl+S"));
	hudTitleAction->setEnabled(false);
	connect(hudTitleAction, SIGNAL(triggered()), this, SLOT(setHudTitle()));

	editModelAction=new QAction(QString::fromLocal8Bit("模型"),this);
	editModelAction->setCheckable(true);
	connect(editModelAction,SIGNAL(triggered(bool)),SLOT(editModel(bool)));

	editVideoAction=new QAction(QString::fromLocal8Bit("监控"),this);
	editVideoAction->setCheckable(true);
	connect(editVideoAction,SIGNAL(triggered(bool)),SLOT(editVideo(bool)));

	editBuildingInfoAction=new QAction(QString::fromLocal8Bit("信息"),this);
	editBuildingInfoAction->setCheckable(true);
	connect(editBuildingInfoAction,SIGNAL(triggered(bool)),SLOT(editBuildingInfo(bool)));
}

void ByyMainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(QString::fromLocal8Bit("文件(&F)"));
	fileMenu->addAction(newAction);
	fileMenu->addAction(openAction);
	fileMenu->addAction(closeAction);
	fileMenu->addAction(saveAction);
	fileMenu->addAction(saveAsAction);
	separatorAction = fileMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		fileMenu->addAction(recentFileActions[i]);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);

	viewMenu=menuBar()->addMenu(QString::fromLocal8Bit("视图(&V)"));
	toolBarMenu=viewMenu->addMenu(QString::fromLocal8Bit("工具栏"));

	projectMenu=menuBar()->addMenu(QString::fromLocal8Bit("项目(&P)"));
	projectMenu->addAction(imageAction);
	projectMenu->addAction(elevationAction);
//	projectMenu->addAction(viewpointAction);
	projectMenu->addAction(modelAction);
	projectMenu->addAction(roadAction);
	projectMenu->addAction(maskAction);
	projectMenu->addAction(hudTitleAction);

	helpMenu=menuBar()->addMenu(QString::fromLocal8Bit("帮助(&H)"));
	helpMenu->addAction(aboutAction);
}

void ByyMainWindow::createToolBars()
{
	fileToolBar = addToolBar(QString::fromLocal8Bit("文件(&F)"));
	fileToolBar->addAction(newAction);
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(saveAction);
	toolBarMenu->addAction(fileToolBar->toggleViewAction());

	projectToolBar=addToolBar(QString::fromLocal8Bit("项目(&P)"));
	projectToolBar->addAction(imageAction);
	projectToolBar->addAction(elevationAction);
//	projectToolBar->addAction(viewpointAction);
	projectToolBar->addAction(modelAction);
	projectToolBar->addAction(roadAction);
	projectToolBar->addAction(maskAction);
	projectToolBar->addAction(hudTitleAction);
	toolBarMenu->addAction(projectToolBar->toggleViewAction());
}

void ByyMainWindow::readSettings()
{
	QSettings settings("Software Inc.", "EarthTool");

	recentFiles = settings.value("recentFiles").toStringList();
	updateRecentFileActions();
}

void ByyMainWindow::writeSettings()
{
	QSettings settings("Software Inc.", "EarthTool");

	settings.setValue("recentFiles", recentFiles);
}

bool ByyMainWindow::okToContinue()
{
	if (isWindowModified()) {
		int r = QMessageBox::warning(this,"Earth Tool",
			QString::fromLocal8Bit("是否保存修改?"),
			QMessageBox::Yes | QMessageBox::No
			| QMessageBox::Cancel);
		if (r == QMessageBox::Yes) {
			return save();
		} else if (r == QMessageBox::Cancel) {
			return false;
		}
	}
	return true;
}

bool ByyMainWindow::loadFile( const QString &fileName )
{
	if (!sceneController->readFile(fileName)) {
		return false;
	}

	imageLayerWidget->init();
	elevationLayerWidget->init();
//	viewpointWidget->init();
	modelWidget->init();
	roadsWidget->init();
	maskWidget->init();
	setCurrentFile(fileName);
	return true;
}

bool ByyMainWindow::saveFile( const QString &fileName )
{
	modelWidget->writeToMapNode();
	roadsWidget->writeToMapNode();
	maskWidget->writeToMapNode();
	if (!sceneController->writeFile(fileName)) {
		return false;
	}

	setCurrentFile(fileName);
	return true;
}

void ByyMainWindow::setCurrentFile( const QString &fileName )
{
	curFile = fileName;
	setWindowModified(false);

	QString shownName = tr("Untitled");
	if (!curFile.isEmpty()) {
		shownName = strippedName(curFile);
		recentFiles.removeAll(curFile);
		recentFiles.prepend(curFile);
		updateRecentFileActions();
	}

	setWindowTitle(tr("%1[*] - %2").arg(shownName)
		.arg("Earth Tool"));

	enableWidgets(true);
}

void ByyMainWindow::enableWidgets( bool enable )
{
	closeAction->setEnabled(enable);
	saveAction->setEnabled(enable);
	saveAsAction->setEnabled(enable);
	imageAction->setEnabled(enable);
	elevationAction->setEnabled(enable);
//	viewpointAction->setEnabled(enable);
	modelAction->setEnabled(enable);
	roadAction->setEnabled(enable);
	maskAction->setEnabled(enable);
	hudTitleAction->setEnabled(enable);
	imageLayerWidget->setEnabled(enable);
	elevationLayerWidget->setEnabled(enable);
//	viewpointWidget->setEnabled(enable);
	modelWidget->setEnabled(enable);
	roadsWidget->setEnabled(enable);
	maskWidget->setEnabled(enable);
}

void ByyMainWindow::updateRecentFileActions()
{
	QMutableStringListIterator i(recentFiles);
	while (i.hasNext()) {
		if (!QFile::exists(i.next()))
			i.remove();
	}

	for (int j = 0; j < MaxRecentFiles; ++j) {
		if (j < recentFiles.count()) {
			QString text = tr("&%1 %2")
				.arg(j + 1)
				.arg(strippedName(recentFiles[j]));
			recentFileActions[j]->setText(text);
			recentFileActions[j]->setData(recentFiles[j]);
			recentFileActions[j]->setVisible(true);
		} else {
			recentFileActions[j]->setVisible(false);
		}
	}
	separatorAction->setVisible(!recentFiles.isEmpty());
}

QString ByyMainWindow::strippedName( const QString &fullFileName )
{
	return QFileInfo(fullFileName).fileName();
}

void ByyMainWindow::addImageLayer()
{
	 QStringList filePaths = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("添加图像"),lastFile.isEmpty()?"./../data/imagelayer":QFileInfo(lastFile).filePath(), tr("GetTiff Images (*.tif)"));
	 if (!filePaths.isEmpty())
	 {
		 lastFile=filePaths[0];
		 imageLayerWidget->add(filePaths);
	 }
}

void ByyMainWindow::addElevationLayer()
{
	QStringList filePaths = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("添加高程"),lastFile.isEmpty()?"./../data/elevationLayer":QFileInfo(lastFile).filePath(), tr("Elevation File (*.tif *.dt1 *.dt2 *.dt3)"));
	if (!filePaths.isEmpty())
	{
		lastFile=filePaths[0];
		elevationLayerWidget->add(filePaths);
	}
}
void ByyMainWindow::addViewpoint()
{
	/*osgEarth::Viewpoint vp=sceneController->getViewpoint();
	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("添加视点"),
		QString::fromLocal8Bit("视点名称:"), QLineEdit::Normal,
		QString(), &ok);
	if (ok && !text.isEmpty())
		vp.setName(text.toLocal8Bit().data());

//	viewpointWidget->add(vp);

	earthToolModified();*/
}

void ByyMainWindow::addModel()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("打开模型"),
		lastFile.isEmpty()?"../data/models":QFileInfo(lastFile).filePath(),tr("Model (*.flt *.osg *.ive *.osgb *.3ds *.osgt *.osgx)"));

	if (!fileName.isEmpty())
	{
		lastFile=fileName;
		modelWidget->add(fileName);
	}
}

void ByyMainWindow::addRoad()
{
	ByyAddPathDialog *_activeDialog = new ByyAddPathDialog(
		sceneController->getRootNode(), sceneController->getMapNode(),
		sceneController->getViewer());

	connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(addRoadFinished(int)));

	_activeDialog->setWindowTitle(QString::fromLocal8Bit("添加道路"));
	_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
	_activeDialog->show();
}

void ByyMainWindow::addRoadFinished( int result )
{
	ByyAddPathDialog *_activeDialog=dynamic_cast<ByyAddPathDialog*>(sender());

	if(_activeDialog&&result==QDialog::Accepted)
	{
		roadsWidget->addRoad(_activeDialog->getAnnotation(),_activeDialog->getWidth(),_activeDialog->getTexturePath());
	}

}

void ByyMainWindow::addMask()
{
	maskWidget->addArea();
}

void ByyMainWindow::setHudTitle()
{
	ByyHudTitleDialog dlg(sceneController);
	if (dlg.exec())
	{
		earthToolModified();
	}
}

void ByyMainWindow::editModel( bool checked )
{
	modelWidget->enableModelEdit(checked);

	if (checked)
	{
		modelDockWidgt->show();
	}
}

void ByyMainWindow::editVideo( bool checked )
{
	modelWidget->enableVideoEdit(checked);

	if (checked)
	{
		modelDockWidgt->show();
	}
}

void ByyMainWindow::editBuildingInfo( bool checked )
{
	modelWidget->enableInfoEdit(checked);

	if (checked)
	{
		modelDockWidgt->show();
	}
}
