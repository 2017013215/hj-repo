#include <QDebug>
#include "JBGroupReader.h"
#include "ParseJunBiao.h"
#include "Common_Base.h"
#include "JBWindow/JBWidget/JBTreeModel.h"



ParseJunBiao::ParseJunBiao()
    : m_nTagID(-1)
{

}
void ParseJunBiao::addJunBiaoNew(CJBTreeItem* root)
{
    CJBTreeItem* rootItem = root;
    QDomDocument doc;
    QFile file("data/JB/JunBiaoNew.xml");
    QString error;
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        ///读取新军标失败
        return ;
    }
    if(!doc.setContent(&file,&error))
    {
        file.close();
        return ;
    }
    QDomElement docElem = doc.documentElement();

    readJunBiaoLibNew(docElem,rootItem);
}

quint32 ParseJunBiao::JBTagID() const
{
    return m_nTagID;
}

JB_GROUP ParseJunBiao::getJunBiaoGroup(quint32 nGroupID)
{
   QMap<uint,JB_GROUP>::iterator it = m_mpGroup.find(nGroupID);
    return it != m_mpGroup.end() ? *it:JB_GROUP();
}

ShapeRect ParseJunBiao::GetRect(QDataStream &in)
{
    ShapeRect rect;
    in>>rect;
    return rect;
}

ShapeEllipse ParseJunBiao::getEllipse(QDataStream &in)
{
    ShapeEllipse ellipse;
    in >> ellipse;
    return ellipse;
}

ShapeArg ParseJunBiao::getArg(QDataStream &in)
{
    ShapeArg arg;
    in >> arg;
    return arg;
}

ShapeText ParseJunBiao::getText(QDataStream &in)
{
    ShapeText text;
    in >> text;
    return text;
}

ShapePolygon ParseJunBiao::getPolygon(QDataStream &in)
{
    ShapePolygon pol;
    in >> pol;
    return pol;
}

ShapeLine ParseJunBiao::getLine(QDataStream &in)
{
    ShapeLine line;
    in >> line;
    return line;
}

///读整个xml的doc
void ParseJunBiao::readJunBiaoLibNew(QDomNode &docNode, CJBTreeItem *pItem)
{
    QDomElement  docElem = docNode.toElement();

    qDebug() << docNode.toElement().tagName();
    QString strName;
    uint unTagID = 0;
    if(docElem.hasAttribute("name"))
    {
        strName = docElem.attribute("name");
        qDebug() << strName;
    }
    if(docElem.hasAttribute("TagID"))
    {
        m_nTagID = unTagID = docElem.attribute("TagID").toUInt();
        qDebug() << unTagID;
    }
    if(strName.isEmpty() || unTagID == 0)
    {
        return;
    }
   //qDebug() << pItem;
    ///根节点（新增军标）
    CJBTreeItem* item  = new CJBTreeItem(strName + "(" + QString::number(unTagID) + ")",JBTreeTrunk,pItem);
    item->setHeadId(unTagID);
    pItem->appendChild(item);
    readGroups(docElem.childNodes(),item);
}

///读取军标数据的组
void ParseJunBiao::readGroups(QDomNodeList &nodeList, CJBTreeItem *pItem)
{

    for(int i = 0;i < nodeList.count();i++)
    {
        QDomElement elem = nodeList.at(i).toElement();
        QString strType;
        ///不是group节点不处理
        if(elem.tagName()!="Group")
        {
            continue;
        }
        ///信号和目标类型
        strType = elem.attribute("type");
        if(strType.isEmpty())
        {
            continue;
        }
        CJBTreeItem* pItemNew = 0;
        if(!pItem->findChildHas(strType))
        {
             ///信号或目标组
             pItemNew = new CJBTreeItem(strType,JBTreeGroup,pItem);
        }
        else
        {
             pItemNew = pItem->findChild(strType);
        }
        ///军标节点组
        JB_GROUP stGroup;

        if(elem.hasAttribute("name"))
        {
            stGroup.strName = elem.attribute("name");
        }
        CJBTreeItem* pGroupItem = new CJBTreeItem(stGroup.strName,JBTreeData,pItemNew);
        stGroup.unTagId = pItem->headId();
        pGroupItem->setHeadId(pItemNew->headId());
        pGroupItem->setDesc(elem.attribute("name"));
        if(elem.hasAttribute("GroupID"))
        {
            stGroup.unGroupId = elem.attribute("GroupID").toInt();
            pGroupItem->setGroupId(stGroup.unGroupId);
        }
//        qDebug() << stGroup.strName;
//        qDebug() << stGroup.unGroupId;
//        qDebug() << stGroup.unTagId;

        readShapes(elem,stGroup.shapeInfo,pGroupItem);
        m_mpGroup.insert(stGroup.unGroupId,stGroup);
    }
}

void ParseJunBiao::readShapes(QDomElement & elem, QByteArray& array,CJBTreeItem *pItem)
{
    QDomNodeList shapeList = elem.childNodes();
    for(int i = 0;i < shapeList.count();++i)
    {
        QDomElement elemShape = shapeList.at(i).toElement();
        QString strType;
        if(elemShape.tagName() != tr8("Shape"))
        {
            continue;
        }
        if(elemShape.hasAttribute("type"))
        {
            strType = elemShape.attribute("type");
        }
        if(strType.isEmpty())
        {
            continue;
        }
        qDebug() << strType;
        QSharedPointer<JBGroupReader> reader;
        if(strType == tr8("Arg"))
        {
            reader = QSharedPointer<JBGroupReader>(new ArgReader);
        }
        else if(strType == tr8("Text"))
        {
            reader = QSharedPointer<JBGroupReader>(new TextReader);
        }
        else if(strType == tr8("Rect"))
        {
            reader = QSharedPointer<JBGroupReader>(new RectReader);
        }
        else if(strType == tr8("Polygon"))
        {
            reader = QSharedPointer<JBGroupReader>(new PolygonReader);
        }
        else if(strType == tr8("Line"))
        {
            reader = QSharedPointer<JBGroupReader>(new LineReader);
        }
        else if(strType == tr8("Ellipse"))
        {
            reader = QSharedPointer<JBGroupReader>(new EllipseReader);
        }
        array +=  reader->readGroup(elemShape);
    }
}
