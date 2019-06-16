#ifndef DIR_H
#define DIR_H

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <dirent.h>

#include "constants.h"
#include "node.h"

namespace fs {

class Dir : public Node
{
public:
    Dir(const std::string &path);
    ~Dir();

    int addChild(Node *node);
    int getSize();
    int deleteChild(Node *node);
    int indexOf(Node *node);
    Node* getChild(unsigned int n);

private:
    std::list<Node*> children;


    bool findClone(Node *node);
};

}


#endif // DIR_H
