#include "XImageMarkerSymbolLayerV2.h"
#include <QGIS/qgssymbolv2.h>
#include <QtGui/QPainter>
#include "XCommonLib.h"
Q_GUI_EXPORT extern int qt_defaultDpiX();
Q_GUI_EXPORT extern int qt_defaultDpiY();

namespace XSpace_Plugins
{
	static void _fixQPictureDPI(QPainter* p)
	{
		// QPicture makes an assumption that we drawing to it with system DPI.
		// Then when being drawn, it scales the painter. The following call
		// negates the effect. There is no way of setting QPicture's DPI.
		// See QTBUG-20361
		p->scale(static_cast<double>(qt_defaultDpiX()) / p->device()->logicalDpiX(),
			static_cast<double>(qt_defaultDpiY()) / p->device()->logicalDpiY());
	}

	XImageCache XImageCache::ms_Instance;
	XImageMarkerSymbolLayerV2::~XImageMarkerSymbolLayerV2()
	{
	}

	XImageMarkerSymbolLayerV2::XImageMarkerSymbolLayerV2(const QString& _strName /*= DEFAULT_IMAGE_MARKER_NAME*/, const double& _dSize /*= DEFAULT_IMAGE_MARKER_SIZE*/, const double& _dAngle /*= DEFAULT_IMAGE_MARKER_ANGLE*/, QgsSymbolV2::ScaleMethod _ScaleMethod /*= DEFAULT_SCALE_METHOD*/)
	{
		m_strImagePath = _strName;
		mSize = _dSize;
		mAngle = _dAngle;
		mOffset = QPointF(0, 0);
		mScaleMethod = _ScaleMethod;
	}

	QgsSymbolLayerV2* XImageMarkerSymbolLayerV2::create(const QgsStringMap& properties /*= QgsStringMap()*/)
	{
		QString strName = DEFAULT_IMAGE_MARKER_NAME;
		double dSize = DEFAULT_IMAGE_MARKER_SIZE;
		double dAngle = DEFAULT_IMAGE_MARKER_ANGLE;
		QgsSymbolV2::ScaleMethod scaleMethod = DEFAULT_SCALE_METHOD;
		if (properties.contains("name"))
			strName = properties["name"];
		if (properties.contains("size"))
			dSize = properties["size"].toDouble();
		if (properties.contains("angle"))
			dAngle = properties["angle"].toDouble();
		if (properties.contains("scale_method"))
			scaleMethod = QgsSymbolLayerV2Utils::decodeScaleMethod(properties["scale_method"]);

		XImageMarkerSymbolLayerV2* pSymbolLayer = new XImageMarkerSymbolLayerV2(strName, dSize, dAngle, scaleMethod);
		pSymbolLayer->restoreDataDefinedProperties(properties);

		return pSymbolLayer;
	}

	QString XImageMarkerSymbolLayerV2::layerType() const
	{
		return "ImageMarket";
	}

	void XImageMarkerSymbolLayerV2::startRender(QgsSymbolV2RenderContext& context)
	{
		QgsMarkerSymbolLayerV2::startRender(context); // get anchor point expressions
		Q_UNUSED(context);
		prepareExpressions(context);
	}

	void XImageMarkerSymbolLayerV2::stopRender(QgsSymbolV2RenderContext& context)
	{
		Q_UNUSED(context);
	}

	XImageMarkerSymbolLayerV2* XImageMarkerSymbolLayerV2::clone() const
	{
		XImageMarkerSymbolLayerV2* pSymLayer = new XImageMarkerSymbolLayerV2(m_strImagePath, mSize, mAngle);
		pSymLayer->setColor(mColor);
		pSymLayer->setOffset(mOffset);
		pSymLayer->setOffsetUnit(mOffsetUnit);
		pSymLayer->setOffsetMapUnitScale(mOffsetMapUnitScale);
		pSymLayer->setSizeUnit(mSizeUnit);
		pSymLayer->setSizeMapUnitScale(mSizeMapUnitScale);
		pSymLayer->setHorizontalAnchorPoint(mHorizontalAnchorPoint);
		pSymLayer->setVerticalAnchorPoint(mVerticalAnchorPoint);
		copyDataDefinedProperties(pSymLayer);
		copyPaintEffect(pSymLayer);
		return pSymLayer;
	}

	void XImageMarkerSymbolLayerV2::renderPoint(QPointF _Point, QgsSymbolV2RenderContext& _Context)
	{
		QPainter *pxPaint = _Context.renderContext().painter();
		if (!pxPaint)
			return;
		bool bHasDataDefinedSize = false;
		double dScaledSize = calculateSize(_Context, bHasDataDefinedSize);
		double dSize = QgsSymbolLayerV2Utils::convertToPainterUnits(_Context.renderContext(), dScaledSize, mSizeUnit, mSizeMapUnitScale);

		//don't render symbols with size below one or above 10,000 pixels
		if (static_cast<int>(dSize) < 1 || 10000.0 < dSize)
			return;
		pxPaint->save();

		QPointF outputOffset;
		double angle = 0.0;
		calculateOffsetAndRotation(_Context, dScaledSize, outputOffset, angle);

		pxPaint->translate(_Point + outputOffset);
		bool rotated = !qgsDoubleNear(angle, 0);
		if (rotated)
			pxPaint->rotate(angle);

		QString strPath = m_strImagePath;
		if (hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_NAME))
		{
			_Context.setOriginalValueVariable(m_strImagePath);
			strPath = evaluateDataDefinedProperty(QgsSymbolLayerV2::EXPR_NAME, _Context, m_strImagePath).toString();
		}

		bool bFitsInCache = true;
		bool bUsePict = true;
		double dHWRatio = 1.0;
		//if (!_Context.renderContext().forceVectorOutput() && !rotated)
		{
			bUsePict = false;
			QImage& img = XImageCache::Instance()->GetImage(m_strImagePath, dSize,
				_Context.renderContext().scaleFactor(), _Context.renderContext().rasterScaleFactor(), bFitsInCache);
			QImage sacleImg = img;// .scaled(img.size()*mSize);
			if (sacleImg.width() > 1)
			{
				if (!xDoubleNear(_Context.alpha(), 1.0))
				{
					QImage transparentImage = sacleImg.copy();
					QgsSymbolLayerV2Utils::multiplyImageOpacity(&transparentImage, _Context.alpha());
					pxPaint->drawImage(-transparentImage.width() / 2.0, -transparentImage.height() / 2.0, transparentImage);
					dHWRatio = static_cast<double>(transparentImage.height()) / static_cast<double>(transparentImage.width());
				}
				else
				{
					pxPaint->drawImage(-sacleImg.width()/2.0, -sacleImg.height()/2.0, sacleImg);
					//pxPaint->drawPoint(_Point);
					//pxPaint->drawImage(QRectF(_Point.x()-sacleImg.width()/2.0, _Point.y()-sacleImg.height()/2.0,
					//	_Point.x() + sacleImg.width() / 2.0, _Point.y() + sacleImg.height() / 2.0),sacleImg);
					dHWRatio = static_cast<double>(sacleImg.height()) / static_cast<double>(sacleImg.width());
				}
			}
		}
		

		//if (bUsePict || !bFitsInCache){
		//	pxPaint->setOpacity(_Context.alpha());
		//	const QPicture& pct = XImageCache::Instance()->GetPicture(m_strImagePath, dSize,
		//		_Context.renderContext().scaleFactor(), _Context.renderContext().rasterScaleFactor(), _Context.renderContext().forceVectorOutput());
		//	if (pct.width() > 1)
		//	{
		//		pxPaint->save();
		//		_fixQPictureDPI(pxPaint);
		//		pxPaint->drawPicture(0, 0, pct);
		//		pxPaint->restore();
		//		dHWRatio = static_cast<double>(pct.height()) / static_cast<double>(pct.width());
		//	}
		//}

		if (_Context.selected())
		{
			QPen pen(_Context.renderContext().selectionColor());
			double penWidth = QgsSymbolLayerV2Utils::lineWidthScaleFactor(_Context.renderContext(), QgsSymbolV2::MM);
			if (penWidth > dSize / 20)
			{
				// keep the pen width from covering symbol
				penWidth = dSize / 20;
			}
			double penOffset = penWidth / 2;
			pen.setWidth(penWidth);
			pxPaint->setPen(pen);
			pxPaint->setBrush(Qt::NoBrush);
			double wSize = dSize + penOffset;
			double hSize = dSize * dHWRatio + penOffset;
			pxPaint->drawRect(QRectF(-wSize / 2.0, -hSize / 2.0, wSize, hSize));
		}

		pxPaint->restore();

		if (_Context.renderContext().flags() & QgsRenderContext::Antialiasing)
		{
			// workaround issue with nested QPictures forgetting antialiasing flag - see http://hub.qgis.org/issues/14960
			pxPaint->setRenderHint(QPainter::Antialiasing);
		}
	}

	double XImageMarkerSymbolLayerV2::calculateSize(QgsSymbolV2RenderContext& context, bool& hasDataDefinedSize) const
	{
		double scaledSize = mSize;
		hasDataDefinedSize = context.renderHints() & QgsSymbolV2::DataDefinedSizeScale || hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_SIZE);

		bool ok = true;
		if (hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_SIZE))
		{
			context.setOriginalValueVariable(mSize);
			scaledSize = evaluateDataDefinedProperty(QgsSymbolLayerV2::EXPR_SIZE, context, mSize, &ok).toDouble();
		}

		if (hasDataDefinedSize && ok)
		{
			switch (mScaleMethod)
			{
			case QgsSymbolV2::ScaleArea:
				scaledSize = sqrt(scaledSize);
				break;
			case QgsSymbolV2::ScaleDiameter:
				break;
			}
		}

		return scaledSize;
	}

	QgsStringMap XImageMarkerSymbolLayerV2::properties() const
	{
		QgsStringMap map;
		map["name"] = QgsSymbolLayerV2Utils::symbolPathToName(m_strImagePath);
		map["size"] = QString::number(mSize);
		map["size_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit(mSizeUnit);
		map["size_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale(mSizeMapUnitScale);
		map["angle"] = QString::number(mAngle);
		map["offset"] = QgsSymbolLayerV2Utils::encodePoint(mOffset);
		map["offset_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit(mOffsetUnit);
		map["offset_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale(mOffsetMapUnitScale);
		map["scale_method"] = QgsSymbolLayerV2Utils::encodeScaleMethod(mScaleMethod);
		map["color"] = QgsSymbolLayerV2Utils::encodeColor(mColor);
		//map["outline_color"] = QgsSymbolLayerV2Utils::encodeColor(mOutlineColor);
		//map["outline_width"] = QString::number(mOutlineWidth);
//		map["outline_width_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit(mOutlineWidthUnit);
//		map["outline_width_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale(mOutlineWidthMapUnitScale);
		map["horizontal_anchor_point"] = QString::number(mHorizontalAnchorPoint);
		map["vertical_anchor_point"] = QString::number(mVerticalAnchorPoint);

		saveDataDefinedProperties(map);
		return map;
	}

	void XImageMarkerSymbolLayerV2::calculateOffsetAndRotation(QgsSymbolV2RenderContext& context, double scaledSize, QPointF& offset, double& angle) const
	{
		//offset
		double offsetX = 0;
		double offsetY = 0;
		markerOffset(context, scaledSize, scaledSize, offsetX, offsetY);
		offset = QPointF(offsetX, offsetY);

		angle = mAngle + mLineAngle;
		if (hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_ANGLE))
		{
			context.setOriginalValueVariable(mAngle);
			angle = evaluateDataDefinedProperty(QgsSymbolLayerV2::EXPR_ANGLE, context, mAngle).toDouble() + mLineAngle;
		}

		bool hasDataDefinedRotation = context.renderHints() & QgsSymbolV2::DataDefinedRotation || hasDataDefinedProperty(QgsSymbolLayerV2::EXPR_ANGLE);
		if (hasDataDefinedRotation)
		{
			// For non-point markers, "dataDefinedRotation" means following the
			// shape (shape-data defined). For them, "field-data defined" does
			// not work at all. TODO: if "field-data defined" ever gets implemented
			// we'll need a way to distinguish here between the two, possibly
			// using another flag in renderHints()
			const QgsFeature* f = context.feature();
			if (f)
			{
				const QgsGeometry *g = f->constGeometry();
				if (g && g->type() == QGis::Point)
				{
					const QgsMapToPixel& m2p = context.renderContext().mapToPixel();
					angle += m2p.mapRotation();
				}
			}
		}

		if (angle)
			offset = _rotatedOffset(offset, angle);
	}

	QgsSymbolLayerV2* XImageMarkerSymbolLayerV2::createFromSld(QDomElement &element)
	{
		QgsDebugMsg("Entered.");

		QDomElement graphicElem = element.firstChildElement("Graphic");
		if (graphicElem.isNull())
			return nullptr;

		QString path, mimeType;
		QColor fillColor;
		double size;

		if (!QgsSymbolLayerV2Utils::externalGraphicFromSld(graphicElem, path, mimeType, fillColor, size))
			return nullptr;

		if (mimeType != "image/img+xml")
			return nullptr;

		double angle = 0.0;
		QString angleFunc;
		if (QgsSymbolLayerV2Utils::rotationFromSldElement(graphicElem, angleFunc))
		{
			bool ok;
			double d = angleFunc.toDouble(&ok);
			if (ok)
				angle = d;
		}

		QPointF offset;
		QgsSymbolLayerV2Utils::displacementFromSldElement(graphicElem, offset);

		XImageMarkerSymbolLayerV2* m = new XImageMarkerSymbolLayerV2(path, size);
		return m;
	}




	void XImageCache::clear()
	{
		m_xLock.lock();
		foreach(XImageEntity* pImg, m_xImageCaches){
			if (pImg)
				delete pImg;
			pImg = NULL;
		}
		m_xImageCaches.clear();
		m_xLock.unlock();
	}

	QImage& XImageCache::GetImage(const QString& _strPath, const double& _dSize, const double& _dWScF, const double& _dRScF, bool& _bIsFirst)
	{
		m_xLock.lock();

		QList<XImageEntity*> xEntries = m_xImageCaches.values(_strPath);
		m_xLock.unlock();
		QList<XImageEntity*>::iterator entryIt = xEntries.begin();
		XImageEntity* pxCurEny = NULL;
		for (; entryIt != xEntries.end(); ++entryIt){
			XImageEntity* xImgEny = *entryIt;
			if (xDoubleNear(xImgEny->dSize, _dSize) && xDoubleNear(xImgEny->dWidthScaleFactor, _dWScF)
				&& xDoubleNear(xImgEny->dRasterScaleFactor, _dRScF))
			{
				pxCurEny = xImgEny;
				break;
			}
		}

		if (!pxCurEny){
			pxCurEny = new XImageEntity();
			pxCurEny->dSize = _dSize;
			pxCurEny->dRasterScaleFactor = _dRScF;
			pxCurEny->dWidthScaleFactor = _dWScF;
			pxCurEny->strPath = _strPath;
			pxCurEny->pImage = new QImage(_strPath);

			pxCurEny->pPicture = new QPicture();
			QRectF rect;

			double hwRatio = 1.0;
			if (pxCurEny->pImage->width() > 0)
				hwRatio = pxCurEny->pImage->height() / (double)pxCurEny->pImage->width();
			double wSize = _dSize;
			double hSize = _dSize*hwRatio;
			QSizeF s(pxCurEny->pImage->size());
			s.scale(wSize, hSize, Qt::KeepAspectRatio);
			rect = QRectF(-s.width() / 2.0, -s.height() / 2.0, s.width(), s.height());

			QPainter p(pxCurEny->pPicture);
			QImage scaleImg = pxCurEny->pImage->scaled(rect.width(), rect.height(), Qt::KeepAspectRatio);
			p.drawImage(rect, scaleImg);
			p.save();

			m_xLock.lock();
			m_xImageCaches.insert(_strPath, pxCurEny);
			m_xLock.unlock();
			_bIsFirst = true;
		}
		else
			_bIsFirst = false;
		return *(pxCurEny->pImage);
	}

	const QPicture& XImageCache::GetPicture(const QString& _strPath, const double& _dSize, const double& _dWScF, const double& _dRScF, bool _bVectorOut)
	{
		m_xLock.lock();

		QList<XImageEntity*> xEntries = m_xImageCaches.values(_strPath);
		m_xLock.unlock();
		QList<XImageEntity*>::iterator entryIt = xEntries.begin();
		XImageEntity* pxCurEny = NULL;
		for (; entryIt != xEntries.end(); ++entryIt){
			XImageEntity* xImgEny = *entryIt;
			if (xDoubleNear(xImgEny->dSize, _dSize) && xDoubleNear(xImgEny->dWidthScaleFactor, _dWScF)
				&& xDoubleNear(xImgEny->dRasterScaleFactor, _dRScF))
			{
				pxCurEny = xImgEny;
				break;
			}
		}

		if (!pxCurEny){
			pxCurEny = new XImageEntity();
			pxCurEny->dSize = _dSize;
			pxCurEny->dRasterScaleFactor = _dRScF;
			pxCurEny->dWidthScaleFactor = _dWScF;
			pxCurEny->strPath = _strPath;
			pxCurEny->pImage = new QImage(_strPath);

			pxCurEny->pPicture = new QPicture();
			QRectF rect;
			
			double hwRatio = 1.0;
			if (pxCurEny->pImage->width() > 0)
				hwRatio = pxCurEny->pImage->height() / (double)pxCurEny->pImage->width();
			double wSize = _dSize;
			double hSize = _dSize*hwRatio;
			QSizeF s(pxCurEny->pImage->size());
			s.scale(wSize, hSize, Qt::KeepAspectRatio);
			rect = QRectF(-s.width() / 2.0, -s.height() / 2.0, s.width(), s.height());

			QPainter p(pxCurEny->pPicture);
			QImage scaleImg = pxCurEny->pImage->scaled(rect.width(),rect.height(), Qt::KeepAspectRatio);
			p.drawImage(rect,scaleImg);
			p.save();

			m_xLock.lock();
			m_xImageCaches.insert(_strPath, pxCurEny);
			m_xLock.unlock();
		}
		return *(pxCurEny->pPicture);
	}

}