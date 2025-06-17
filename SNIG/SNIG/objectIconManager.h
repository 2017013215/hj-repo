#ifndef OBJECTICONMANAGER_H
#define OBJECTICONMANAGER_H

class ByyObjectIconManager
{
public:
	ByyObjectIconManager();
	~ByyObjectIconManager();

	static ByyObjectIconManager& instance();

	QIcon getIcon(ByyForceType type);

private:
	QMap<ByyForceType,QIcon> myIcons;
	
};

#endif // OBJECTICONMANAGER_H
