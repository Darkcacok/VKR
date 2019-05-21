#ifndef NODE_H
#define NODE_H

#include <string>
#include <sys/stat.h>
#include <vector>

#include "constants.h"


namespace fs {

class Dir;

class Node
{
public:
    Node();
    ~Node();
    Node(const std::string &path);

    Dir *getParent();
    NodeType getType();
    std::string getPath();
    std::string getName();

protected:
    void setParent(Node *node);

private:
    std::string path;
    std::string name;
    NodeType type;
    Dir *parent;

    /*functions*/

};
}
#endif // NODE_H
