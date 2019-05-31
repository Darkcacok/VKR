#include "isofs.h"

IsoFS::IsoFS()
{
    if(iso_init()<0)
    {
        throw "cant init lib isofs";
    }

    if(iso_write_opts_new(&m_opts, 0) < 0)
    {
        throw "cant create options for record";
    }
}

IsoFS::~IsoFS()
{
    iso_finish();
}

int IsoFS::setExtnsnRockRidge()
{
    return iso_write_opts_set_rockridge(m_opts, 1);
}

int IsoFS::setExtnsnJoliet()
{
    return iso_write_opts_set_joliet(m_opts, 1);
}

int IsoFS::setExtnsnIso1999()
{
    return iso_write_opts_set_iso1999(m_opts, 1);
}

int IsoFS::CreateImage(fs::Dir *dir, const std::string name)
{
    if(dir->getParent() !=  NULL)
        return -1;

    int ret;

    ret = iso_image_new(name.c_str(), &m_image);
    if(ret < 0)
        return ret;

   /* for(int i = 0; i < dir->getSize(); ++i)
    {
        fs::Node *node = dir->getChild(i);

        switch(node->getType())
        {
        case fs::NodeType::ISO_FILE:
            iso_tree_add_node(m_image, iso_image_get_root(m_image), node->getPath().c_str(), NULL);
            break;
        case fs::NodeType::ISO_DIR:
            IsoDir *dir;
            //iso_tree_add_new_dir(iso_image_get_root(m_image), node->getName().c_str(), &dir);
            iso_image_add_new_dir(m_image, iso_image_get_root(m_image), node->getName().c_str(), &dir);
            //iso_tree_add_dir_rec(m_image, dir, node->getPath().c_str());
            break;
        }
    }*/

    addDir(dir, iso_image_get_root(m_image));

    return 1;
}

int IsoFS::addDir(fs::Dir *dir, IsoDir *isoDir)
{
    for(int i = 0; i < dir->getSize(); ++i)
    {
        fs::Node *node = dir->getChild(i);

        if(!node->getName().compare("WorldClock"))
        {
            int x;
            x = 5;
        }

        if(node->getType() == fs::ISO_DIR)
        {
            IsoDir *newDir;
            iso_image_add_new_dir(m_image, isoDir, node->getName().c_str(), &newDir);
            addDir((fs::Dir*)node, newDir);
        }
        else
        {
            iso_tree_add_node(m_image, isoDir, node->getPath().c_str(), NULL);
        }
    }

    return 0;
}

int IsoFS::writeImage(const std::string &file_path, std::function<void(int)> percent)
{
    struct burn_source *burn_src;
    int block_size = 2048;
    unsigned char buf[block_size];
    off_t progress = 0;

    if(iso_image_create_burn_source(m_image, m_opts, &burn_src) < 0)
    {
        return -1;
    }

    std::ofstream iso_file;
    iso_file.open(file_path, std::ios_base::binary);

    off_t iso_size = burn_src->get_size(burn_src);

    if(iso_size == 0)
    {
        return -1;
    }

    while(burn_src->read_xt(burn_src, buf, block_size) == block_size)
    {
        iso_file.write(reinterpret_cast<char*>(buf), block_size);

        progress += block_size;
        percent((progress * 100.0)/iso_size);
    }

    iso_file.close();

    return 1;
}
