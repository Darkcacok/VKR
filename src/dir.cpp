#include "dir.h"

fs::Dir::Dir(const std::string &path) : Node(path)
{
    if(!path.compare("root"))
        return;

    if(this->getType() != NodeType::ISO_DIR)
        throw "error";

    struct dirent *entry;
    DIR *dir;

    dir = opendir(path.c_str());
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            Node *node;
            if(entry->d_type == DT_DIR)
                node = new Dir(path + "/" + std::string(entry->d_name));
            else
                node = new Node(path + "/" + std::string(entry->d_name));

            this->addChild(node);
        }
    }
    closedir(dir);

}

fs::Dir::~Dir()
{
    for(int i = 0; i < getSize(); ++i)
        children[i]->~Node();
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

int fs::Dir::indexOf(fs::Node *node)
{
    for(int i = 0; i < getSize();++i)
    {
        if(children[i] == node)
            return i;
    }

    return -1;
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
