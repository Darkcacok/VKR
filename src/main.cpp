#include "mainwindow.h"
#include "createimage.h"
#include <QApplication>
#include <QTextCodec>
#include "../lib/libisofs/libisofs.h"
#include "file.h"
#include "dir.h"
#include <isofs.h>
#include <string>
#include "QDebug"
#include "recordform.h"

#include "fs.h"

void progress(float p)
{
    std::cout << p << std::endl;
}


int main(int argc, char *argv[])
{
    /*QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);*/

    QApplication a(argc, argv);

    /*if(iso_init()< 0)
        return -1;

    IsoImage *image;
    IsoDir *idir;

    iso_image_new("NEW", &image);

    iso_tree_add_dir_rec(image, iso_image_get_root(image), "/home/comp/brasero/Shared/Новая папка/Новая папка/Vb/");

    idir = iso_image_get_root(image);*/


    /*fs::File *file = new  fs::File(std::string("/home/user/army2018.tar.gz"));
    std::string name = file->getName();
    fs::Dir *dir = new fs::Dir(std::string("/home/user/Diplom"));
    fs::Dir *dir2 = new fs::Dir(std::string("/home/user/Diplom"));

    dir->addChild(file);
    dir->addChild(dir2);
    dir->getParent();

    IsoFS iso;
    iso.setExtnsnRockRidge();
    iso.CreateImage(dir, std::string("test"));
    iso.writeImage(std::string("/home/user/test.iso"), progress);*/


    CreateImage w;
    w.show();

    return a.exec();
}
