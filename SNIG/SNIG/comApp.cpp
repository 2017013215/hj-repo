#include "StdAfx.h"
#include "comApp.h"
#include "ByyApp.h"

comApp::comApp(QObject *parent)
	: QObject(parent)
{
	myApp = 0;
}

comApp::~comApp()
{

}

comApp* comApp::instance()
{
	static comApp t_instance;
	return &t_instance;
}

void comApp::setApp(ByyApp* app)
{
	myApp = app;

}

ByyApp* comApp::getApp()
{
	return myApp;
}