#include "dir.h"

fs::Dir::Dir(const std::string &path) : Node(path)
{
    //if(this->getType() != NodeType::ISO_DIR)
        //throw "error";
}

int fs::Dir::addChild(fs::Node *node)
{
    if(this->findClone(node))
        return -1;

    this->children.push_back(node);
    this->setParent(node);

    return 1;
}

int fs::Dir::getSize()
{
    return children.size();
}

fs::Node *fs::Dir::getChild(unsigned int n)
{
    if(n >= this->getSize())
        return NULL;

    return children[n];
}

bool fs::Dir::findClone(fs::Node *node)
{
    for(int i = 0; i < children.size(); ++i)
    {
        if(children[i]->getType() == node->getType() && !children[i]->getPath().compare(node->getPath()))
            return true;
    }

    return false;
}
