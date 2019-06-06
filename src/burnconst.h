#ifndef BURNCONST_H
#define BURNCONST_H

#include "libburn.h"
#include "sys/types.h"
#include "string"

struct disc_info
{
    std::string path_to_disc;
    int profile_no = -1;
    char profile_name[80];
    off_t available_size = 0;
    enum burn::burn_disc_status status;
};

#endif // BURNCONST_H
