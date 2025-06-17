#include "stdafx.h"
#include "prop.h"
#include "model.h"
#include "ByyIG.h"
#include "terrain.h"
#include "ByyRenderer.h"

class ModelLoadTask : public QRunnable
{
public:
	ModelLoadTask(ByyProp* comp,osgEarth::MapNode* mapNode,const osgEarth::Config& conf,int index,int total)
		:_mapNode(mapNode),_conf(conf),_comp(comp),_index(index),_total(total)
	{

	}

	void run()
	{
		if (_comp->_threadNeedRelease)
		{
			return;
		}

		ByyModel *model=new ByyModel(_mapNode,_conf);

		printf("加载第%d个模型,共%d个：%s\n",_index,_total,model->name().toLocal8Bit().data());

		_comp->modelLoadFinish(model);
	}

private:
	osgEarth::MapNode *_mapNode;
	const osgEarth::Config _conf;

	ByyProp* _comp;

	int _index;
	int _total;
};

ByyProp::ByyProp( ByyIG* ig )
	: myIG(ig),_threadNeedRelease(false)
{
	connect(this,SIGNAL(modelLoadFinish(ByyModel*)),this,SLOT(processModelLoadFinish(ByyModel*)));

	connect(myIG->terrain(),SIGNAL(terrainLoaded(osgEarth::MapNode*)),this,SLOT(init(osgEarth::MapNode*)));
}

ByyProp::~ByyProp()
{
	
}

void ByyProp::init( osgEarth::MapNode *mapNode )
{
	osg::ref_ptr<osgDB::Options> dbOptions = osgEarth::Registry::instance()->cloneOrCreateOptions();

	const osgEarth::Config& externals = mapNode->externalConfig();

	const osgEarth::Config& modelsConf        = externals.child("models");
	const osgEarth::Config& roadsConf       = externals.child("roads");

	if (!modelsConf.empty())
	{
		const osgEarth::ConfigSet& children = modelsConf.children();

		int total=children.size();

		int j=1;

		for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
		{
			if (!i->empty())
			{
				ModelLoadTask *task=new ModelLoadTask(this,mapNode,*i,j,total);

				myLoadThreadPool.start(task,10);
			}
			++j;
		}
	}
}

void ByyProp::release()
{
	_threadNeedRelease=true;
	myLoadThreadPool.waitForDone();
	_threadNeedRelease=false;

	qDeleteAll(myModels);
	myModels.clear();
}

QList<ByyModel*>& ByyProp::models()
{
	return myModels;
}

void ByyProp::processModelLoadFinish( ByyModel* model )
{
	myModels.append(model);

	if (model->useLOD())
	{
		osg::LOD *lod=new osg::LOD;
		lod->addChild(model->getModel(),0,model->lodRange()*1000);

		myIG->renderer()->addNodeToRoot(lod,ByyOsgRenderer::Prop);
	}
	else
		myIG->renderer()->addNodeToRoot(model->getModel(),ByyOsgRenderer::Prop);
	
}