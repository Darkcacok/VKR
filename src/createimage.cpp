#include "createimage.h"

CreateImage::CreateImage(QWidget *parent) :
    QWidget(parent)
{
    createWindow();

    m_root = new fs::Dir("/home/user");
}

CreateImage::~CreateImage()
{
}

int CreateImage::createWindow()
{
    window = new QWidget(this);

    v_layout = new QVBoxLayout();
    h_layout_top = new QHBoxLayout();
    h_layout_buttom = new QHBoxLayout();

    add_file = new QPushButton("Добавить файл");
    add_dir = new QPushButton("Добавить папку");
    delete_node = new QPushButton("Удалить");
    record = new QPushButton("Записать");

    connect(add_file, SIGNAL(clicked()), this, SLOT(addFile()));
    connect(add_dir, SIGNAL(clicked()), this, SLOT(addDir()));
    connect(delete_node, SIGNAL(clicked()), this, SLOT(deleteNode()));

    choose_disc = new QComboBox();

    /************TreeWidget****************************/
    nodes_view = new QTreeWidget();
    top_level_item = new QTreeWidgetItem();

    nodes_view->setColumnCount(column);
    nodes_view->setHeaderLabels(QStringList{"Название", "Тип", "Размер"});


    h_layout_top->addWidget(add_file);
    h_layout_top->addWidget(add_dir);
    h_layout_top->addWidget(delete_node);
    h_layout_top->addItem(new QSpacerItem(40, 20,  QSizePolicy::Expanding, QSizePolicy::Minimum));

    h_layout_buttom->addWidget(choose_disc, 1);
    h_layout_buttom->addWidget(record);

    v_layout->addLayout(h_layout_top);
    v_layout->addWidget(nodes_view);
    v_layout->addLayout(h_layout_buttom);

    window->setLayout(v_layout);
    window->setWindowTitle("Созадние образа");
}

int CreateImage::addFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"), "/home/user");
    qDebug() << fileName;

    if(fileName.isEmpty())
        return -1;

    QTreeWidgetItem *item = new QTreeWidgetItem();
    QTreeWidgetItem *curr_item = nodes_view->currentItem();
    if(curr_item == NULL)
    {
        fs::File *file = new fs::File(fileName.toStdString());

        if(m_root->addChild(file) < 0)
            return -1;

        item->setText(0, QString(file->getName().c_str()));
        item->setText(1, "FILE");
        item->setText(2, "4354");

        nodes_view->addTopLevelItem(item);
    }
    else
    {
        fs::File *file = new fs::File(fileName.toStdString());
        if(curr_item->parent() == NULL)
        {
            if(m_root->addChild(file) < 0)
                return -1;

            item->setText(0, QString(file->getName().c_str()));
            item->setText(1, "FILE");
            item->setText(2, "4354");

            nodes_view->addTopLevelItem(item);
        }
        else
        {

        }
    }



}

int CreateImage::addDir()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home/user",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())
        return -1;

    QTreeWidgetItem *curr_item = nodes_view->currentItem();
    QTreeWidgetItem *item = new QTreeWidgetItem();
    fs::Dir *folder = new fs::Dir(dir.toStdString());
    if(curr_item == NULL)
    {
        if(m_root->addChild(folder) < 0)
            return -1;

        item->setText(0, QString(folder->getName().c_str()));
        item->setText(1, "DIR");
        item->setText(2, "4354");

        nodes_view->addTopLevelItem(item);
    }
    else
    {

    }

}

int CreateImage::deleteNode()
{
    QTreeWidgetItem *item = nodes_view->currentItem();

    if(item != NULL)
    {
        item->~QTreeWidgetItem();
    }
}