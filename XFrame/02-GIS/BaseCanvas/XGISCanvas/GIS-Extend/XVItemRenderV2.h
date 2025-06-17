#ifndef XVITEMRENDERV2_H
#define XVITEMRENDERV2_H

#include <XGISCanvasDef.h>

#include <qgsrendererv2.h>

namespace XGIS
{
	class XVItemRenderV2 : public QgsFeatureRendererV2
	{
	public:
		XVItemRenderV2();
		~XVItemRenderV2();

	public:
		 virtual void startRender( QgsRenderContext& context, const QgsFields& fields );
		  virtual void stopRender( QgsRenderContext& context );
		  virtual QList<QString> usedAttributes();
		  virtual QgsFeatureRendererV2* clone() const;
		  virtual bool renderFeature( QgsFeature& feature, QgsRenderContext& context, int layer = -1, bool selected = false, bool drawVertexMarker = false );
	};
}
#endif // XVITEMRENDERV2_H
