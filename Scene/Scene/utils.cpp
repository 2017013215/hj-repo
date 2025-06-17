#include "StdAfx.h"
#include "utils.h"

#include "osgFileCache.h"
#include "mappingQuery.h"
#include "ByyIG.h"

#include <QPainter>

SCENE_EXPORT osg::Node* loadEntityModel( const QString& type,ByyIG& ig )
{
	QString modelfile=ByyMappingQuery::instance()->getEntityModel(type.toLocal8Bit().data());
	//QString modelfile= "D:/AFSIM2.9.7z/afsim-2.9.0/afsim-2.9.0-win64/resources/models/3d/f22.osgb";
	if(modelfile.isEmpty())
		modelfile="../data/models/SnModels/Other/Unknown.flt";

	osg::Node* node=ig.fileCache().readNodeFile(modelfile,osg::CopyOp::DEEP_COPY_NODES);

	return node;
}

SCENE_EXPORT osg::Image* loadEntityIcon( const QString& type,ByyIG& ig )
{
	QString modelfile=ByyMappingQuery::instance()->getEntityIcon(type.toLocal8Bit().data());

	osg::Image* image=ig.fileCache().readImageFile(modelfile);

	return image;
}

SCENE_EXPORT QPixmap loadEntityIcon( const QString& type,ByyForceType ft )
{
	QString modelfile=ByyMappingQuery::instance()->getEntityIcon(type.toStdString().c_str());

	QPixmap pixmap(modelfile);

	QPainter painter(&pixmap);

	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

	QColor clr=Qt::green;

	switch(ft)
	{
	case ByyForceOpposing:
		clr=Qt::blue;
		break;
	case ByyForceFriendly:
		clr=Qt::red;
		break;
	}

	painter.fillRect(pixmap.rect(),clr);

	painter.end();

	return pixmap;
}

SCENE_EXPORT osg::Vec3 forceTypeColor( const ByyForceType& ft )
{
	osg::Vec3 rel(1,1,1);
	switch(ft)
	{
	case ByyForceOpposing:
		rel=osg::Vec3(0,0,1);
		break;
	case ByyForceFriendly:
		rel=osg::Vec3(1,0,0);
		break;
	case ByyForceNeutral:
		rel=osg::Vec3(0,1,0);
		break;
	}

	return rel;
}

SCENE_EXPORT osg::Vec4 forceTypeColorAlpha( const ByyForceType& ft ,float alpha)
{
	return osg::Vec4(forceTypeColor(ft),alpha);
}
