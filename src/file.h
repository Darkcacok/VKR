#ifndef FILE_H
#define FILE_H

#include <string>
#include <sys/stat.h>

#include "node.h"
#include "constants.h"

namespace fs {

class File : public  Node
{
public:
    File(const std::string &path);
    ~File();

public:
    int getSize();
};

}


#endif // FILE_H
