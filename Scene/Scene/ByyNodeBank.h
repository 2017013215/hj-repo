#ifndef BYYDATABANK_H
#define BYYDATABANK_H

#include "placeNode.h"

class ByyNodeBank
{
public:
	~ByyNodeBank();

	static ByyNodeBank& instance();

	enum MajorType
	{
		InvalidMajor,
		Entity,
		ControlObject
	};

	enum MinorType
	{
		InvalidMinor,
		MainObject
	};

	struct NodeData
	{
		MajorType majorType;
		MinorType minorType;
		unsigned int objectId;

		NodeData()
		{
			majorType=InvalidMajor;
			minorType=InvalidMinor;
			objectId=0;
		}

		bool valid()
		{
			return majorType!=InvalidMajor&&minorType!=InvalidMinor&&objectId!=0;
		}
	};

	void addNode(osg::Node* node,MajorType majorType,MinorType minorType,unsigned int objectId);
	void removeNode(osg::Node* node);

	bool hasNode(osg::Node* node);

	NodeData getNodeData(osg::Node* node);

	void addPlaceNode(osgEarth::Annotation::ByyPlaceNode* node,unsigned int objectId);
	void removePlaceNode(osgEarth::Annotation::ByyPlaceNode* node);

	typedef QMap<osgEarth::Annotation::ByyPlaceNode*,unsigned int> PlaceNodeList;

	PlaceNodeList& getPlaceNodeList();

private:
	ByyNodeBank();

private:
	QMap<osg::Node*,NodeData> myNodeDatas;

	PlaceNodeList myPlaceNodes;
};

#endif // BYYDATABANK_H
