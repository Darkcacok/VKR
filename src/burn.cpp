#include "burn.h"

Burn::Burn()
{
    if(!burn::burn_initialize())
        throw "error init";

    opc = 0;
    multi = 0;
    simulate_burn = 0;
    read_speed = 0;
    write_speed = 0;
    underrun_proof = 1;
}

Burn::~Burn()
{
    burn::burn_finish();
}

int Burn::driveScan()
{
    struct burn::burn_drive_info *drive_list;
    unsigned int drive_count = 0;

    while(!burn::burn_drive_scan(&drive_list, &drive_count))
    {
        usleep(100002);
    }

    for(int i = 0; i < drive_count; ++i)
    {
        char adr[BURN_DRIVE_ADR_LEN];

        if(burn::burn_drive_d_get_adr(drive_list[i].drive, adr) <= 0)
        {
            std::cout << "ERROR: " << "burn_drive_d_get_adr" << std::endl;
        }

        m_drives.push_back(adr);
    }

    burn::burn_drive_info_free(drive_list);

    return 1;
}


int Burn::getDrivesCount()
{
    return m_drives.size();
}

std::string Burn::getDrivePath(int n)
{
    if(n >= getDrivesCount())
        return NULL;

    return m_drives[n];
}

int Burn::writeIso(unsigned int d, std::string &iso_path, void (*progress)(float))
{
    struct burn::burn_drive_info *drive_info = NULL;
    int ret;

    /*Try grab known driver*/
    ret = burn::burn_drive_scan_and_grab(&drive_info, strdup(m_drives[d].c_str()), 1);
    if(ret < 0)
        return ret;

    int fd;
    int fd_size;
    struct stat stbuf;

    /*Is file exists?Is file a real file?*/
    fd = open(iso_path.c_str(), O_RDONLY);
    if(fd >= 0)
    {
        if(fstat(fd, &stbuf) != -1)
        {
            if((stbuf.st_mode & S_IFMT) == S_IFREG)
            {
                fd_size = stbuf.st_size;
            }
            else
            {
                std::cout << "ERROR: " << iso_path << "is not real file" << std::endl;
                return -1;
            }

        }
        else
        {
            /*Error fstat*/
            return -1;
        }
    }
    else
    {
        /*Error open*/
        return -1;
    }


    struct burn::burn_source *data_src = NULL, *fifo_src = NULL;
    struct burn::burn_disc *target_disc = NULL;
    struct burn::burn_session *session = NULL;
    struct burn::burn_write_opts *burn_options = NULL;
    struct burn::burn_track *track = NULL;
    struct burn::burn_progress b_progress;
    int fifo_chunksize = 2048;
    int fifo_chunks = 2048;
    int padding = 300 * 1024; //a padding of 300 kiB helps to avoid the read-ahead bug

    /*Convert this filedescriptor into a burn_source object*/
    data_src = burn::burn_fd_source_new(fd, -1, fd_size);
    if (data_src == NULL)
    {
        std::cout << "Could not open data source " << iso_path << std::endl;
        return -1;
    }

    /* Install a fifo object on top of that data source object */
    fifo_src = burn::burn_fifo_source_new(data_src, fifo_chunksize, fifo_chunks, 0);
    if (fifo_src == NULL)
    {
        std::cout << "Could not create fifo object of 4 MB\n"
                  << iso_path << std::endl;
        return -1;
    }

    /* Use the fifo object as data source for the track */
    if (burn::burn_track_set_source(track, fifo_src) != burn::BURN_SOURCE_OK)
    {
        std::cout << "Cannot attach source object to track object\n";
        return -1;
    }

    /*Add a track to a session at specified position*/
    if(burn::burn_session_add_track(session, track, BURN_POS_END) == 0)
    {
        std::cout << "Error" << std::endl;
        return -1;
    }

     burn_source_free(data_src);

     /*Check disc state*/
     enum burn::burn_disc_status disc_state = burn::burn_disc_get_status(drive_info->drive);
     if(disc_state != burn::BURN_DISC_BLANK && disc_state != burn::BURN_DISC_APPENDABLE)
     {
         return -1;
     }

     /*Create burn option*/
     if((burn_options = burn::burn_write_opts_new(drive_info->drive)) == NULL)
     {
         return -1;
     }
     burn::burn_write_opts_set_perform_opc(burn_options, opc);
     burn::burn_write_opts_set_multi(burn_options, multi);
     burn::burn_write_opts_set_simulate(burn_options, simulate_burn);
     burn::burn_write_opts_set_underrun_proof(burn_options, underrun_proof);
     char reasons[BURN_REASONS_LEN];
     if(burn::burn_write_opts_auto_write_type(burn_options, target_disc, reasons, 0) == burn::BURN_WRITE_NONE)
     {
         std::cout << "FATAL: Failed to find a suitable write mode with this media.\n"
                      << "Reasons given:" << std::endl
                      << reasons << std::endl;
                 return -1;
     }

     burn::burn_drive_set_speed(drive_info->drive, read_speed, write_speed);

     /*Finally write iso to disc*/
     burn::burn_disc_write(burn_options, target_disc);

     while(burn::burn_drive_get_status(drive_info->drive, NULL) == burn::BURN_DRIVE_SPAWNING)
         usleep(100002);

     while(burn::burn_drive_get_status(drive_info->drive, &b_progress) != burn::BURN_DRIVE_IDLE)
     {
         if(b_progress.sectors > 0 && b_progress.sector >= 0)
             progress(1.0 + 100.0 * b_progress.sector / b_progress.sectors);

         sleep(1);
     }


     /*Free all struct*/
     if(burn_options != NULL)
         burn::burn_write_opts_free(burn_options);
     if(fifo_src != NULL)
         burn::burn_source_free(fifo_src);
     if(track != NULL)
         burn::burn_track_free(track);
     if(target_disc != NULL)
         burn::burn_disc_free(target_disc);
     if(data_src != NULL)
         burn::burn_source_free(data_src);
     if(session != NULL)
         burn::burn_session_free(session);

     burn::burn_drive_info_forget(drive_info, 0);

     burn::burn_drive_release(drive_info->drive, 1);

     return 1;
}
