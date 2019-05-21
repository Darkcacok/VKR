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

    if(S_ISREG(st.st_mode))
    {
        this->type = NodeType::ISO_FILE;
        return;
    }

    if(S_ISDIR(st.st_mode))
    {
        this->type = NodeType::ISO_DIR;
        return;
    }

    if(S_ISLNK(st.st_mode))
    {
        this->type = NodeType::ISO_SYMLINK;
        return;
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
