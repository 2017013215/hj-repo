#pragma once
#ifndef SCENARIOMERGER_H
#define SCENARIOMERGER_H

#include <QObject>
#include <QDomDocument>

class ScenarioMerger : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioMerger(QObject *parent = 0);

    // 合并场景文件
    bool mergeScenarios(const QString& redScnPath, 
                       const QString& blueScnPath,
                       const QString& outputPath);

private:
    // 查找指定内容的注释节点
    QDomNode findCommentNode(QDomElement parent, const QString& content) const;

    // 提取注释后的所有Item节点
    QList<QDomNode> extractItemsAfterComment(QDomNode comment) const;

    // 在指定注释后插入节点
    void insertItemsAfterComment(QDomDocument& doc, 
                               QDomElement& parent, 
                               QDomNode& comment, 
                               const QList<QDomNode>& items) const;

    // 获取Time节点
    QDomElement getTimeNode(QDomDocument& doc) const;
};

#endif // SCENARIOMERGER_H
