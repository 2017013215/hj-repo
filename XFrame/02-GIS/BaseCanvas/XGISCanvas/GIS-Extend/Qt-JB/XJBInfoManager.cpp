#include "XJBInfoManager.h"

#include "XGISCanvasDef.h"
#include <QDir>
#include "JBDef.h"
#include "XCommonLib.h"

#include <QDomDocument>

namespace XGIS
{
	const char XJBInfoManager::ms_strCheckTag[] = "DBLIB100";

	XJBInfoManager XJBInfoManager::ms_Instance;
	XJBInfoManager::XJBInfoManager(void)
		:m_strCurJBDir(QString::null)
		,m_bIsLoad(false)
		,ms_nConstCurveFlag(0x100)
	{
		m_cJBLibMap.clear();
	}

	XJBInfoManager::~XJBInfoManager(void)
	{
		clearAllData();
	}

	XJBInfoManager* XJBInfoManager::GetInstance()
	{
		return &ms_Instance;
	}

	bool XJBInfoManager::ReadJBInfoByDir(const QString& _strFilePath)
	{
		if(true == _strFilePath.isEmpty())
		{
			return false;
		}
		
		if(_strFilePath == m_strCurJBDir && false == m_bIsLoad)
		{
			return true;
		}

		QDir jbDir(_strFilePath);
		if(false == jbDir.exists())
		{
			return false;
		}

		m_strCurJBDir = _strFilePath;

		// 读取并解析JB库文件
		QFileInfoList fileList = jbDir.entryInfoList(QStringList("*.tag"),QDir::NoDotAndDotDot|QDir::Readable|QDir::Files|QDir::Dirs);

		if(true == fileList.isEmpty())
		{
			m_strCurJBDir.clear();
			return false;
		}

		bool bReadRes = false;
		foreach(QFileInfo fileInfo, fileList)
		{
			bReadRes = readJBLibPath(fileInfo.absoluteFilePath());
			if(false == bReadRes)
			{
				// 清除数据
				clearAllData();
				return false;
			}
		}
		m_strCurJBDir = _strFilePath;

		// 判断是否包含扩展绘制描述文件 Exceptive.xml
		QString strExceptivePath = m_strCurJBDir + QString("/Exceptive.xml");
		if(true == QFile::exists(strExceptivePath))
		{
			bool bRes = readExceptive(strExceptivePath);
			Q_ASSERT(bRes);
			if(false == bRes)
			{
				return false;
			}
		}

		return true;
	}

	bool XJBInfoManager::readJBLibPath(const QString& _strFilePath)
	{
		if(false == QFile::exists(_strFilePath))
		{
			return false;
		}

		QFile file(_strFilePath);
		if(false == file.open(QIODevice::ReadOnly))
		{
			return false;
		}

#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐

		QJB_HEAD JBHead;
		QJB_LAYER JBLayer;
		if( -1 == file.read((char*)&JBHead,sizeof(QJB_HEAD)))
		{
			file.close();
			return false;
		}

		// 校验库标识 "DBLIB100"
		if(strncmp(JBHead.csFileID,ms_strCheckTag,sizeof(JBHead.csFileID)) != 0)
		{
			file.close();
			return false;
		}

		// 校验库ID是否重复
		if(true == m_cJBLibMap.contains(JBHead.nID))
		{
			file.close();
			return false;
		}

		Q_JBLib* pNewJBLib = new Q_JBLib();
		pNewJBLib->strFilePath = _strFilePath;
		pNewJBLib->unID = JBHead.nID;
		pNewJBLib->strDesc = QString::fromLocal8Bit(JBHead.csFileDesc);
		pNewJBLib->cCategoryList.clear();
		pNewJBLib->cCategoryMap.clear();

		QMap<quint32,QJB_LAYER> mapIDLayerInfo;
		
		for (quint32 unIdx=0; unIdx < JBHead.unLayerNum; ++unIdx)
		{
			if(-1 == file.read((char*)&JBLayer,sizeof(JBLayer)))
			{
				file.close();
				delete pNewJBLib;
				pNewJBLib = nullptr;
				return false;
			}

			Q_JBCategory* pCate = new Q_JBCategory();
			pCate->unID = JBLayer.unID;
			pCate->unLibID = pNewJBLib->unID;
			pCate->strName = QString::fromLocal8Bit(JBLayer.csName);
			if(JBLayer.nGrpNum <= 0)
			{
				pCate->bHaveInfo = false;
			}else
			{
				pCate->bHaveInfo = true;
				// 读取JB信息
				bool bRes = readCateJBInfo(pCate,&JBLayer,file);
				if(false == bRes)
				{
					delete pCate;
					pCate = nullptr;
					file.close();
					delete pNewJBLib;
					pNewJBLib = nullptr;
					return false;
				}
			}

			if(1 == getLayerLevel(JBLayer.unID))
			{
				pNewJBLib->cCategoryList.append(pCate);
				pNewJBLib->cCategoryMap[pCate->unID] = pCate;
			}else
			{
				// 寻找所属父类别
				bool bIsFind = false;
				foreach(Q_JBCategory* pParCate,pNewJBLib->cCategoryList)
				{
					Q_JBCategory* pTgtCate = isSubCate(pParCate,pCate->unID);
					if(nullptr == pTgtCate)
					{
						continue;
					}

					bIsFind = true;
					pTgtCate->cSubCateList.append(pCate);
					pTgtCate->cSubCateMap[pCate->unID] = pCate;
					pTgtCate = nullptr;
					break;
				}
				Q_ASSERT(bIsFind);
			}
		}

		int md = 0;
	
#pragma pack(pop)

		// 加入库集合
		m_cJBLibMap[pNewJBLib->unID] = pNewJBLib;
		m_cJBLibList.append(pNewJBLib);
		pNewJBLib = nullptr;
		return true;
	}

	void XJBInfoManager::clearAllData()
	{
		m_strCurJBDir.clear();
		m_bIsLoad = false;

		foreach(Q_JBLib* pLib,m_cJBLibMap)
		{
			if(nullptr != pLib)
			{
				m_cJBLibMap[pLib->unID] = nullptr;
				delete pLib;
				pLib = nullptr;
			}
		}
		m_cJBLibMap.clear();
		m_cJBLibList.clear();

		QMap<quint32,QMap<quint32,ExceptiveInfo*>>::iterator itr = m_cExceptiveInfos.begin();
		while(itr != m_cExceptiveInfos.end())
		{
			foreach(ExceptiveInfo* pInfo,itr.value())
			{
				if(nullptr != pInfo)
				{
					delete pInfo;
					pInfo = nullptr;
				}
			}
			itr.value().clear();
			++itr;
		}
		m_cExceptiveInfos.clear();
	}

	int XJBInfoManager::getLayerLevel(quint32 _unID)
	{
		static int S_L;

		if(_unID==0) S_L=0;
		else if((_unID | 0x000000FF)==0x000000FF) S_L=1;    // 第一层
		else if((_unID | 0x0000FFFF)==0x0000FFFF) S_L=2;    // 第二层
		else if((_unID | 0x00FFFFFF)==0x00FFFFFF) S_L=3;    // 第三层
		else S_L=4;                                         // 第四层
		return(S_L);
	}

	bool XJBInfoManager::readCateJBInfo(Q_JBCategory* _pCate,const QJB_LAYER* _pLayer, QFile& _refFile)
	{
		if(_pLayer->nGrpNum <= 0)
		{
			return false;
		}

		QJB_ITEM_HEAD itemHead;

		quint32 unOffset=_pLayer->unGrpOffset;
		quint32 unItemDataSize = 0;
		quint32 unItemHeadLen = sizeof(itemHead);
		
		// 记录当前读取位置
		quint32 unOldPos = _refFile.pos();

		for (int nIdx = 0; nIdx < _pLayer->nGrpNum; nIdx ++)
		{
			// 设置读取位置
			_refFile.seek(unOffset);

			// 读取ItemHead
			if( -1 == _refFile.read((char*)&itemHead,unItemHeadLen))
			{
				return false;
			}

			unItemDataSize = itemHead.unLen - unItemHeadLen;

			Q_JBItem* pJBItem = new Q_JBItem();
			pJBItem->byteInfo.resize(unItemDataSize);
			pJBItem->rectBound = QRect(itemHead.rectBound.nLeft,itemHead.rectBound.nTop,itemHead.rectBound.nRight-itemHead.rectBound.nLeft,itemHead.rectBound.nBottom-itemHead.rectBound.nTop);
			pJBItem->snAngle = itemHead.snAngle;
			pJBItem->snFlag = itemHead.snFlag;
			pJBItem->snHaveTextBorder = itemHead.snHaveTextBorder;
			pJBItem->snHotSpotAttr = itemHead.snHotSpotAttr;
			pJBItem->strName = QString::fromLocal8Bit(itemHead.csName);
			pJBItem->cHotSpot = QPoint(itemHead.pointHotSpot.x,itemHead.pointHotSpot.y);
			pJBItem->unID = itemHead.unGrpID;

			if(-1 == _refFile.read(pJBItem->byteInfo.data(),unItemDataSize))
			{
				delete pJBItem;
				pJBItem = nullptr;
				_refFile.seek(unOldPos);
				return false;
			}

			// 解析Item图元
			if(false == parseItemUnit(pJBItem))
			{
				delete pJBItem;
				pJBItem = nullptr;
				_refFile.seek(unOldPos);
				return false;
			}

			_pCate->cJBItems.append(pJBItem);
			_pCate->cJBItemMap[pJBItem->unID] = pJBItem;
			pJBItem = nullptr;

			unOffset = itemHead.unNextOff;
		}

		_refFile.seek(unOldPos);

		return true;
	}

	Q_JBCategory* XJBInfoManager::isSubCate(Q_JBCategory* _pParCate, quint32 _unSubID)
	{
		if(nullptr == _pParCate)
		{
			return false;
		}

		qint32 nParCate = getLayerLevel(_pParCate->unID);
		qint32 nSubCate = getLayerLevel(_unSubID);
		quint32 Mask[]={0,0xFF,0xFFFF,0xFFFFFF};

		if(nSubCate - nParCate != 1
			|| (_pParCate->unID & Mask[nParCate]) != (_unSubID & Mask[nParCate]))
		{
			// 寻找所有子类别
			foreach(Q_JBCategory* pParCate,_pParCate->cSubCateList)
			{
				bool bRes = isSubCate(pParCate,_unSubID);
				if(true == bRes)
				{
					return pParCate;
				}
			}
			return false;
		}
		
		return _pParCate;
	}

	const Q_JBItem* XJBInfoManager::FindJBItem(quint32 _unLibID, quint32 _unItemID) const
	{
		if(false == m_cJBLibMap.contains(_unLibID) || nullptr == m_cJBLibMap[_unLibID])
		{
			// 找不到包含该库ID的库
			return nullptr;
		}

		return m_cJBLibMap[_unLibID]->FindJBItem(_unItemID);
	}

	const bool XJBInfoManager::parseItemUnit(Q_JBItem* _pItem)
	{
		// 暂时不做处理，绘制时解析
		if(nullptr == _pItem || true ==  _pItem->byteInfo.isEmpty())
		{
			return true;
		}

		{
			unsigned int uLen;                              //< 单条图元的长度
			unsigned int uCount = _pItem->byteInfo.size();   //< 总长度.

			// 获取头
			QJB_ITEM_UNIT_HEAD *pJBUH = (QJB_ITEM_UNIT_HEAD *)_pItem->byteInfo.data();

			// 遍历所有基本绘制图元进行绘制
			int kk = 0;
			bool bRes = false;
			for(;uCount > 0;uCount -= uLen)
			{
				uLen = pJBUH->unLen;

				// 解析图元信息
				{
					switch(pJBUH->snType)
					{
					case U_BLINE:   /// 曲/直线
						parseBLine(_pItem,pJBUH);
						break;
					case U_BBLINE:  /// 内含曲/直线
						parseBBLine(_pItem,pJBUH);
						//qDebug() << "U_BBLINE";
						break;
					case U_PLINE:
						parsePLine(_pItem,pJBUH);
						//qDebug() << "U_PLINE";
						break;
					case U_LINE: /// 直线
						parsePLine(_pItem,pJBUH);
						//qDebug() << "U_LINE";
						break;
					case U_RECT: /// 矩形
						parseRect(_pItem,pJBUH);
						//qDebug() << "U_RECT";
						break;
					case U_ELLI: /// 椭圆
						parseEllipse(_pItem,pJBUH);
						//qDebug() << "U_ELLI";
						break;
					case U_PIE: /// 饼
						parsePie(_pItem,pJBUH);
						//qDebug() << "U_PIE";
						break;
					case U_ARC: /// 圆弧
						parseArc(_pItem,pJBUH);
						//qDebug() << "U_ARC";
						break;
					case U_PGON:
						parsePolygon(_pItem,pJBUH);
						//qDebug() << "U_PGON";
						break;
					case U_TEXT:
						//drawText(_pUnitInfo);
						parseText(_pItem,pJBUH);
						//qDebug() << "U_TEXT";
						break;
					case U_TEXTBORDER:
						//qDebug() << "U_TEXTBORDER";
						{
							TEXT_ATTR *TA = (TEXT_ATTR *)(pJBUH + 1);

							Q_JBItemUnitInfo unitInfo;
							unitInfo.unType = pJBUH->snType;
							unitInfo.cTxtAttr = *TA;

							Q_JBUnitSegm segm;
							segm.bIsCurve = false;

							QJB_POINT *pt = (QJB_POINT *)(TA+1);

							segm.cPoints.append(QPointF(pt->x,pt->y));
							segm.cPoints.append(QPointF(pt[1].x,pt[1].y));

							unitInfo.cSegms.append(segm);
							_pItem->cDrawUnitInfo.append(unitInfo);

							pt = nullptr;
						}
						break;
					case U_BITMAP:
						{
							// 暂时不处理
						}
						break;
					default:
						break;
					}
				}

				// 移动数据指针
				pJBUH = (QJB_ITEM_UNIT_HEAD *)(((char *)pJBUH) + uLen);
			}
		}

		return true;
	}

	Q_JBItem* XJBInfoManager::CloneItem(quint32 _unLibID, quint32 _unItemID) const
	{
		const Q_JBItem* pOrItem = FindJBItem(_unLibID,_unItemID);
		if(nullptr == pOrItem)
		{
			return nullptr;
		}

		Q_JBItem* pNewItem = new Q_JBItem();
		*pNewItem = *pOrItem;
		pOrItem = nullptr;

		return pNewItem;
	}

	const bool XJBInfoManager::parseBLine(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		if(nullptr == _pJBItem || nullptr == _pUnitInfo)
		{
			return false;
		}

		const LINE_ATTR *pLAtt = (LINE_ATTR *)(_pUnitInfo + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *pLAtt;

		QJB_POINT xy[4];
		int * d,*d0,nDotNum;
		//    int CtrlDot,j;
		int CtrlDot;
		bool bClose = pLAtt->snClose;
		d=(int*)(pLAtt + 1);
		nDotNum = *d++;		// 本曲线基本点数

		checkPointCache(nDotNum);
		QJB_POINT *xy0,*xy1;
		xy1 = xy0 = m_pPointCache;
		d0 = d;

		for(;nDotNum>0; nDotNum--)
		{
			Q_JBUnitSegm segm;
			CtrlDot=*d & 0xFF;						// 本段控制点数
			if(*d & ms_nConstCurveFlag)				// 如果是曲线
			{
				segm.bIsCurve = true;
				for (int i=0;i<4;i++)
				{
					segm.cPoints.append(QPointF(0,0));
				}

				d++;
				//xy[0].x=*d++; 
				//xy[0].y=*d++;
				segm.cPoints[0].setX(*d++);
				segm.cPoints[0].setY(*d++);

				if(CtrlDot==1)						// 一个控制点
				{ 
					//xy[1].x=*d++; 
					//xy[1].y=*d++;
					segm.cPoints[1].setX(*d++);
					segm.cPoints[1].setY(*d++);

					segm.cControlPnts.append(segm.cPoints[1]);
				}  
				else								// 两个控制点
				{ 
					QPointF ctlPnt;
					ctlPnt.setX(*d++); 
					ctlPnt.setY(*d++); 
					segm.cControlPnts.append(ctlPnt);
					//xy[1].x=*d++; 
					//xy[1].y=*d++;
					segm.cPoints[1].setX(*d++);
					segm.cPoints[1].setY(*d++);
				} 

				if(nDotNum==1 && bClose)				// 最后一点且封闭
				{
					//xy[3].x=d0[1]; xy[3].y=d0[2];
					//xy[2].x=d0[3]; xy[2].y=d0[4];
					segm.cPoints[3].setX(d0[1]);
					segm.cPoints[3].setY(d0[2]);
					segm.cPoints[2].setX(d0[3]);
					segm.cPoints[2].setY(d0[4]);
				}
				else
				{
					//xy[3].x=d[1]; xy[3].y=d[2];
					//xy[2].x=d[3]; xy[2].y=d[4];
					segm.cPoints[3].setX(d[1]);
					segm.cPoints[3].setY(d[2]);
					segm.cPoints[2].setX(d[3]);
					segm.cPoints[2].setY(d[4]);
				}
				//++xy1;
			}
			else // 直线
			{
				d++;
				segm.bIsCurve = false;
				for (int i=0;i<2;i++)
				{
					segm.cPoints.append(QPointF(0,0));
				}

				//xy[0].x=*d++; 
				//xy[0].y=*d++;
				segm.cPoints[0].setX(*d++);
				segm.cPoints[0].setY(*d++);

				if(CtrlDot==1)
				{
// 					d++; 
// 					d++;
					QPointF ctlPnt;
					ctlPnt.setX(*d++); 
					ctlPnt.setY(*d++); 
					segm.cControlPnts.append(ctlPnt);
				}

				if(nDotNum==1 && bClose)			// 最后一点且封闭
				{
					//xy[1].x = d0[1];
					//xy[1].y = d0[2];
					segm.cPoints[1].setX(d0[1]);
					segm.cPoints[1].setY(d0[2]);
				}
				else
				{
					//xy[1].x = d[1]; 
					//xy[1].y = d[2];
					segm.cPoints[1].setX(d[1]);
					segm.cPoints[1].setY(d[2]);
				}

				//path.moveTo((int)xy[0].x,xy[0].y);
				//path.lineTo(xy[1].x,xy[1].y);

				//xy1->x = xy[1].x; 
				//xy1->y = xy[1].y;
				//++xy1;
			}

			unitInfo.cSegms.append(segm);

			if(nDotNum==2 && bClose==0)
			{
				break;
			}
		}

		_pJBItem->cDrawUnitInfo.append(unitInfo);

		xy0 = xy1 = nullptr;
		d = d0 = nullptr;
		pLAtt = nullptr;
		pLAtt = nullptr;

		return true;
	}

	int XJBInfoManager::browseLine(int *_pData, int flag)
	{
		int *d = _pData, DotNum;
		int TotalDot = 1;

// 		DotNum = *d++;  // 本曲线基本点数
// 
// 		for (;DotNum > 0; DotNum--) 
// 		{
// 			if (*d & ms_nConstCurveFlag) 
// 			{
// 				TotalDot+=ms_unBezierPrecision + 1;
// 			}
// 			else 
// 			{
// 				TotalDot++;
// 			}
// 			d += (*d & 0xFF)*2 + 3;
// 			if (DotNum == 2 && flag == 0)
// 				break;
// 		}
		return TotalDot;
	}

	void XJBInfoManager::checkPointCache(quint32 _unNewCount)
	{
		if(_unNewCount > m_unPointCacheCount)
		{
			memset(m_pPointCache,0,sizeof(QJB_POINT)*m_unPointCacheCount);
			//memset(ms_cQPoints,0,sizeof(QPointF)*m_unPointCacheCount);
			delete[]m_pPointCache;
			//delete[] ms_cQPoints;
			m_pPointCache = new QJB_POINT[_unNewCount];
			//ms_cQPoints = new QPointF[_unNewCount];
			m_unPointCacheCount = _unNewCount;
		}
	}

	const bool XJBInfoManager::parseBBLine(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		bool bClose = LA->snClose;

		QJB_POINT *xy0,*xy1,xy[4];
		int *d,*d0,nDotNum,nTotalDot;
		int CtrlDot,nDotCnt;
		int *DATA = (int*)(LA + 1);

		int nPolyNum = DATA[0];
		if (nPolyNum > 40) {
			nPolyNum = 40;
		}

		int *Data1[40], vNum[40];
		Data1[0] = DATA + 2;

		nTotalDot = 0;
		int i = 0;
		for (;i < nPolyNum;i++)
		{
			if (i) Data1[i] = (int*)((unsigned char*)Data1[i-1] + *(Data1[i-1]-1));

			vNum[i] = browseLine(Data1[i], bClose);
			nTotalDot += vNum[i];
		}

		checkPointCache(nTotalDot);
		xy1 = xy0 = m_pPointCache;

		i=0;
		nDotCnt=0;
		for (;i<nPolyNum;i++)
		{
			d=(int *)Data1[i];
			nDotNum=*d++;
			d0=d;

			Q_JBUnitSegm segm;
			segm.bIsCurve = false;

			for(;nDotNum>0; nDotNum--)
			{
				Q_JBUnitSegm subSegm;
				CtrlDot=*d & 0xFF;						// 本段控制点数
				if(*d & ms_nConstCurveFlag)				// 如果是曲线
				{    
					d++;
					subSegm.bIsCurve = true;
					for (int i=0;i<4;i++)
					{
						subSegm.cPoints.append(QPointF(0,0));
					}

					subSegm.cPoints[0].setX(*d++);
					subSegm.cPoints[0].setY(*d++);

					if(CtrlDot==1)
					{									// 一个控制点
						subSegm.cPoints[1].setX(*d++);
						subSegm.cPoints[1].setY(*d++);

						subSegm.cControlPnts.append(subSegm.cPoints[1]);
					}  
					else								// 两个控制点
					{ 
						QPointF ctlPnt;
						ctlPnt.setX(*d++); 
						ctlPnt.setY(*d++); 
						subSegm.cControlPnts.append(ctlPnt);
						
						subSegm.cPoints[1].setX(*d++);
						subSegm.cPoints[1].setY(*d++);
					} 

					if(nDotNum==1 && bClose)				// 最后一点且封闭
					{
						subSegm.cPoints[3].setX(d0[1]);
						subSegm.cPoints[3].setY(d0[2]);
						subSegm.cPoints[2].setX(d0[3]);
						subSegm.cPoints[2].setY(d0[4]);
					}
					else
					{
						subSegm.cPoints[3].setX(d[1]);
						subSegm.cPoints[3].setY(d[2]);
						subSegm.cPoints[2].setX(d[3]);
						subSegm.cPoints[2].setY(d[4]);
					}
				}
				else // 直线
				{
					d++;

					subSegm.bIsCurve = false;
					for (int i=0;i<2;i++)
					{
						subSegm.cPoints.append(QPointF(0,0));
					}

					subSegm.cPoints[0].setX(*d++);
					subSegm.cPoints[0].setY(*d++);
					if(CtrlDot==1)
					{
						QPointF ctlPnt;
						ctlPnt.setX(*d++); 
						ctlPnt.setY(*d++); 
						subSegm.cControlPnts.append(ctlPnt);
					}

					if(nDotNum==1 && bClose)// 最后一点且封闭
					{
						subSegm.cPoints[1].setX(d0[1]);
						subSegm.cPoints[1].setY(d0[2]);
					}
					else
					{
						subSegm.cPoints[1].setX(d[1]);
						subSegm.cPoints[1].setY(d[2]);
					}
				}

				segm.cSubSegms.append(subSegm);

				if(nDotNum==2 && bClose==0)
					break;
			}

			nDotCnt+=vNum[i];
			unitInfo.cSegms.append(segm);
		}

		_pJBItem->cDrawUnitInfo.append(unitInfo);

		d = d0 = DATA = nullptr;
		LA = nullptr;
		return true;
	}

	const bool XJBInfoManager::parsePLine(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		int * d=(int *)(LA + 1);
		int DotNum=*d++;

		QJB_POINT *pt=(QJB_POINT*)d;

		checkPointCache(DotNum);
		Q_JBUnitSegm segm;
		segm.bIsCurve = false;
		for(int i=0; i<DotNum; i++) 
		{
			segm.cPoints.append(QPointF(pt[i].x,pt[i].y));
		}
		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);
		
		pt = nullptr;
		LA = nullptr;
		d = nullptr;

		return true;
	}

	const bool XJBInfoManager::parseRect(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		Q_JBUnitSegm segm;
		segm.bIsCurve = false;
		segm.cPoints.append(QPointF(pt[0].x,pt[0].y));
		segm.cPoints.append(QPointF(pt[1].x,pt[1].y));

		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);

		pt = nullptr;
		LA = nullptr;
		return true;
	}

	const bool XJBInfoManager::parseEllipse(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		Q_JBUnitSegm segm;
		segm.bIsCurve = false;
		segm.cPoints.append(QPointF(pt[0].x,pt[0].y));
		segm.cPoints.append(QPointF(pt[1].x,pt[1].y));

		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);

		pt = nullptr;
		LA = nullptr;
		return true;
		return true;
	}

	const bool XJBInfoManager::parsePie(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);
		
		Q_JBUnitSegm segm;
		segm.bIsCurve = false;
		segm.cPoints.append(QPointF(pt[0].x, pt[0].y));
		segm.cPoints.append(QPointF(pt[1].x, pt[1].y));
		segm.cPoints.append(QPointF(pt[2].x, pt[2].y));
		segm.cPoints.append(QPointF(pt[3].x, pt[3].y));

		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);

		pt = nullptr;
		return true;
	}

	const bool XJBInfoManager::parseArc(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		QJB_POINT *pt = (QJB_POINT*)(LA + 1);

		Q_JBUnitSegm segm;
		segm.bIsCurve = false;
		segm.cPoints.append(QPointF(pt[0].x, pt[0].y));
		segm.cPoints.append(QPointF(pt[1].x, pt[1].y));
		segm.cPoints.append(QPointF(pt[2].x, pt[2].y));
		segm.cPoints.append(QPointF(pt[3].x, pt[3].y));

		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);

		pt = nullptr;
		LA = nullptr;
		return true;
	}

	const bool XJBInfoManager::parsePolygon(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		LINE_ATTR *LA = (LINE_ATTR *)(_pUnitInfo + 1);

		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cLineAttr = *LA;

		int *DATA = (int *)(LA + 1);
		int nDotNum = *DATA++;

		QJB_POINT *pt = (QJB_POINT*)(DATA);

		Q_JBUnitSegm segm;
		segm.bIsCurve = false;
		for(int i=0; i<nDotNum; i++)
		{
			segm.cPoints.append(QPointF(pt[i].x, pt[i].y));
		}

		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);

		DATA = nullptr;
		pt = nullptr;

		return true;
	}

	const bool XJBInfoManager::parseText(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo)
	{
		TEXT_ATTR *TA = (TEXT_ATTR *)(_pUnitInfo + 1);
		
		Q_JBItemUnitInfo unitInfo;
		unitInfo.unType = _pUnitInfo->snType;
		unitInfo.cTxtAttr = *TA;
		
		Q_JBUnitSegm segm;
		segm.bIsCurve = false;

		QJB_POINT *pt = (QJB_POINT *)(TA+1);

		segm.cPoints.append(QPointF(pt->x,pt->y));
		segm.strTxt.append(QString::fromLocal8Bit((char*)(pt+1)));

		unitInfo.cSegms.append(segm);
		_pJBItem->cDrawUnitInfo.append(unitInfo);

		pt = nullptr;
		return true;
	}

	const bool XJBInfoManager::ItemIsPointType(quint32 _unLibID, quint32 _unItemID)
	{
		if(true == m_cExceptiveInfos.contains(_unLibID) 
			&& true == m_cExceptiveInfos[_unLibID].contains(_unItemID))
		{
			return false;
		}

		return true;
	}

	// 读取特别Item绘制规则
	bool XJBInfoManager::readExceptive(const QString& _strXMLPath)
	{
		// 判断文件是否存在

		if(sfnFileIsExist(_strXMLPath) == false)
		{
			return false;
		}

		// 读取文件并解析
		QDomDocument doc("ItemDef");
		QFile file(_strXMLPath);
		if(!file.open(QIODevice::ReadOnly))
		{
			return false;
		}

		if(!doc.setContent(&file))
		{
			file.close();
			return false;
		}
		file.close();

		QDomElement docElem = doc.documentElement();
		QDomNode n = docElem.firstChild();
		while(!n.isNull()) 
		{
			QDomElement e = n.toElement(); // try to convert the node to an element.
			if(!e.isNull()) 
			{
				quint32 unLibID = e.attribute("LibID").toUInt();
				quint32 unItemID = e.attribute("ItemID").toUInt();
				QString strItemName = e.attribute("Name");
				
				quint32 unMinPts = 1;
				quint32 unMaxPts = 1;
				quint32 unDwType = 0;
				// 循环二级节点
				QDomNode subNode = e.firstChild();
				while(!subNode.isNull())
				{
					QDomElement subE = subNode.toElement();
					QString strTagName = subE.tagName();
					if(strTagName == "MinPoint")
					{
						unMinPts = subE.text().toUInt();
					}else if(strTagName == "MaxPoint")
					{
						unMaxPts = subE.text().toUInt();
					}else if(strTagName == "DrawType")
					{
						unDwType = subE.text().toUInt();
					}

					subNode = subNode.nextSibling();
				}

				ExceptiveInfo* pInfo = new ExceptiveInfo();
				pInfo->unLibID = unLibID;
				pInfo->unItemID = unItemID;
				pInfo->strName = strItemName;
				pInfo->unMinPoint = unMinPts;
				pInfo->unMaxPoint = unMaxPts;
				pInfo->eDwType = ExceptiveInfo::EDrawType(unDwType);

				m_cExceptiveInfos[unLibID][unItemID] = pInfo;
			}
			n = n.nextSibling();
		}

		return true;
	}

	ExceptiveInfo* XJBInfoManager::GetItemExceptiveInfo(quint32 _unLibID, quint32 _unItemID)
	{
		if(true == ItemIsPointType(_unLibID,_unItemID))
		{
			return nullptr;
		}

		return m_cExceptiveInfos[_unLibID][_unItemID];
	}

}