#ifndef BURN_H
#define BURN_H

#include <vector>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <functional>

#include "../libburn/libburn.h"
#include "burnconst.h"

class Burn
{
public:
    Burn();
    ~Burn();

    int driveScan();
    int driveScanAndGrab(std::string &device_addr);

    int getDrivesCount();
    struct disc_info *getDiscInfo(int n);

    /**
        @param n Index of m_drives starts from zero
        @return path succes, NULL error
    */
    std::string getDrivePath(int n);


    int writeIso(std::string &iso_path, std::function<void(float)> progress);

    /******** DEBUG INFO *********/

private:
    std::vector<struct disc_info> m_drives;
    struct burn::burn_drive *m_drive;

    /*********Burn opts****************/
    int opc; //If non-zero, optical power calibration will be performed at start of burn
    int multi; //1=media will be appendable, 0=media will be closed (default)
    int simulate_burn; //Non-zero enables simulation, 0 enables real writing
    int read_speed; //Read speed in k/s (0 is max, -1 is min).
    int write_speed; //Write speed in k/s (0 is max, -1 is min).
    int underrun_proof; //if non-zero, buffer underrun protection is enabled

};

#endif // BURN_H
