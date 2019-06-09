#ifndef CREATEIMAGE_H
#define CREATEIMAGE_H

#include <QWidget>
#include <QTreeView>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QItemSelectionModel>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QProgressDialog>
#include <string>
#include <map>

#include "fs.h"
#include "burn.h"
#include "senddata.h"
#include "recordform.h"
#include "isoitemtree.h"

class CreateImage : public QWidget
{
    Q_OBJECT

public:
    explicit CreateImage(QWidget *parent = nullptr);
    ~CreateImage();

private:

    fs::Dir *m_root;
    IsoItemTree *isoItemTree;
    Burn *burn;
    bool checLabel, checkTree;


    /******Layout************/
    QVBoxLayout *v_layout;
    QHBoxLayout *h_layout_buttom;
    QHBoxLayout *h_layout_top;

    /******Buttons*****************/
    QPushButton *add_file;
    QPushButton *add_dir;
    QPushButton *delete_node;
    QPushButton *record;

    /*******Line Edit*************/
    QLineEdit *imgNameEdit;

    /*****Combobox****************/
    QComboBox *choose_disc;


    /*****TreeWidget*************/
    QTreeView *nodes_view;
    //QTreeWidgetItem *top_level_item;
    static const int column = 3;

    /***/
    //std::map<QTreeWidgetItem*, fs::Node*> modelView;


    /********Support Forms***********/
    RecordForm *recordForm;

    /*************Functions******************/
    int createWindow();
    //QTreeWidgetItem *addItem(fs::Node *node);
    //QTreeWidgetItem *addFile(fs::Dir *dir);

    void driveScan();

    void checkRecord();


public slots:
    int addFile();
    int addDir();
    int deleteNode();
    void recordIsoImage();
    void change(QPoint &event);
    void labelEdited(const QString &text);

    void recieveData(InfoForRecord ifr);

signals:
    void sendData(InfoForRecord ifr);
    void setValue(int);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // CREATEIMAGE_H
