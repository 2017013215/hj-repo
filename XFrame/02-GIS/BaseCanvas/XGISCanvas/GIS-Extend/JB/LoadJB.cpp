#include <QApplication>
#include <QMessageBox>
#include <QMap>
#include "LoadJB.h"
#include "JBType.h"
#include "Common_Base.h"
const char CLoadJB::JB_TAG[] = "DBLIB100";

CLoadJB::CLoadJB():m_bLoad(false)
{
}

/// 返回军标的指针 
CLoadJB* CLoadJB::GetInstance()
{
    static CLoadJB tmpLoadJB;
    return(&tmpLoadJB);
}

/// 加载军标
bool CLoadJB::Load(const QString &sDirName)
{
    QString tmpDir;

    /// 如果没有填写路径，则直接用可执行文件所在的路径
    if(sDirName.isEmpty())
    {
        tmpDir = QApplication::applicationDirPath();
    }
    else
    {
        tmpDir = sDirName;
    }

    /// 如果已加载且文件路径相同，则直接返回
    if(m_sDirName == tmpDir && m_bLoad)
    {
        return(m_bLoad);
    }

    QFileInfoList fileList = OpenDir(tmpDir);

    /// 如果没有查找到文件
    if(fileList.size() < 1)
    {
        return(false);
    }

    m_sDirName = tmpDir;
    m_bLoad = true;

    /// 读取军标文件
    ReadJB(fileList);
	return true;
}

/// 打开文件夹
QFileInfoList CLoadJB::OpenDir(const QString &sDirName)
{
    QDir containDir(sDirName);

    QStringList tmpFiters;
    tmpFiters.push_back(QString("*.tag"));
    /// 返回查找结果
    return (containDir.entryInfoList(tmpFiters,QDir::NoDotAndDotDot|QDir::Readable|QDir::Files|QDir::Dirs));
}

/// 读取军标文件
void CLoadJB::ReadJB(const QFileInfoList &allFile)
{
    QVector<JB_LAYER_TABLE> vLayer;


    JB_HEAD JBLibHead;
    JB_LAYER_TABLE JBLayerTable;

    FILE* fp = NULL;
    QString sTemp;

    QT_JB tmpJB;
    QT_LAYER tmpLayer;

    foreach (QFileInfo fileInfo, allFile)
    {
        sTemp = QDir::toNativeSeparators(fileInfo.absoluteFilePath());

        /// 打开军标文件
        fp=fopen(fileInfo.absoluteFilePath().toLocal8Bit().data(),"rb");
        if(fp!=NULL)
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
                BuildChain(tmpJB,vLayer,mapIDLayerInfo,fp);

                /// 放入军标链中
                m_vJbChain.push_back(tmpJB);
            }

            /// 关闭文件
            fclose(fp);
        }
    }
}

/// 构建链表
void CLoadJB::BuildChain(QT_JB& rJB, QVector<JB_LAYER_TABLE> &vLayerInfo, QMap<quint32,JB_LAYER_TABLE>& vMapInfo, FILE *pFile)
{
    for(QList<QT_LAYER>::iterator itor = rJB.listLayer.begin() ;
        itor != rJB.listLayer.end();
        ++itor)
    {
        /// 如果有子图层
        if(!itor->bHaveInfo)
        {
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

                    /// 读取Group数据
                    ReadGroupInfo(itorV,&tmpJBLayer,pFile);

                    itor->listChild.append(tmpJBLayer);
                }
            }
        }
        else
        {
            /// 查看信息是否保存到图层中
            QMap<quint32,JB_LAYER_TABLE>::iterator itorMap = vMapInfo.find(itor->unID);
            if(vMapInfo.end() != itorMap)
            {
                ReadGroupInfo(itorMap.operator ->(),itor.operator ->(),pFile);
            }
        }
    }
}

/// 读取元组信息
void CLoadJB::ReadGroupInfo(const JB_LAYER_TABLE *rLayerInfo, QT_LAYER *rLayer, FILE* pFile)
{
    JB_GROUP_HEAD JBGroup;

    /// nGrpNum =0空类别；>0含有Num个标号；<0含有-Num个子类别

    quint32 Offset=rLayerInfo->unGrpOffset;   // 第一个标号位置

    QT_GROUP tmpGroup;

    quint32 unReadSize=0,unGroupSize=sizeof(JBGroup);

    for(int i=0; i<rLayerInfo->nGrpNum; i++)
    {
        /// 读入标号
        fseek(pFile,Offset,SEEK_SET);

        /// 读取Group头
        fread(&JBGroup,unGroupSize,1,pFile);
        unReadSize = JBGroup.unLen - unGroupSize;

        /// 复制Group头信息
        tmpGroup.byteInfo.resize(unReadSize);
        tmpGroup.rectBound = JBGroup.rectBound;
        tmpGroup.sAngle = JBGroup.sAngle;
        tmpGroup.sFlag = JBGroup.sFlag;
        tmpGroup.sHaveTextBorder = JBGroup.sHaveTextBorder;
        tmpGroup.sHotSpotAttr = JBGroup.sHotSpotAttr;
        tmpGroup.qstrName = tr8(JBGroup.csName);
        tmpGroup.unID  = JBGroup.unGrpID;

        /// 读取元组图形信息
        fread(tmpGroup.byteInfo.data(),unReadSize,1,pFile);

        rLayer->listGropu.append(tmpGroup);
        Offset = JBGroup.unNextOff;

        myHashGroup.insert(tmpGroup.unID, tmpGroup);
    }
}

/// 删除链表
void CLoadJB::ClearChain()
{
    m_vJbChain.clear();
}

bool CLoadJB::GetGroup(quint32 unId, QT_GROUP &rGroup)
{
    QHash<quint32,QT_GROUP>::iterator itr = myHashGroup.find(unId);
    if (itr != myHashGroup.end())
    {
        rGroup = itr.value();
        return true;
    }
    return false;

}

/// 获取图层等级
int CLoadJB::GetLayerLevel(quint32 unID)
{
    static int S_L;

    if(unID==0) S_L=0;
    else if((unID | 0x000000FF)==0x000000FF) S_L=1;    /// 第一层
    else if((unID | 0x0000FFFF)==0x0000FFFF) S_L=2;   /// 第二层
    else if((unID | 0x00FFFFFF)==0x00FFFFFF) S_L=3;  /// 第三层
    else S_L=4;                                                                       /// 第四层
    return(S_L);
}

/// L1是否L0的子层
bool CLoadJB::IsChildLayer(quint32 unID0,quint32 unID1)
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
