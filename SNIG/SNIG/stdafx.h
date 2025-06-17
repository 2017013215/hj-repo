#pragma once

#include <QtGui>
#include <Windows.h>
#include <osgViewer/View>
#include <osgViewer/CompositeViewer>

#include "ByyRenderer.h"
#include "terrain.h"
#include "ByyIG.h"
#include "ByyDisplay.h"
#include "ByyOsgWindow.h"
#include "ByyInputController.h"
#include "ByyChannel.h"
#include "ByyObserver.h"
#include "ByyCore.h"
#include "ByyInputController.h"
#include "opd.h"
#include "remoteObjectManager.h"
#include "localObjectManager.h"
#include "ByyControlObject.h"
#include "sceneControlObject.h"
#include "sceneObjectManager.h"
#include "localEntity.h"
#include "ByyEventProcessor.h"
#include "ByyHdose.h"

#define ICONPATH QString("../data/icons/")
#define VIEWPOINTPATH "../data/viewpoints/"