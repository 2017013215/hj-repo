#include "StdAfx.h"
#include "sceneObjectRegistration.h"
#include "ByyIG.h"
#include "sceneObjectManager.h"
#include "tacticalGraphicsProcesser.h"

#include "ByyWaypoint.h"
#include "ByyPhaseLine.h"
#include "ByyRoute.h"
#include "ByyCircle.h"
#include "ByyPolygon.h"
#include "ByyEllipse.h"
#include "ByyRectangle.h"
#include "ByyFineArrow.h"
#include "ByyDoubleArrow.h"
#include "ByyGatheringPlace.h"
#include "ByyAttackArrow.h"
#include "ByyStraightArrow.h"
#include "ByyArc.h"

#include "sceneWayPoint.h"
#include "scenePhaseLine.h"
#include "sceneRoute.h"
#include "sceneCircle.h"
#include "scenePolygon.h"
#include "sceneEllipse.h"
#include "sceneRectangle.h"
#include "sceneFineArrow.h"
#include "sceneDoubleArrow.h"
#include "sceneGatheringPlace.h"
#include "sceneAttackArrow.h"
#include "sceneStraightArrow.h"
#include "ByySceneArc.h"

#include "routeCreator.h"
#include "polygonCreator.h"
#include "ellipseCreator.h"
#include "waypointCreator.h"
#include "phaseLineCreator.h"
#include "rectangleCreator.h"
#include "circleCreator.h"
#include "fineArrowCreator.h"
#include "doubleArrowCreator.h"
#include "gatheringPlaceCreator.h"
#include "attackArrowCreator.h"
#include "straightArrowCreator.h"
#include "ByyArcCreator.h"

#include "circleEditor.h"
#include "ellipseEditor.h"
#include "polygonEditor.h"
#include "rectangleEditor.h"
#include "routeEditor.h"
#include "phaseLineEditor.h"
#include "wayPointEditor.h"
#include "ByyArcEditor.h"

void registerSceneObject( ByyIG& ig )
{
	ByySceneObjectManager* sceneObjMgr=ig.sceneObjectManager();
	sceneObjMgr->addCreator(ByyCircle::theType(),ByySceneCircle::create);
	sceneObjMgr->addCreator(ByyArc::theType(),ByySceneArc::create);
	sceneObjMgr->addCreator(ByyEllipse::theType(),ByySceneEllipse::create);
	sceneObjMgr->addCreator(ByyPhaseLine::theType(),ByyScenePhaseLine::create);
	sceneObjMgr->addCreator(ByyRectangle::theType(),ByySceneRectangle::create);
	sceneObjMgr->addCreator(ByyRoute::theType(),ByySceneRoute::create);
	sceneObjMgr->addCreator(ByyWaypoint::theType(),ByySceneWayPoint::create);
	sceneObjMgr->addCreator(ByyPolygon::theType(),ByyScenePolygon::create);
	sceneObjMgr->addCreator(ByyFineArrow::theType(),ByySceneFineArrow::create);
	sceneObjMgr->addCreator(ByyDoubleArrow::theType(),ByySceneDoubleArrow::create);
	sceneObjMgr->addCreator(ByyGatheringPlace::theType(),ByySceneGatheringPlace::create);
	sceneObjMgr->addCreator(ByyAttackArrow::theType(),ByySceneAttackArrow::create);
	sceneObjMgr->addCreator(ByyStraightArrow::theType(),ByySceneStraightArrow::create);

	ByyTacticalGraphicsProcesser &scneObjProcesser=ig.tacticalGraphicsProcesser();

	scneObjProcesser.addCreatorCreator(ByyWaypoint::theType(),ByyWaypointCreator::create);
	scneObjProcesser.addCreatorCreator(ByyRoute::theType(),ByyRouteCreator::create);
	scneObjProcesser.addCreatorCreator(ByyPolygon::theType(),ByyPolygonCreator::create);
	scneObjProcesser.addCreatorCreator(ByyEllipse::theType(),ByyEllipseCreator::create);
	scneObjProcesser.addCreatorCreator(ByyPhaseLine::theType(),ByyPhaseLineCreator::create);
	scneObjProcesser.addCreatorCreator(ByyRectangle::theType(),ByyRectangleCreator::create);
	scneObjProcesser.addCreatorCreator(ByyCircle::theType(),ByyCircleCreator::create);
	scneObjProcesser.addCreatorCreator(ByyArc::theType(),ByyArcCreator::create);
	scneObjProcesser.addCreatorCreator(ByyFineArrow::theType(),ByyFineArrowCreator::create);
	scneObjProcesser.addCreatorCreator(ByyDoubleArrow::theType(),ByyDoubleArrowCreator::create);
	scneObjProcesser.addCreatorCreator(ByyGatheringPlace::theType(),ByyGatheringPlaceCreator::create);
	scneObjProcesser.addCreatorCreator(ByyAttackArrow::theType(),ByyAttackArrowCreator::create);
	scneObjProcesser.addCreatorCreator(ByyStraightArrow::theType(),ByyStraightArrowCreator::create);

	scneObjProcesser.addEditorCreator(ByyCircle::theType(),ByyCircleEditor::create);
	scneObjProcesser.addEditorCreator(ByyEllipse::theType(),ByyEllipseEditor::create);
	scneObjProcesser.addEditorCreator(ByyPolygon::theType(),ByyPolygonEditor::create);
	
	scneObjProcesser.addEditorCreator(ByyRectangle::theType(),ByyRectangleEditor::create);
	scneObjProcesser.addEditorCreator(ByyRoute::theType(),ByyRouteEditor::create);
	scneObjProcesser.addEditorCreator(ByyPhaseLine::theType(),ByyPhaseLineEditor::create);
	scneObjProcesser.addEditorCreator(ByyWaypoint::theType(),ByyWaypointEditor::create);

	scneObjProcesser.addEditorCreator(ByyArc::theType(),ByyArcEditor::create);
	scneObjProcesser.addEditorCreator(ByyFineArrow::theType(),ByyPolygonEditor::create);
	scneObjProcesser.addEditorCreator(ByyDoubleArrow::theType(),ByyPolygonEditor::create);
	scneObjProcesser.addEditorCreator(ByyGatheringPlace::theType(),ByyPolygonEditor::create);
	scneObjProcesser.addEditorCreator(ByyAttackArrow::theType(),ByyPolygonEditor::create);

}
