#include "XGISVLayer.h"
#include "GIS-Extend/XVItem.h"
#include "XVItemPoint.h"

#include <qgsgeometry.h>

namespace XGIS
{
	XGISVLayer::XGISVLayer(void)
		:m_unNextItemId(1)
	{
	}

	XGISVLayer::XGISVLayer(const QString& _strPath, const QString& _strBaseName, const QString& _strProviderLib /*= QString::null */)
		: QgsVectorLayer(_strPath,_strBaseName,_strProviderLib)
		,m_unNextItemId(1)
	{
		
	}


	XGISVLayer::~XGISVLayer(void)
	{
	}
}