#include "isoitemtree.h"

const QIcon IsoItemTree::folderIcon = QIcon("../src/icons/folder.svg");
const QIcon IsoItemTree::fileIcon = QIcon("../src/icons/text-x-generic.svg");

IsoItemTree::IsoItemTree(QObject *parent)
    :QAbstractItemModel(parent)
{
    rootNode = NULL;
}

void IsoItemTree::setRooteNode(fs::Node *node)
{
    rootNode = node;
    //reset();
}

QModelIndex IsoItemTree::index(int row, int column, const QModelIndex &parent) const
{
    if(!rootNode)
        return QModelIndex();
    fs::Node *parentNode = nodeFromeIndex(parent);

    return createIndex(row, column, static_cast<fs::Dir*>(parentNode)->getChild(row));
}

QModelIndex IsoItemTree::parent(const QModelIndex &child) const
{
    fs::Node *node = nodeFromeIndex(child);

    if(!node)
        return QModelIndex();

    fs::Dir *parentNode = node->getParent();
    if(!parentNode)
        return QModelIndex();

    fs::Dir *grandparentNode = parentNode->getParent();
    if(!grandparentNode)
        return QModelIndex();

    int row  = grandparentNode->indexOf(parentNode);

    return createIndex(row, child.column(), parentNode);
}

int IsoItemTree::rowCount(const QModelIndex &parent) const
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

int IsoItemTree::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant IsoItemTree::data(const QModelIndex &index, int role) const
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

            char buf[4];
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

fs::Node *IsoItemTree::nodeFromeIndex(const QModelIndex &index) const
{
    if(index.isValid())
    {
        return static_cast<fs::Node*>(index.internalPointer());
    }
    else {
        return rootNode;
    }
}

int IsoItemTree::insertRow(const QModelIndex &index, fs::Node *node)
{
    if(index.isValid())
    {
        fs::Node *currentNode = nodeFromeIndex(index);

        if(currentNode->getType() == fs::ISO_DIR)
        {

            beginInsertRows(index, static_cast<fs::Dir*>(currentNode)->getSize(), static_cast<fs::Dir*>(currentNode)->getSize());
            int ret = static_cast<fs::Dir*>(currentNode)->addChild(node);

            if(ret < 0)
                return -1;

            endInsertRows();
        }
        else
        {
            fs::Dir *dir = currentNode->getParent();

            beginInsertRows(index.parent(), dir->getSize() - 1, dir->getSize() - 1);

            int ret = dir->addChild(node);

            if(ret < 0)
                return -1;

            endInsertRows();
        }
    }
    else
    {
        beginInsertRows(QModelIndex(), ((fs::Dir*)rootNode)->getSize(), ((fs::Dir*)rootNode)->getSize());

        int ret = ((fs::Dir*)rootNode)->addChild(node);

        if(ret < 0)
            return -1;

        endInsertRows();
    }

    return 1;
}


QVariant IsoItemTree::headerData(int section, Qt::Orientation orientation, int role) const
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
