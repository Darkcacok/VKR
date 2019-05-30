#ifndef CREATEIMAGE_H
#define CREATEIMAGE_H

#include <QWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QItemSelectionModel>
#include <string>
#include <map>

#include "fs.h"

class CreateImage : public QWidget
{
    Q_OBJECT

public:
    explicit CreateImage(QWidget *parent = 0);
    ~CreateImage();

private:

    fs::Dir *m_root;


    /******Layout************/
    QVBoxLayout *v_layout;
    QHBoxLayout *h_layout_buttom;
    QHBoxLayout *h_layout_top;

    /******Buttons*****************/
    QPushButton *add_file;
    QPushButton *add_dir;
    QPushButton *delete_node;
    QPushButton *record;

    /*****Combobox****************/
    QComboBox *choose_disc;


    /*****TreeWidget*************/
    QTreeWidget *nodes_view;
    QTreeWidgetItem *top_level_item;
    static const int column = 3;

    /***/
    std::map<QTreeWidgetItem*, fs::Node*> modelView;


    /*************Functions******************/
    int createWindow();
    QTreeWidgetItem *addItem(fs::Node *node);
    QTreeWidgetItem *addFile(fs::Dir *dir);


private slots:
    int addFile();
    int addDir();
    int deleteNode();
    void change();

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // CREATEIMAGE_H
