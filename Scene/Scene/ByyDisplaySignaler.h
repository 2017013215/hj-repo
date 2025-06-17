#ifndef BYYDISPLAYSIGNALER_H
#define BYYDISPLAYSIGNALER_H

#include <QObject>
#include "scene_global.h"

class ByyOsgWindow;
class ByyChannel;

class SCENE_EXPORT ByyDisplaySignaler : public QObject
{
	Q_OBJECT

public:
	~ByyDisplaySignaler();

	static ByyDisplaySignaler* instance();

public:
	void emitWindowCreated(ByyOsgWindow* window);
	void emitWindowToBeDestroyed(ByyOsgWindow* window);
	void emitChannelCreated(ByyChannel* channel);
	void emitChannelToBeDestroyed(ByyChannel* channel);
signals:
	void windowCreated(ByyOsgWindow*);
	void windowToBeDestroyed(ByyOsgWindow*);
	void channelCreated(ByyChannel*);
	void channelToBeDestroyed(ByyChannel*);

protected:
	ByyDisplaySignaler();
};

#endif // BYYDISPLAYSIGNALER_H
