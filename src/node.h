#ifndef NODE_H
#define NODE_H

#include <string>
#include <sys/stat.h>
#include <vector>
#include <dirent.h>
#include <string.h>

#include "constants.h"


namespace fs {

class Dir;

class Node
{
public:
    Node();
    virtual ~Node();
    Node(const std::string &path);

    Dir *getParent();
    NodeType getType();
    std::string getPath();
    std::string getName();
    virtual int getSize();

protected:
    void setParent(Node *node);
    int size;

private:
    std::string path;
    std::string name;
    NodeType type;
    Dir *parent;
};
}
#endif // NODE_H
