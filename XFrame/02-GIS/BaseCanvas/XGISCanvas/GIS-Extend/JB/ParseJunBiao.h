#ifndef PARSEJUNBIAO_H
#define PARSEJUNBIAO_H

#include <QDomDocument>
#include <QDomNode>
#include <QMap>

#include "JBWindow/JunBiaoInfo.hpp"
class CJBTreeItem;

class ParseJunBiao
{
public:
    ParseJunBiao();
    /**
     * @brief addJunBiaoNew  添加新军标
     */
    void addJunBiaoNew(CJBTreeItem *root);

    quint32 JBTagID() const;

    /**
     * @brief getJunBiaoGroup   获取军标的字节数组
     * @param nGroupID            军标号Group号
     * @return                             返回军标数组
     */
    JB_GROUP getJunBiaoGroup(quint32 nGroupID);


    static ShapeRect GetRect(QDataStream& in);
    static ShapeEllipse  getEllipse(QDataStream& in);
    static ShapeArg getArg(QDataStream& in);
    static ShapeText getText(QDataStream& in);
    static ShapePolygon getPolygon(QDataStream& in);
    static ShapeLine getLine(QDataStream& in);
private:
    /**
     * @brief readJunBiaoLibNew         读取新增的军标节点
     * @param docNode                       XML的DOC
     * @param pItem                            军标树中的root节点
     */
    void readJunBiaoLibNew(QDomNode& docNode, CJBTreeItem* pItem);

    /**
     * @brief readGroups                    读取军标组数据
     * @param nodeList                      军标组列表
     * @param pItem                          军标组的树枝节点
     */
    void readGroups(QDomNodeList& nodeList,CJBTreeItem* pItem);

    /**
     * @brief readShapes        读取图元信息
     * @param elem               存放图元信息的xml元素
     * @param pItem              添加图元对应节点
     */
    void readShapes(QDomElement& elem, QByteArray &array, CJBTreeItem* pItem);
private:
    QMap<uint,JB_GROUP> m_mpGroup;
    quint32 m_nTagID;
};

#endif // PARSEJUNBIAO_H
