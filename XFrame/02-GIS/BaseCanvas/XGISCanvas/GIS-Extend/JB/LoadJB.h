#ifndef CLOADJB_H
#define CLOADJB_H

#include <QDir>
#include "JBType.h"

#include <QHash>

class CLoadJB
{
public:
    /**
     * @brief 获取加载军标文件的单例
     * @return
     */
    static CLoadJB* GetInstance();

    /**
     * @brief 加载军标文件
     * @param sFileName  军标文件所在的路径
     * @return
     * @attention 程序会自动寻找 指定路径下的所有军标文件
     *                   不包括 子文件夹
     */
    bool Load(const QString& sDirName="");

    /**
     * @brief 查看是否已加载军标
     * @return
     */
    bool IsLoad(){return (m_bLoad);}

    /**
     * @brief 获取链表
     * @return
     */
    const QList<QT_JB>& GetChain(){return(m_vJbChain);}

    /**
     * @brief 清空军标链
     */
    void ClearChain();

    bool GetGroup(quint32 unId, QT_GROUP &rGroup);

private:
    CLoadJB();

    /**
     * @brief 打开文件夹
     * @param sDirName
     */
    QFileInfoList OpenDir(const QString &sDirName);

    /**
     * @brief 读取军标文件
     * @param allFile
     */
    void ReadJB(const QFileInfoList& allFile);

    /**
     * @brief 获取层次信息
     * @param unID
     * @return
     */
    int GetLayerLevel(quint32 unID);

    /**
     * @brief L1是否L0的子层
     * @param unID0
     * @param unID1
     * @return
     */
    bool IsChildLayer(quint32 unID0,quint32 unID1);

    /**
     * @brief 构建军标链
     */
    void BuildChain(QT_JB &rJB, QVector<JB_LAYER_TABLE>& vLayerInfo, QMap<quint32, JB_LAYER_TABLE> &vMapInfo,FILE* pFile);

    /**
     * @brief 读取军标信息
     * @param rLayerInfo
     * @param rLayer
     */
    void ReadGroupInfo(const JB_LAYER_TABLE* rLayerInfo, QT_LAYER* rLayer, FILE *pFile);
private:
    QString                 m_sDirName;   /// 军标路径
    bool                    m_bLoad;        /// 是否已加载
    QList<QT_JB>            m_vJbChain;
    const static  char      JB_TAG[];

    QHash<quint32,QT_GROUP> myHashGroup;

};

#endif // CLOADJB_H
