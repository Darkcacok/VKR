#ifndef ISOFS_H
#define ISOFS_H

#include <string>
#include <iostream>
#include <fstream>
#include <inttypes.h>


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
    int writeImage(const std::string &file_path, void (*percent)(float));


private:
    //Status of IsoFS class

    //
    IsoImage *m_image;
    IsoWriteOpts *m_opts;
};

#endif // ISOFS_H
