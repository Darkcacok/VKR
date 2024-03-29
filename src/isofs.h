#ifndef ISOFS_H
#define ISOFS_H

#include <string>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <functional>


#define LIBISOFS_WITHOUT_LIBBURN yes
#include "../lib/libisofs/libisofs.h"

#include "fs.h"

class IsoFS
{
public:
    IsoFS();
    ~IsoFS();

    /***********Set the write options***************/

    //Add Extensions
    int setExtnsnRockRidge();
    int setExtnsnJoliet();
    int setExtnsnIso1999();

    //Add ISO-9660 options

    /***********Image tree manipulation************/
    int CreateImage(fs::Dir *dir, const std::string name);


    /***********Write image*************************/
    int writeImage(const std::string &file_path, std::function<void(int)> percent);


    /*Error*/
    std::string getLastError();

private:
    int addDir(fs::Dir *dir, IsoDir *isoDir);

private:
    //Status of IsoFS class

    //
    IsoImage *m_image;
    IsoWriteOpts *m_opts;

    /*Error**/
    std::string strError;
};

#endif // ISOFS_H
