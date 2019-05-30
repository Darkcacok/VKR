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

#include "../libburn/libburn.h"

class Burn
{
public:
    Burn();
    ~Burn();

    int driveScan();

    int getDrivesCount();

    /**
        @param n Index of m_drives starts from zero
        @return path succes, NULL error
    */
    std::string getDrivePath(int n);


    int writeIso(unsigned int d, std::string &iso_path, void (*progress)(float));

    /******** DEBUG INFO *********/

private:
    std::vector<std::string> m_drives;

    /*********Burn opts****************/
    int opc; //If non-zero, optical power calibration will be performed at start of burn
    int multi; //1=media will be appendable, 0=media will be closed (default)
    int simulate_burn; //Non-zero enables simulation, 0 enables real writing
    int read_speed; //Read speed in k/s (0 is max, -1 is min).
    int write_speed; //Write speed in k/s (0 is max, -1 is min).
    int underrun_proof; //if non-zero, buffer underrun protection is enabled

};

#endif // BURN_H
