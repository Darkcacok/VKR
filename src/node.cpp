#include "node.h"

fs::Node::Node()
{

}

fs::Node::~Node()
{

}

fs::Node::Node(const std::string &path)
{
    if(!path.compare("root"))
    {
        this->path = "root";
        this->name = "root";
        this->parent = NULL;
        this->type = fs::ISO_DIR;
        this->size = 0;

        return;
    }

    if(path.empty())
        throw "empty path";

    this->path = path;
    this->parent = NULL;


    int edge;
    for(edge = path.length() - 1; edge >=0 && path.at(edge) != '/'; --edge);
    this->name = path.substr(edge + 1, path.length() - 1);

    struct stat st;

    if(lstat(path.c_str(), &st) < 0)
    {
        throw "error stat()";
    }

    switch(st.st_mode & S_IFMT)
    {
    case S_IFREG:
        this->type = NodeType::ISO_FILE;
        this->size = st.st_size;
        break;
    case S_IFDIR:
        this->type = NodeType::ISO_DIR;

        struct dirent *entry;
        DIR *dir;

        dir = opendir(path.c_str());

        this->size = 0;
        while((entry = readdir(dir)) != NULL)
        {
            ++this->size;
        }

        this->size -= 2;

        closedir(dir);

        break;
    case S_IFLNK:
        this->type = NodeType::ISO_SYMLINK;
        this->size = st.st_size;
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

int fs::Node::getSize()
{
    return size;
}


void fs::Node::setParent(fs::Node *node)
{
    node->parent = (Dir*)this;
}
