#ifndef TASKWIDGETFACTORY_H
#define TASKWIDGETFACTORY_H

class ByyTaskEditorWidget;
class ByyTaskEditorWidgetCreator;

class ByyTaskWidgetFactory
{
public:
	ByyTaskWidgetFactory();
	~ByyTaskWidgetFactory();

	static ByyTaskWidgetFactory& instance();

	void addCreator(ByyTaskEditorWidgetCreator* creator);
	void removeCreator(ByyTaskEditorWidgetCreator* creator);

	ByyTaskEditorWidget* createTaskEditor(const QString& taskType,ByyIG& ig,QWidget* parent=0);

protected:
	typedef QMap<QString,ByyTaskEditorWidgetCreator*> TaskEditorWidgetCreatorMap;
	TaskEditorWidgetCreatorMap myTaskEditorWidgetCreators;
};

#endif // TASKWIDGETFACTORY_H
