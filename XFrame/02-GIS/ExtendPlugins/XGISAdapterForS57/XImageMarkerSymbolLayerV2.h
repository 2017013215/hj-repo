#ifndef XIMAGE_MARKET_SYMBOL_LAYERV2_HEADER
#define XIMAGE_MARKET_SYMBOL_LAYERV2_HEADER
#include "XTypeDef.h"
#include <QGIS/qgsmarkersymbollayerv2.h>
#include <QtCore/QMultiHash>

#define DEFAULT_IMAGE_MARKER_NAME         "./Data/Symbol/default.png"
#define DEFAULT_IMAGE_MARKER_SIZE         2*DEFAULT_POINT_SIZE
#define DEFAULT_IMAGE_MARKER_ANGLE        0

namespace XSpace_Plugins
{
	struct  XImageEntity
	{
		QImage* pImage;
		QPicture* pPicture;
		double dWidthScaleFactor;
		double dRasterScaleFactor;
		double dSize;
		QString strPath;
	};
	class XImageCache
	{
	public:
		XImageCache(){
			clear();
		};
		~XImageCache(){
			clear();
		};

	public:
		static XImageCache* Instance(){
			return &ms_Instance;
		}

		QImage& GetImage(const QString& _strPath, const double& _dSize,const double& _dWScF, const double& _dRScF, bool& _bIsFirst);
		const QPicture& GetPicture(const QString& _strPath, const double& _dSize, const double& _dWScF, const double& _dRScF, bool _bVectorOut);

	private:
		void clear();

	private:
		static XImageCache ms_Instance;

		QMultiHash<QString, XImageEntity*> m_xImageCaches;

		QMutex m_xLock;
	};

	class XImageMarkerSymbolLayerV2 : public QgsMarkerSymbolLayerV2
	{
	public:
		XImageMarkerSymbolLayerV2(const QString& _strName = DEFAULT_IMAGE_MARKER_NAME,
                                  const double& _dSize = DEFAULT_IMAGE_MARKER_SIZE,
								  const double& _dAngle = DEFAULT_IMAGE_MARKER_ANGLE,
								  QgsSymbolV2::ScaleMethod _ScaleMethod = DEFAULT_SCALE_METHOD);

		~XImageMarkerSymbolLayerV2();

		static QgsSymbolLayerV2* create(const QgsStringMap& properties = QgsStringMap());
		static QgsSymbolLayerV2* createFromSld(QDomElement &element);

		QString layerType() const override;

		void startRender(QgsSymbolV2RenderContext& context) override;

		void stopRender(QgsSymbolV2RenderContext& context) override;

		XImageMarkerSymbolLayerV2* clone() const override;

		void renderPoint(QPointF _Point, QgsSymbolV2RenderContext& _Context) override;

		QgsStringMap properties() const override;

	private:
		double calculateSize(QgsSymbolV2RenderContext& context, bool& hasDataDefinedSize) const;
		void calculateOffsetAndRotation(QgsSymbolV2RenderContext& context, double scaledSize, QPointF& offset, double& angle) const;

	protected:
		QString m_strImagePath;
	};
}

#endif