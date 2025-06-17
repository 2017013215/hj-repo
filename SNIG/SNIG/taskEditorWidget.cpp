#include "StdAfx.h"
#include "taskEditorWidget.h"

ByyTaskEditorWidget::ByyTaskEditorWidget(ByyIG& ig,QWidget *parent)
	: QWidget(parent,Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint)
	,myIG(ig)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle("");
}

ByyTaskEditorWidget::~ByyTaskEditorWidget()
{

}
