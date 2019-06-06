#include "createimage.h"

CreateImage::CreateImage(QWidget *parent) :
    QWidget(parent)
{
    createWindow();

    recordForm = new RecordForm();
    recordForm->setModal(true);

    connect(this, SIGNAL(sendData(InfoForRecord)), recordForm, SLOT(recieveData(InfoForRecord)));
    connect(recordForm, SIGNAL(sendData(InfoForRecord)), this, SLOT(recieveData(InfoForRecord)));

    m_root = new fs::Dir("root");
    burn = new Burn();
    checLabel = false;
    checkTree = false;

    QtConcurrent::run(this, &CreateImage::driveScan);

}

void CreateImage::driveScan()
{
    burn->driveScan();

    for(int i = 0; i < burn->getDrivesCount(); ++i)
    {
        struct disc_info *di;
        char scales[] = "bkmgtp";
        char scale_c = scales[0];
        float size;
        std::string str;

        di = burn->getDiscInfo(i);

        size = di->available_size;

        for(int i = 1; scales[i]!= '\0'; ++i)
        {
            if(size <= 1024)
                break;
            size /= 1024;
            scale_c = scales[i];
        }

        str = di->profile_name;
        char buf[4];
        sprintf(buf, "%.2f", size);
        str += ": " + std::string(buf);

        switch (scale_c)
        {
        case 'b':
            str += "byte";
            break;
        case 'k':
            str += "KiB";
            break;
        case 'm':
            str += "MiB";
            break;
        case 'g':
            str += "GiB";
            break;
        case 't':
            str += "TiB";
            break;
        case 'p':
            str += "PiB";
            break;
        }
    }
}

void CreateImage::checkRecord()
{
    if(checLabel && checkTree)
        record->setEnabled(true);
    else
        record->setEnabled(false);
}

CreateImage::~CreateImage()
{
}

int CreateImage::createWindow()
{

    v_layout = new QVBoxLayout();
    h_layout_top = new QHBoxLayout();
    h_layout_buttom = new QHBoxLayout();

    add_file = new QPushButton("Добавить Файл");
    add_dir = new QPushButton("Добавить Папку");
    delete_node = new QPushButton("Удалить");
    record = new QPushButton("Записать");
    record->setEnabled(false);


    add_file->setToolTip("Добавить Файл");
    add_dir->setToolTip("Добавить Файл");
    delete_node->setToolTip("Удалить Файл или Папку");
    record->setToolTip("Записать образ на диск");

    /********************Set Icons******************************/
    add_file->setIcon(QIcon(":/icons/document-new.svg"));
    add_dir->setIcon(QIcon(":/icons/folder-add.svg"));
    delete_node->setIcon(QIcon(":/icons/edit-delete.svg"));
    record->setIcon(QIcon(":/icons/application-x-cda.svg"));

    add_file->setIconSize(QSize(20,20));
    add_dir->setIconSize(QSize(20,20));
    delete_node->setIconSize(QSize(20,20));

    connect(add_file, SIGNAL(clicked()), this, SLOT(addFile()));
    connect(add_dir, SIGNAL(clicked()), this, SLOT(addDir()));
    connect(delete_node, SIGNAL(clicked()), this, SLOT(deleteNode()));
    connect(record, SIGNAL(clicked()), this, SLOT(recordIsoImage()));

    imgNameEdit = new QLineEdit();
    imgNameEdit->setPlaceholderText("Название Образа");
    connect(imgNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(labelEdited(const QString&)));

    choose_disc = new QComboBox();
    choose_disc->addItem(QString("Файл"));

    /************TreeWidget****************************/
    nodes_view = new QTreeWidget();
    top_level_item = new QTreeWidgetItem();

    nodes_view->setColumnCount(column);
    nodes_view->setHeaderLabels(QStringList{"Название", "Тип", "Размер"});
    //nodes_view->viewport()->installEventFilter(this);
    //connect(nodes_view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(change(const QPoint &)));
    //connect(nodes_view, SIGNAL(itemSelectionChanged()), this, SLOT(change()));


    h_layout_top->addWidget(add_file);
    h_layout_top->addWidget(add_dir);
    h_layout_top->addWidget(delete_node);
    h_layout_top->addItem(new QSpacerItem(40, 20,  QSizePolicy::Expanding, QSizePolicy::Minimum));

    h_layout_buttom->addWidget(choose_disc, 1);
    h_layout_buttom->addWidget(record);

    v_layout->addLayout(h_layout_top);
    v_layout->addWidget(nodes_view);
    v_layout->addWidget(imgNameEdit);
    v_layout->addLayout(h_layout_buttom);


    this->setWindowTitle("Созадние образа");
    this->setLayout(v_layout);
}

QTreeWidgetItem *CreateImage::addItem(fs::Node *node)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();

    item->setText(0, QString(node->getName().c_str()));
    switch (node->getType()) {
    case fs::NodeType::ISO_FILE:
        item->setIcon(0, QIcon(":/icons/text-x-generic.svg"));
        item->setText(1, "Файл");
        item->setText(2, QString::number(node->getSize()) + QString(" байт"));
        break;
    case fs::NodeType::ISO_DIR:
        item->setIcon(0, QIcon(":/icons/folder.svg"));
        item->setText(1, "Папка");
        item->setText(2, QString::number(node->getSize()) + QString(" элементов"));
        break;
    case fs::NodeType::ISO_SYMLINK:
        item->setIcon(0, QIcon(":/icons/text-x-generic.svg"));
        item->setText(1, "Ссылка");
        item->setText(2, QString::number(node->getSize()) + QString(" байт"));
        break;
    }


    return item;
}

QTreeWidgetItem *CreateImage::addFile(fs::Dir *dir)
{
    QTreeWidgetItem *item = addItem(dir);
    modelView.insert(std::pair<QTreeWidgetItem*,fs::Node*>(item, dir));

    for(int i = 0; i < dir->getSize(); ++i)
    {
        fs::Node *node = dir->getChild(i);
        if(node->getType() == fs::ISO_DIR)
        {
            item->addChild(addFile((fs::Dir*)node));
        }
        else
        {
            QTreeWidgetItem *childItem = new QTreeWidgetItem();
            childItem = addItem(node);
            item->addChild(childItem);
            modelView.insert(std::pair<QTreeWidgetItem*,fs::Node*>(childItem, node));
        }
    }

    return item;
}

int CreateImage::addFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выбрать файл"), "/home/user");
    qDebug() << fileName;

    if(fileName.isEmpty())
        return -1;

    QTreeWidgetItem *curr_item = nodes_view->currentItem();
    if(curr_item == NULL)
    {
        fs::File *file = new fs::File(fileName.toStdString());

        if(m_root->addChild(file) < 0)
        {
            QMessageBox::information(this, "Внимание", "В этой папке уже есть файл с таким именем: " + QString(file->getName().c_str()));
            return -1;
        }

        QTreeWidgetItem *item = addItem(file);
        modelView.insert(std::pair<QTreeWidgetItem*,fs::Node*>(item, file));
        nodes_view->addTopLevelItem(item);
    }
    else
    {
        fs::File *file = new fs::File(fileName.toStdString());
        fs::Node *node = modelView[curr_item];

        if(node->getType() != fs::ISO_DIR)
        {
            node = node->getParent();
            curr_item = curr_item->parent();
        }

        QTreeWidgetItem *item = addItem(file);
        modelView.insert(std::pair<QTreeWidgetItem*,fs::Node*>(item, file));

        if(((fs::Dir*)node)->addChild(file) < 0)
        {
            QMessageBox::information(this, "Внимание", "В этой папке уже есть файл с таким именем: " + QString(file->getName().c_str()));
            return -1;
        }

        if(curr_item == NULL)
            nodes_view->addTopLevelItem(item);
        else
            curr_item->addChild(item);
    }

    if(!checkTree)
    {
        checkTree = true;
        checkRecord();
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
    fs::Dir *folder = new fs::Dir(dir.toStdString());
    if(curr_item == NULL)
    {
        if(m_root->addChild(folder) < 0)
        {
            QMessageBox::information(this, "Внимание", "В этой папке уже есть папка с таким именем: " + QString(folder->getName().c_str()));
            return -1;
        }

        nodes_view->addTopLevelItem(addFile(folder));

    }
    else
    {
        fs::Node *node = modelView[curr_item];

        if(node->getType() != fs::ISO_DIR)
        {
            node = node->getParent();
            curr_item = curr_item->parent();
        }

        if(((fs::Dir*)node)->addChild(folder) < 0)
        {
            QMessageBox::information(this, "Внимание", "В этой папке уже есть папка с таким именем: " + QString(folder->getName().c_str()));
            return -1;
        }

        if(curr_item == NULL)
            nodes_view->addTopLevelItem(addFile(folder));
        else
            curr_item->addChild(addFile(folder));
    }

    if(!checkTree)
    {
        checkTree = true;
        checkRecord();
    }

}

int CreateImage::deleteNode()
{
    QTreeWidgetItem *item = nodes_view->currentItem();

    if(item != NULL)
    {
        item->~QTreeWidgetItem();
    }

    if(m_root->getSize() == 0)
    {
        checLabel == false;

        checkRecord();
    }
}

void CreateImage::recordIsoImage()
{      
    InfoForRecord ifr;
    int  choose = choose_disc->currentIndex();

    if(choose == 0)
        ifr.discPath = "";
    else
    {
        ifr.discPath = burn->getDrivePath(choose);
        struct disc_info *di = burn->getDiscInfo(choose - 1);

        if(di->status == burn::BURN_DISC_FULL)
        {
            QMessageBox msgBox;
            msgBox.setText("Диск полный");
            msgBox.setInformativeText("Хотите отчистить диск?");
            msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
            int ret = msgBox.exec();


            if(ret == QMessageBox::Yes)
            {
                if(burn->driveScanAndGrab(di->path_to_disc) < 0)
                    QMessageBox::information(this, "Ошибка", "Не удалсоь захваить диск");

                QProgressDialog progress(this);
                connect(this, SIGNAL(setValue(int)), &progress, SLOT(setValue(int)));
                progress.setMinimum(0);
                progress.setMaximum(100);
                progress.setLabelText("Отчистка Диска");
                progress.setModal(true);
                progress.show();

                burn->blankDisc(1, [this](float p){
                    emit setValue(p);
                });

                return;
            }
            else {
                return;
            }

        }
    }

    QString Qpath = QFileDialog::getSaveFileName(this, tr("Сохранить образ диска"), "", tr("All Files (*)"));
    if(Qpath.isEmpty())
    {
        return;
    }

    ifr.isoPath = Qpath.toStdString();

    ifr.imgName = imgNameEdit->text().toStdString();
    ifr.dir = m_root;

    recordForm->show();

    emit sendData(ifr);
}

void CreateImage::change(QPoint &event)
{
    int x;
    x = 2;
}

void CreateImage::labelEdited(const QString &text)
{
    if(text.isEmpty())
    {
        checLabel = false;
    }
    else
    {
        checLabel = true;
    }

    checkRecord();
}

void CreateImage::recieveData(InfoForRecord ifr)
{

}

bool CreateImage::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == nodes_view->viewport() && event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);

        QTreeWidgetItem *item = nodes_view->itemAt(me->pos());

        if(item == NULL)
        {
            //nodes_view->currentItem()->setSelected(false);
            //nodes_view->clearFocus();
            nodes_view->setCurrentIndex(QModelIndex());
            //nodes_view->selectionModel()->clearSelection();
        }
    }
}
