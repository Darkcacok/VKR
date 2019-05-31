#ifndef SENDDATA_H
#define SENDDATA_H

#include <string>

#include "fs.h"

struct InfoForRecord
{
    std::string isoPath;
    std::string imgName;
    std::string discPath;

    fs::Dir *dir;
};

#endif // SENDDATA_H
