#ifndef BYY_TRITON_CALLBACK_H
#define BYY_TRITON_CALLBACK_H 1

#include <osg/Referenced>
#include "TritonExport.h"

namespace Triton
{
	class Ocean;
	class Environment;

}
class SNIGRITON_EXPORT TritonCallback : public osg::Referenced
{
public:
	TritonCallback():myOcean(0),myEnvironment(0){}

    virtual void onInitialize(Triton::Environment* env,Triton::Ocean* ocean)
	{
		myEnvironment=env;
		myOcean=ocean;
	}

    virtual void onDrawOcean() { }

protected:
	Triton::Ocean* myOcean;
	Triton::Environment* myEnvironment;
};

#endif // BYY_TRITON_CALLBACK_H
