#ifndef CJBSERIALIZE_H
#define CJBSERIALIZE_H

#include <QVector>
#include <QString>
#include <QDir>

#include "JBType.h"

class CJBTreeItem;

class CJBSerialize
{
public:
    CJBSerialize();

    bool Execute(CJBTreeItem *pRootItem, const QString &sDirName);

private:

    /**
     * @brief 解析军标文件
     */
    bool ParseJBFile(CJBTreeItem *pRootItem, const QFileInfo &fileInfo);


    /**
     * @brief 构建军标链
     */
    void BuildChain(CJBTreeItem *pParentItem, QT_JB &rJB, QVector<JB_LAYER_TABLE>& vLayerInfo, QMap<quint32, JB_LAYER_TABLE> &vMapInfo,FILE* pFile);

    /**
     * @brief 读取军标信息
     * @param rLayerInfo
     * @param rLayer
     */
    void ReadGroupInfo(CJBTreeItem *pParentItem,const JB_LAYER_TABLE* rLayerInfo, QT_JB& rJB, QT_LAYER* rLayer, FILE *pFile);

    /**
     * @brief L1是否L0的子层
     * @param unID0
     * @param unID1
     * @return
     */
    bool IsChildLayer(quint32 unID0,quint32 unID1);

    /**
     * @brief 打开文件夹
     * @param sDirName
     */
    QFileInfoList OpenDir(const QString &sDirName);

    /**
     * @brief 获取层次信息
     * @param unID
     * @return
     */
    int GetLayerLevel(quint32 unID);

// 	void ClearChain();
// 
// 	QList<QT_JB> GetChain() const;

private:
    QVector<JB_LAYER_TABLE> vLayer;
    JB_HEAD                 JBLibHead;
    JB_LAYER_TABLE          JBLayerTable;

    QT_JB                   tmpJB;
    QT_LAYER                tmpLayer;
    QList<QT_JB>            m_vJbChain;

    const static  char      JB_TAG[];

};

#endif // CJBSERIALIZE_H
