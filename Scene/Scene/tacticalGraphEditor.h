#ifndef TACTICALGRAPHEDITOR_H
#define TACTICALGRAPHEDITOR_H

#include "tacticalGraphicsCreator.h"

class SCENE_EXPORT ByyTacticalGraphEditor : public ByyTacticalGraphicsCreator
{
public:
	ByyTacticalGraphEditor(ByyIG& ig,ByyControlObject* obj);
	~ByyTacticalGraphEditor();

protected:
	virtual bool mouseClicked(int button,int x,int y,osg::View *view);

protected:
	ByyControlObject* myObj;
};

#endif // TACTICALGRAPHEDITOR_H
