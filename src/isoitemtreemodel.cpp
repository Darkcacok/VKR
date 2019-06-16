#include "isoitemtreemodel.h"

const QIcon IsoItemTreeModel::folderIcon = QIcon("../src/icons/folder.svg");
const QIcon IsoItemTreeModel::fileIcon = QIcon("../src/icons/text-x-generic.svg");

IsoItemTreeModel::IsoItemTreeModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    rootNode = NULL;
}

void IsoItemTreeModel::setRooteNode(fs::Dir *node)
{
    rootNode = node;
    //reset();
}

QModelIndex IsoItemTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!rootNode)
        return QModelIndex();
    fs::Node *parentNode = nodeFromeIndex(parent);

    return createIndex(row, column, static_cast<fs::Dir*>(parentNode)->getChild(row));
}

QModelIndex IsoItemTreeModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    fs::Node *childItem = static_cast<fs::Node*>(child.internalPointer());
    if(!childItem)
        return QModelIndex();

    fs::Node *parentItem = childItem->getParent();
    if(!parentItem)
        return QModelIndex();

    if(parentItem == rootNode)
        return QModelIndex();

    return createIndex(parentItem->getParent()->indexOf(parentItem), 0, parentItem);

    /*fs::Node *node = nodeFromeIndex(child);

    if(!node)
        return QModelIndex();

    fs::Dir *parentNode = node->getParent();
    if(!parentNode)
        return QModelIndex();

    fs::Dir *grandParentNode = parentNode->getParent();
    if(!grandParentNode)
        return QModelIndex();

    int row  = grandParentNode->indexOf(parentNode);

    return createIndex(row, 0, parentNode);*/
}

int IsoItemTreeModel::rowCount(const QModelIndex &parent) const
{
    fs::Node *parentNode = nodeFromeIndex(parent);

    if(!parentNode)
        return 0;

    if(parentNode->getType() == fs::ISO_DIR)
        return static_cast<fs::Dir*>(parentNode)->getSize();
    else {
        return 0;
    }
}

int IsoItemTreeModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant IsoItemTreeModel::data(const QModelIndex &index, int role) const
{
    fs::Node *node = nodeFromeIndex(index);

    if(!node)
        return QVariant();

    fs::NodeType type = node->getType();

    if(role == Qt::DecorationRole && index.column() == 0)
    {
        switch (type)
        {
        case fs::ISO_DIR:
            return folderIcon;
        case fs::ISO_FILE:
            return fileIcon;
        default:
            return  fileIcon;
        }

    }

    if(role != Qt::DisplayRole)
        return QVariant();

    if(index.column() == 0)
        return node->getName().c_str();
    else if(index.column() == 1)
    {
        switch (node->getType()) {
        case fs::ISO_FILE:
            return tr("Файл");
        case fs::ISO_DIR:
            return  tr("Папка");
        case fs::ISO_SYMLINK:
            return tr("Символическая ссылка");
        default:
            return tr("Unknown");
        }
    }
    else if(index.column() == 2)
    {
        if(node->getType() == fs::ISO_DIR)
        {
            int size = node->getSize();
            QString str(QString::number(size));

            int remainder = size%100;

check:
            if(remainder == 0)
                str += " элемнтов";
            else if(remainder == 1)
                str += " элемент";
            else if(remainder >=2 && remainder <=4)
                str += " элемента";
            else if(remainder >=5 && remainder <=19)
                str += " элементов";
            else
            {
                remainder = remainder % 10;\
                goto check;
            }

            return str;
        }
        else
        {
            char scales[] = "bkmgtp";
            char scale_c = scales[0];
            float size = node->getSize();
            QString str;

            for(int i = 1; scales[i]!= '\0'; ++i)
            {
                if(size <= 1024)
                    break;
                size /= 1024;
                scale_c = scales[i];
            }

            char buf[7];
            sprintf(buf, "%.2f", size);
            str += buf;

            switch (scale_c)
            {
            case 'b':
                str += " byte";
                break;
            case 'k':
                str += " KiB";
                break;
            case 'm':
                str += " MiB";
                break;
            case 'g':
                str += " GiB";
                break;
            case 't':
                str += " TiB";
                break;
            case 'p':
                str += " PiB";
                break;
            }
            return str;
        }

        return QVariant();
    }
}

fs::Node *IsoItemTreeModel::nodeFromeIndex(const QModelIndex &index) const
{
    if(index.isValid())
    {
        return static_cast<fs::Node*>(index.internalPointer());
    }
    else {
        return rootNode;
    }
}

int IsoItemTreeModel::insertRow(const QModelIndex &index, fs::Node *node)
{
    if(index.isValid())
    {
        fs::Node *currentNode = nodeFromeIndex(index);

        if(currentNode->getType() == fs::ISO_DIR)
        {

            beginInsertRows(index.siblingAtColumn(0), static_cast<fs::Dir*>(currentNode)->getSize(), static_cast<fs::Dir*>(currentNode)->getSize());
            int ret = static_cast<fs::Dir*>(currentNode)->addChild(node);

            if(ret < 0)
                return -1;

            endInsertRows();
        }
        else
        {
            fs::Dir *dir = currentNode->getParent();

            if(dir->getParent() == NULL)
            {
                beginInsertRows(QModelIndex(), dir->getSize(), dir->getSize());

                int ret = dir->addChild(node);

                if(ret < 0)
                    return -1;

                endInsertRows();
            }
            else {
                beginInsertRows(index.siblingAtColumn(0).parent(), dir->getSize(), dir->getSize());

                int ret = dir->addChild(node);

                if(ret < 0)
                    return -1;

                endInsertRows();
            }
        }
    }
    else
    {
        beginInsertRows(QModelIndex(), rootNode->getSize(), rootNode->getSize());

        int ret = rootNode->addChild(node);

        if(ret < 0)
            return -1;

        endInsertRows();
    }

    return 1;
}

int IsoItemTreeModel::deleteRow(const QModelIndex &index)
{
    if(index.isValid())
    {
        fs::Node *node = nodeFromeIndex(index);

        fs::Dir *dir = node->getParent();

        beginRemoveRows(index.parent(), dir->indexOf(node), dir->indexOf(node));
        dir->deleteChild(node);
        endRemoveRows();
    }

    return 0;
}


QVariant IsoItemTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("Название");
        }
        else if (section == 1) {
            return tr("Тип");
        }
        else if(section == 2){
            return tr("Размер");
        }
    }
    return QVariant();
}
