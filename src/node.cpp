#include "node.h"

fs::Node::Node()
{

}

fs::Node::~Node()
{

}

fs::Node::Node(const std::string &path)
{
    if(path.empty())
        throw "empty path";

    this->path = path;
    this->parent = NULL;


    int edge;
    for(edge = path.length() - 1; edge >=0 && path.at(edge) != '/'; --edge);
    this->name = path.substr(edge + 1, path.length() - 1);


    struct stat st;

    if(stat(path.c_str(), &st) < 0)
    {
        throw "error stat()";
    }

    switch(st.st_mode & S_IFMT)
    {
    case S_IFREG:
        this->type = NodeType::ISO_FILE;
        break;
    case S_IFDIR:
        this->type = NodeType::ISO_DIR;
        break;
    case S_IFLNK:
        this->type = NodeType::ISO_SYMLINK;
        break;
    }
}

fs::Dir *fs::Node::getParent()
{
    return this->parent;
}

fs::NodeType fs::Node::getType()
{
    return type;
}

std::string fs::Node::getPath()
{
    return this->path;
}

std::string fs::Node::getName()
{
    return this->name;
}


void fs::Node::setParent(fs::Node *node)
{
    node->parent = (Dir*)this;
}
