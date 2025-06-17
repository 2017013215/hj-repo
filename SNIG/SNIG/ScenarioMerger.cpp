#include "StdAfx.h"
#include "ScenarioMerger.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

ScenarioMerger::ScenarioMerger(QObject *parent) : QObject(parent)
{
}

bool ScenarioMerger::mergeScenarios(const QString& redScnPath, 
                                  const QString& blueScnPath,
                                  const QString& outputPath)
{
    // 加载红方场景文件
    QFile redFile(redScnPath);
    if (!redFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开红方场景文件:" << redScnPath;
        return false;
    }
    
    QDomDocument bothDoc;
    if (!bothDoc.setContent(&redFile)) {
        qDebug() << "解析红方场景文件失败:" << redScnPath;
        redFile.close();
        return false;
    }
    redFile.close();

    // 加载蓝方场景文件
    QFile blueFile(blueScnPath);
    if (!blueFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开蓝方场景文件:" << blueScnPath;
        return false;
    }
    
    QDomDocument blueDoc;
    if (!blueDoc.setContent(&blueFile)) {
        qDebug() << "解析蓝方场景文件失败:" << blueScnPath;
        blueFile.close();
        return false;
    }
    blueFile.close();

    // 获取Time节点
    QDomElement bothTime = getTimeNode(bothDoc);
    QDomElement blueTime = getTimeNode(blueDoc);
    
    if (bothTime.isNull() || blueTime.isNull()) {
        qDebug() << "未找到Time节点";
        return false;
    }

    // 需要提取的蓝方信息类别
    QStringList blueSections;
    blueSections << QString::fromLocal8Bit("蓝方初始态势") << QString::fromLocal8Bit("控制对象信息") << QString::fromLocal8Bit("任务信息") << QString::fromLocal8Bit("编队信息");

    // 遍历蓝方信息类别
    foreach (const QString& section, blueSections) {
        // 在蓝方文件中查找对应的注释节点
        QDomNode blueComment = findCommentNode(blueTime, section);
        if (!blueComment.isComment()) continue;

        // 提取蓝方信息节点
        QList<QDomNode> blueItems = extractItemsAfterComment(blueComment);
        if (blueItems.isEmpty()) continue;

        // 在红方文件中查找对应的注释节点
        QDomNode bothComment = findCommentNode(bothTime, section);
        if (!bothComment.isComment()) continue;

        // 将蓝方节点插入到红方文件中
        insertItemsAfterComment(bothDoc, bothTime, bothComment, blueItems);
    }

    // 保存合并后的文件
    QFile bothFile(outputPath);
    if (!bothFile.open(QIODevice::WriteOnly)) {
        qDebug() << "无法创建输出文件:" << outputPath;
        return false;
    }
    
    QTextStream out(&bothFile);
    out.setCodec("GB2312");
    bothDoc.save(out, 4, QDomNode::EncodingFromDocument);
    bothFile.close();

    return true;
}

QDomNode ScenarioMerger::findCommentNode(QDomElement parent, const QString& content) const
{
    QDomNode node = parent.firstChild();
    while (!node.isNull()) {
        if (node.isComment() && node.nodeValue().contains(content)) {
            return node;
        }
        node = node.nextSibling();
    }
    return QDomNode(); // 返回空节点
}

QList<QDomNode> ScenarioMerger::extractItemsAfterComment(QDomNode comment) const
{
    QList<QDomNode> result;
    QDomNode node = comment.nextSibling();
    
    while (!node.isNull()) {
        if (node.isComment()) break; // 遇到下一个注释停止
        
        if (node.nodeName() == "Item") {
            result.append(node);
        }
        node = node.nextSibling();
    }
    return result;
}

void ScenarioMerger::insertItemsAfterComment(QDomDocument& doc, 
                                           QDomElement& parent, 
                                           QDomNode& comment, 
                                           const QList<QDomNode>& items) const
{
    QDomNode refNode = comment.nextSibling();
    
    foreach (const QDomNode& item, items) {
        QDomNode imported = doc.importNode(item, true);
        parent.insertAfter(imported, refNode);
        refNode = imported; // 更新参考节点位置
    }
}

QDomElement ScenarioMerger::getTimeNode(QDomDocument& doc) const
{
    QDomNodeList timeNodes = doc.elementsByTagName("Time");
    if (!timeNodes.isEmpty()) {
        return timeNodes.at(0).toElement();
    }
    return QDomElement();
}