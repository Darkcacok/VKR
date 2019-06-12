#ifndef ISOITEMTREE_H
#define ISOITEMTREE_H


#include <QAbstractItemModel>
#include <QIcon>

#include "fs.h"

class IsoItemTreeModel : public QAbstractItemModel
{
public:
    IsoItemTreeModel(QObject *parent);


    void setRooteNode(fs::Node *node);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    fs::Node *nodeFromeIndex(const QModelIndex &index) const;

    int insertRow(const QModelIndex &index, fs::Node *node);

private:
    fs::Node *rootNode;


    /*Icon*/
    static const  QIcon folderIcon;
    static const QIcon fileIcon;

};

#endif // ISOITEMTREE_H
