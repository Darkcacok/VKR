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
    /*for(int i = 0; i < getSize(); ++i)
        children[i]->~Node();*/

    std::list<Node*>::iterator it;
    for(it = children.begin(); it != children.end(); ++it)
    {
        fs::Node *node = *it;
        if((*it)->getType() == NodeType::ISO_DIR)
            delete static_cast<fs::Dir*>(node);
        else
            delete node;
    }
    children.clear();
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
    std::list<Node*>::iterator iterator;
    int k = 0;

    for(iterator = children.begin(); iterator != children.end(); ++iterator)
    {
        if(*iterator == node)
            return k;
        ++k;
    }

    return -1;

    /*for(auto const&i : children)
    {

    }
    for(int i = 0; i < getSize();++i)
    {
        if(children[i] == node)
            return i;
    }*/

    return -1;
}

fs::Node *fs::Dir::getChild(unsigned int n)
{
    if(n >= this->getSize())
        return NULL;

    std::list<Node*>::iterator iterator = children.begin();
    std::advance(iterator, n);

    return *iterator;
}

int fs::Dir::deleteChild(fs::Node *node)
{
    std::list<Node*>::iterator it = std::find(children.begin(), children.end(), node);
    children.remove(node);

    if((*it)->getType() == NodeType::ISO_DIR)
       delete static_cast<fs::Dir*>(*it);
    else
        delete (*it);
    return 1;
}

bool fs::Dir::findClone(fs::Node *node)
{

    std::list<Node*>::iterator iterator;

    for(iterator = children.begin(); iterator != children.end(); ++iterator)
    {
        if((*iterator)->getType() == node->getType() && !(*iterator)->getName().compare(node->getName()))
            return true;
    }


    /*for(int i = 0; i < children.size(); ++i)
    {
        if(children[i]->getType() == node->getType() && !children[i]->getName().compare(node->getName()))
            return true;
    }*/

    return false;
}
