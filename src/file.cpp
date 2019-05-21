#include "file.h"

fs::File::File(const std::string &path) : Node(path)
{
    //if(this->getType() != NodeType::ISO_FILE)
        //throw "error";
}

fs::File::~File()
{

}
