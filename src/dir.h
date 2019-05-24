#ifndef DIR_H
#define DIR_H

#include <vector>
#include <string>
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
    Node* getChild(unsigned int n);

private:
    std::vector<Node*> children;


    /*functions*/
    bool findClone(Node *node);
};

}


#endif // DIR_H
