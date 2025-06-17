#include "ByyHdoseEntity.h"

#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"

ByyHdoseEntity::ByyHdoseEntity(CObj* obj)
	:myObj(obj)
{
	//generateAttributeInfo();
}

ByyHdoseEntity::~ByyHdoseEntity()
{

}

void ByyHdoseEntity::generateAttributeInfo()
{
	if(!myObj)
		return;

	ClassInfo* pInfo=myObj->GetClsInfo();

	std::string strName,strType;

	int size=pInfo->m_attributeinfos.GetSize();

	for(int i=1;i<size;++i)
	{
		QString strName=pInfo->m_attributeinfos[i]->name;
		int ioffset=pInfo->m_attributeinfos[i]->offset;

		char* adr=((char*)myObj)+ioffset;

		AttributeInfo_st& attInfo=myAttributeInfoList[strName];

		attInfo.name=strName;
		attInfo.dataPt=adr;


		QString strType=pInfo->m_attributeinfos[i]->type;

	//	OutputDebugString((strName+"_"+strType).toLocal8Bit().data());

		AttributeInfo_st::DataType dt;

		
		if (strType=="double")
		{
			dt=AttributeInfo_st::t_double;
		}
		else if (strType=="float")
		{
			dt=AttributeInfo_st::t_float;
		}
		else if (strType=="GeoPt")
		{
			dt=AttributeInfo_st::t_GeoPt;
		}
		else if (strType=="Vec3")
		{
			dt=AttributeInfo_st::t_Vec3;
		}
		else
		{
			dt=AttributeInfo_st::t_other;
		}
		/*else if (strType=="char")
		{
			dt=AttributeInfo_st::t_char;
		}
		else if (strType=="BOOL")
		{
			dt=AttributeInfo_st::t_BOOL;
		}
		else if (strType=="WORD")
		{
			dt=AttributeInfo_st::t_WORD;
		}
		else if (strType=="DWORD")
		{
			dt=AttributeInfo_st::t_DWORD;
		}
		else if (strType=="bool")
		{
			dt=AttributeInfo_st::t_bool;
		}
		else if (strType=="int")
		{
			dt=AttributeInfo_st::t_int;
		}
		else if (strType=="long")
		{
			dt=AttributeInfo_st::t_long;
		}
		else
		{
			dt=AttributeInfo_st::t_other;
		}*/

		attInfo.myType=dt;
	}
}

QStringList ByyHdoseEntity::attributeNameList()
{
	return myAttributeInfoList.keys();
}

QStringList ByyHdoseEntity::attributeNameList( AttributeInfo_st::DataType type )
{
	QStringList rel;

	AttributeInfoList::iterator it=myAttributeInfoList.begin();

	for (;it!=myAttributeInfoList.end();++it)
	{
		if (it.value().myType==type)
		{
			rel.append(it.key());
		}
	}

	return rel;
}

template<class T>
T getValue(char* data)
{
	T rel=*(T*)data;

	return rel;
}

double ByyHdoseEntity::getAttributeValue( const QString& name )
{
	AttributeInfoList::iterator it=myAttributeInfoList.find(name);

	if (it==myAttributeInfoList.end())
		return 0;

	double rel=0;

	switch(it->myType)
	{
/*
	case AttributeInfo_st::t_bool:
		rel=getValue<bool>(it->dataPt);
		break;
	case AttributeInfo_st::t_int:
		rel=getValue<int>(it->dataPt);
		break;
	case AttributeInfo_st::t_long:
		rel=getValue<long>(it->dataPt);
		break;*/
	case AttributeInfo_st::t_double:
		rel=getValue<double>(it->dataPt);
		break;
	case AttributeInfo_st::t_float:
		rel=getValue<float>(it->dataPt);
		break;
/*
	case AttributeInfo_st::t_char:
		rel=getValue<char>(it->dataPt);
		break;
	case AttributeInfo_st::t_BOOL:
		rel=getValue<BOOL>(it->dataPt);
		break;
	case AttributeInfo_st::t_WORD:
		rel=getValue<WORD>(it->dataPt);
		break;
	case AttributeInfo_st::t_DWORD:
		rel=getValue<DWORD>(it->dataPt);
		break;*/
	}

	return rel;
}

ByyHdoseEntity::AttributeInfoList& ByyHdoseEntity::attributeInfoList()
{
	return myAttributeInfoList;
}

double ByyHdoseEntity::getGeoPtValue( const QString& name,unsigned index )
{
	AttributeInfoList::iterator it=myAttributeInfoList.find(name);

	if (it==myAttributeInfoList.end())
		return 0;

	GeoPt* getPt=(GeoPt*)it->dataPt;

	return (*getPt)[index];
}

double ByyHdoseEntity::getVec3Value( const QString& name,unsigned index )
{
	AttributeInfoList::iterator it=myAttributeInfoList.find(name);

	if (it==myAttributeInfoList.end())
		return 0;

	Vec3* getPt=(Vec3*)it->dataPt;

	return (*getPt)[index];
}


