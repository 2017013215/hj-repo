#include "JBSerialize.h"
#include "JBWindow/JBWidget/JBTreeModel.h"

#include <QDebug>
#include "Common_Base.h"

const char CJBSerialize::JB_TAG[] = "DBLIB100";
CJBSerialize::CJBSerialize()
{

}

bool CJBSerialize::Execute(CJBTreeItem *pRootItem, const QString &sDirName)
{
    QFileInfoList fileList = OpenDir(sDirName);

    /// 如果没有查找到文件
    if(fileList.size() < 1)
    {
        return(false);
    }


    foreach (QFileInfo fileInfo, fileList)
    {
        ParseJBFile(pRootItem, fileInfo);
    }

    return true;
}

bool CJBSerialize::ParseJBFile(CJBTreeItem *pRootItem, const QFileInfo &fileInfo)
{
    FILE* fp = NULL;
    QString sTemp;

    sTemp = QDir::toNativeSeparators(fileInfo.absoluteFilePath());

    /// 打开军标文件
    fp=fopen(fileInfo.absoluteFilePath().toLocal8Bit().data(),"rb");

    if (fp != NULL)
    {
        vLayer.clear();

        /// 读取军标头
        fread(&JBLibHead,sizeof(JBLibHead),1,fp);

        /// 判断文件是否有效
        if(strncmp(JBLibHead.csFileID,JB_TAG,sizeof(JBLibHead.csFileID)) == 0)
        {
            QMap<quint32,JB_LAYER_TABLE> mapIDLayerInfo;
            /// 保存信息
            tmpJB.qstrName = tr8(JBLibHead.csFileDeclaring);
            tmpJB.nID  = JBLibHead.nID;
            tmpJB.listLayer.clear();

			QString strUid = tmpJB.qstrName + '('+QString::number(tmpJB.nID) + ')';

			CJBTreeItem *pTopItem = new CJBTreeItem(strUid, JBTreeTrunk, pRootItem);
			pTopItem->setHeadId(tmpJB.nID);

            pRootItem->appendChild(pTopItem);

            /// 读取层 信息
            for(unsigned int index = 0; index<JBLibHead.unLayerNum; ++index)
            {
                /// 读取图层表
                fread(&JBLayerTable,sizeof(JBLayerTable),1,fp);

                /// 构建第一级结构
                if(1 == GetLayerLevel(JBLayerTable.unID))
                {
                    tmpLayer.qstrName = tr8(JBLayerTable.csName);
                    tmpLayer.unID  = JBLayerTable.unID;

                    /// 有子图层时设置获取有信息为空
                    if(JBLayerTable.nGrpNum <= 0)
                    {
                        tmpLayer.bHaveInfo = false;
                    }
                    else
                    {
                        tmpLayer.bHaveInfo = true;
                        mapIDLayerInfo[JBLayerTable.unID] = JBLayerTable;
                    }

                    tmpJB.listLayer.append(tmpLayer);
                }
                else
                {
                    vLayer.push_back(JBLayerTable);
                }
            }


            /// 构建军标链
            BuildChain(pTopItem,tmpJB,vLayer,mapIDLayerInfo,fp);
			
            /// 放入军标链中
            m_vJbChain.push_back(tmpJB);
        }

        /// 关闭文件
        fclose(fp);
    }

    return true;
}

void CJBSerialize::BuildChain(CJBTreeItem *pParentItem, QT_JB &rJB, 
							  QVector<JB_LAYER_TABLE> &vLayerInfo, 
							  QMap<quint32, JB_LAYER_TABLE> &vMapInfo, FILE *pFile)
{
	CJBTreeItem *pFirstItem = NULL;

    for(QList<QT_LAYER>::iterator itor = rJB.listLayer.begin() ;
        itor != rJB.listLayer.end();
        ++itor)
    {
        /// 如果有子图层
        if(!itor->bHaveInfo)
		{
			pFirstItem = new CJBTreeItem(itor->qstrName, JBTreeTrunk, pParentItem);
			pParentItem->appendChild(pFirstItem);

            /// 获取子图层
            /// 构建第二层子目录
            for(QVector<JB_LAYER_TABLE>::const_iterator itorV = vLayerInfo.begin();
                itorV != vLayerInfo.end(); ++itorV)
            {
                if(IsChildLayer(itor->unID,itorV->unID))
                {
                    QT_LAYER tmpJBLayer;
                    tmpJBLayer.unID = itorV->unID;
                    tmpJBLayer.qstrName = tr8(itorV->csName);

					CJBTreeItem *pSecondItem = new CJBTreeItem(tmpJBLayer.qstrName, JBTreeBranch, pFirstItem);
                    pFirstItem->appendChild(pSecondItem);


                    /// 读取Group数据
                    ReadGroupInfo(pSecondItem,itorV, rJB,&tmpJBLayer,pFile);

                    itor->listChild.append(tmpJBLayer);
                }
            }
        }
        else
        {
			pFirstItem = new CJBTreeItem(itor->qstrName , JBTreeBranch, pParentItem);
			pParentItem->appendChild(pFirstItem);

            /// 查看信息是否保存到图层中
            QMap<quint32,JB_LAYER_TABLE>::iterator itorMap = vMapInfo.find(itor->unID);
            if(vMapInfo.end() != itorMap)
            {
                ReadGroupInfo(pFirstItem,itorMap.operator ->(), rJB,itor.operator ->(),pFile);
            }
        }
    }
}

/// 读取元组信息
void CJBSerialize::ReadGroupInfo(CJBTreeItem *pParentItem, const JB_LAYER_TABLE* rLayerInfo, QT_JB& rJB, QT_LAYER* rLayer, FILE *pFile)
{
    JB_GROUP_HEAD JBGroup;

    /// nGrpNum =0空类别；>0含有Num个标号；<0含有-Num个子类别

    quint32 Offset=rLayerInfo->unGrpOffset;   // 第一个标号位置

    //QT_GROUP tmpGroup;

    quint32 unReadSize=0,unGroupSize=sizeof(JBGroup);

    for(int i=0; i<rLayerInfo->nGrpNum; i++)
    {
        /// 读入标号
        fseek(pFile,Offset,SEEK_SET);

        /// 读取Group头
        fread(&JBGroup,unGroupSize,1,pFile);
        unReadSize = JBGroup.unLen - unGroupSize;

		/*
        /// 复制Group头信息
        tmpGroup.byteInfo.resize(unReadSize);
        tmpGroup.rectBound = JBGroup.rectBound;
        tmpGroup.sAngle = JBGroup.sAngle;
        tmpGroup.sFlag = JBGroup.sFlag;
        tmpGroup.sHaveTextBorder = JBGroup.sHaveTextBorder;
        tmpGroup.sHotSpotAttr = JBGroup.sHotSpotAttr;
        tmpGroup.qstrName = tr8(JBGroup.csName);
        tmpGroup.unID  = JBGroup.unGrpID;
		*/

		CJBTreeItem::DATA_PTR pData(new QT_GROUP());
		/// 复制Group头信息
		pData->byteInfo.resize(unReadSize);
		pData->rectBound		= JBGroup.rectBound;
		pData->sAngle			= JBGroup.sAngle;
		pData->sFlag			= JBGroup.sFlag;
		pData->sHaveTextBorder	= JBGroup.sHaveTextBorder;
		pData->sHotSpotAttr		= JBGroup.sHotSpotAttr;
		pData->qstrName			= tr8(JBGroup.csName);
		pData->unID				= JBGroup.unGrpID;


		CJBTreeItem *pItem = new CJBTreeItem(pData->qstrName,JBTreeLeaf,pData,pParentItem);
		pParentItem->appendChild(pItem);
		pItem->setHeadId(rJB.nID);

        /// 读取元组图形信息
        //fread(tmpGroup.byteInfo.data(),unReadSize,1,pFile);
		fread(pData->byteInfo.data(),unReadSize,1,pFile);

        //rLayer->listGropu.append(tmpGroup);
        Offset = JBGroup.unNextOff;

       // myHashGroup.insert(tmpGroup.unID, tmpGroup);
    }
}

bool CJBSerialize::IsChildLayer(quint32 unID0, quint32 unID1)
{
    static qint32 L0,L1;
    static quint32 Mask[]={0,0xFF,0xFFFF,0xFFFFFF};

   L0=GetLayerLevel(unID0);
   L1=GetLayerLevel(unID1);

   if(L1-L0!=1)
   {
       return(false);
   }

   if((unID0 & Mask[L0])==(unID1 & Mask[L0]))
   {
       return(true);
   }
   else
   {
       return(false);
   }
}

/// 打开文件夹
QFileInfoList CJBSerialize::OpenDir(const QString &sDirName)
{
    QDir containDir(sDirName);

    QStringList tmpFiters;
    tmpFiters.push_back(QString("*.tag"));
    /// 返回查找结果
    return (containDir.entryInfoList(tmpFiters,QDir::NoDotAndDotDot|QDir::Readable|QDir::Files|QDir::Dirs));
}

/// 获取图层等级
int CJBSerialize::GetLayerLevel(quint32 unID)
{
    static int S_L;

    if(unID==0) S_L=0;
    else if((unID | 0x000000FF)==0x000000FF) S_L=1;     /// 第一层
    else if((unID | 0x0000FFFF)==0x0000FFFF) S_L=2;     /// 第二层
    else if((unID | 0x00FFFFFF)==0x00FFFFFF) S_L=3;     /// 第三层
    else S_L=4;                                         /// 第四层
    return(S_L);
}
// 
// void CJBSerialize::ClearChain()
// {
// 	m_vJbChain.clear();
// }
// 
// QList<QT_JB> CJBSerialize::GetChain() const
// {
// 	return m_vJbChain;
// }
