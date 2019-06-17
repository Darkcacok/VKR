#include "burn.h"

Burn::Burn()
{
    if(!burn::burn_initialize())
        throw "error init";

    m_drive = NULL;
    m_drive_list = NULL;
    opc = 0;
    multi = 0;
    simulate_burn = 0;
    read_speed = 0;
    write_speed = 0;
    underrun_proof = 1;
}

Burn::~Burn()
{
    if(m_drive != NULL)
        burn::burn_drive_release(m_drive, 0);
    if(m_drive_list != NULL)
        burn::burn_drive_info_free(m_drive_list);
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
        struct burn::burn_multi_caps *caps = NULL;
        struct burn::burn_write_opts *o = NULL;
        struct disc_info di;

        if(burn::burn_drive_d_get_adr(drive_list[i].drive, adr) <= 0)
        {
            std::cout << "ERROR: " << "burn_drive_d_get_adr" << std::endl;
            strError = "ERROR: burn_drive_d_get_adr";
            return -1;
        }

        di.path_to_disc = adr;

        if(!burn::burn_drive_grab(drive_list[i].drive, 1))
        {
            std::cout << "ERROR: " << "burn_drive_grab" << std::endl;
            strError = "ERROR: burn_drive_grab";
            return -1;
        }
        else
        {
            di.status = burn::burn_disc_get_status(drive_list[i].drive);

            if(!burn::burn_disc_get_profile(drive_list[i].drive, &di.profile_no, di.profile_name))
            {
                std::cout << "ERROR: " << "burn_drive_get_profile" << std::endl;
                strError = "ERROR: burn_drive_get_profile";
                return -1;
            }

            if(burn::burn_disc_get_multi_caps(drive_list[i].drive, burn::BURN_WRITE_NONE, &caps, 0) > 0)
            {
                o = burn::burn_write_opts_new(drive_list[i].drive);
                if(o != NULL)
                {
                    burn::burn_write_opts_set_perform_opc(o, 0);

                    if(caps->advised_write_mode == burn::BURN_WRITE_TAO)
                        burn::burn_write_opts_set_write_type(o, burn::BURN_WRITE_TAO, burn::BURN_BLOCK_MODE1);
                    else if(caps->advised_write_mode == burn::BURN_WRITE_SAO)
                        burn::burn_write_opts_set_write_type(o, burn::BURN_WRITE_SAO, burn::BURN_BLOCK_SAO);
                    else{
                        burn::burn_write_opts_free(o);
                        o = NULL;
                    }
                }

                di.available_size = burn::burn_disc_available_space(drive_list[i].drive, o);

                burn::burn_disc_free_multi_caps(&caps);
                if(o != NULL)
                    burn::burn_write_opts_free(o);
            }
        }

        burn::burn_drive_release(drive_list[i].drive, 0);

        m_drives.push_back(di);
    }

    burn::burn_drive_info_free(drive_list);

    return 1;
}

int Burn::driveScanAndGrab(std::string &device_addr)
{
    if(m_drive != NULL)
        burn::burn_drive_release(m_drive, 0);

    int ret;

    ret = burn::burn_drive_scan_and_grab(&m_drive_list, strdup(device_addr.c_str()), 1);

    if(ret <= 0)
    {
        strError = "Не удалось захватить устройство по адрессу: " + device_addr;
        return -1;
    }


    char adr[BURN_DRIVE_ADR_LEN];
    struct burn::burn_multi_caps *caps = NULL;
    struct burn::burn_write_opts *o = NULL;
    struct disc_info di;

    if(burn::burn_drive_d_get_adr(m_drive_list[0].drive, adr) <= 0)
    {
        std::cout << "ERROR: " << "burn_drive_d_get_adr" << std::endl;
        strError = "ERROR: burn_drive_d_get_adr";
        return -1;
    }


    di.status = burn::burn_disc_get_status(m_drive_list[0].drive);

    if(!burn::burn_disc_get_profile(m_drive_list[0].drive, &di.profile_no, di.profile_name))
    {
        std::cout << "ERROR: " << "burn_drive_get_profile" << std::endl;
        strError = "ERROR: burn_drive_get_profile";
        return -1;
    }

    if(burn::burn_disc_get_multi_caps(m_drive_list[0].drive, burn::BURN_WRITE_NONE, &caps, 0) > 0)
    {
        o = burn::burn_write_opts_new(m_drive_list[0].drive);
        if(o != NULL)
        {
            burn::burn_write_opts_set_perform_opc(o, 0);

            if(caps->advised_write_mode == burn::BURN_WRITE_TAO)
                burn::burn_write_opts_set_write_type(o, burn::BURN_WRITE_TAO, burn::BURN_BLOCK_MODE1);
            else if(caps->advised_write_mode == burn::BURN_WRITE_SAO)
                burn::burn_write_opts_set_write_type(o, burn::BURN_WRITE_SAO, burn::BURN_BLOCK_SAO);
            else{
                burn::burn_write_opts_free(o);
                o = NULL;
            }
        }

        di.available_size = burn::burn_disc_available_space(m_drive_list[0].drive, 0);

        burn::burn_disc_free_multi_caps(&caps);
        if(o != NULL)
            burn::burn_write_opts_free(o);
    }

    m_drive = m_drive_list[0].drive;

    return 1;
}


int Burn::getDrivesCount()
{
    return m_drives.size();
}

disc_info *Burn::getDiscInfo(int n)
{
    if(n >= getDrivesCount())
        return NULL;

    return &m_drives[n];
}

std::string Burn::getDrivePath(int n)
{
    if(n >= getDrivesCount())
        return NULL;

    return m_drives[n].path_to_disc;
}

int Burn::writeIso(std::string &iso_path, std::function<void(float)> progress)
{

    /* Print messages of severity SORRY or more directly to stderr */
    burn::burn_msgs_set_severities("NEVER", "SORRY", "libburner : ");
    burn::burn_set_signal_handling((void*)"libburner : ", NULL, 0x30);

    int ret;

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
                strError = iso_path + " не является файлом";
                return -1;
            }

        }
        else
        {
            strError = "ERROR: fstat";
            return -1;
        }
    }
    else
    {
        strError = "Не удалсоь открыть файл по адрессу: " + iso_path;
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


    target_disc = burn::burn_disc_create();
    session = burn::burn_session_create();
    burn::burn_disc_add_session(target_disc, session, BURN_POS_END);

    track = burn::burn_track_create();
    burn::burn_track_define_data(track, 0, padding, 1, BURN_MODE1);


    /*Convert this filedescriptor into a burn_source object*/
    data_src = burn::burn_fd_source_new(fd, -1, fd_size);
    if (data_src == NULL)
    {
        std::cout << "Could not open data source " << iso_path << std::endl;
        strError = "Не удалсоь открыть файл по адрессу: " + iso_path;
        return -1;
    }

    /* Install a fifo object on top of that data source object */
    fifo_src = burn::burn_fifo_source_new(data_src, fifo_chunksize, fifo_chunks, 0);
    if (fifo_src == NULL)
    {
        std::cout << "Could not create fifo object of 4 MB\n"
                  << iso_path << std::endl;
        strError = "Не удалсоь открыть создать объект fifo в 4 Мбайт: " + iso_path;
        return -1;
    }


    /* Use the fifo object as data source for the track */
    if (burn::burn_track_set_source(track, fifo_src) != burn::BURN_SOURCE_OK)
    {
        std::cout << "Cannot attach source object to track object\n";
        strError = "Не удалсоь прикрепить данные к трэку";
        return -1;
    }



    /*Add a track to a session at specified position*/
    if(burn::burn_session_add_track(session, track, BURN_POS_END) == 0)
    {
        std::cout << "Error" << std::endl;
        strError = "Не удалсоь добавить трэк к сессии";
        return -1;
    }

    burn::burn_source_free(data_src);
    data_src = NULL;

    /*Check disc state*/
    enum burn::burn_disc_status disc_state = burn::burn_disc_get_status(m_drive);
    if(disc_state != burn::BURN_DISC_BLANK && disc_state != burn::BURN_DISC_APPENDABLE)
    {
        strError = "Диск не пустой";
        return -1;
    }

    /*Create burn option*/
    if((burn_options = burn::burn_write_opts_new(m_drive)) == NULL)
    {
        return -1;
    }
    burn::burn_write_opts_set_perform_opc(burn_options, opc);
    burn::burn_write_opts_set_multi(burn_options, multi);
    burn::burn_write_opts_set_simulate(burn_options, simulate_burn);
    burn::burn_write_opts_set_underrun_proof(burn_options, underrun_proof);
    burn::burn_drive_set_speed(m_drive, read_speed, write_speed);
    char reasons[BURN_REASONS_LEN];

    if(burn::burn_write_opts_auto_write_type(burn_options, target_disc, reasons, 0) == burn::BURN_WRITE_NONE)
    {
        std::cout << "FATAL: Failed to find a suitable write mode with this media.\n"
                  << "Reasons given:" << std::endl
                  << reasons << std::endl;
        strError = "Не удалось найти подходящий режим записи для этого диска. Причина: " + std::string(reasons);
        return -1;
    }

    /*Finally write iso to disc*/
    burn::burn_disc_write(burn_options, target_disc);

    while(burn::burn_drive_get_status(m_drive, NULL) == burn::BURN_DRIVE_SPAWNING)
        usleep(100002);

    while(burn::burn_drive_get_status(m_drive, &b_progress) != burn::BURN_DRIVE_IDLE)
    {
        if(b_progress.sectors > 0 && b_progress.sector >= 0)
        {
            int p = 100.0 * b_progress.sector / b_progress.sectors;
            std::cout << p << std::endl;
            progress(p);
        }
    }

    if(burn::burn_is_aborting(0) > 0)
    {
        std::cout << "aborting" << std::endl;
    }

    /*Free all struct*/
    if(burn_options != NULL)
        burn::burn_write_opts_free(burn_options);
    if(fifo_src != NULL)
        burn::burn_source_free(fifo_src);
    if(track != NULL)
        burn::burn_track_free(track);
    if(session != NULL)
        burn::burn_session_free(session);
    if(target_disc != NULL)
        burn::burn_disc_free(target_disc);
    if(data_src != NULL)
        burn::burn_source_free(data_src);

    burn::burn_drive_release(m_drive, 1);
    m_drive = NULL;

    return 1;
}

int Burn::blankDisc(int blank_fast, std::function<void(float)> progress)
{
    int current_profile;
    char name_po[80];
    enum burn::burn_disc_status disc_state;
    struct burn::burn_progress p;
    double percent = 1.0;

    disc_state = burn::burn_disc_get_status(m_drive);

    burn::burn_disc_get_profile(m_drive, &current_profile, name_po);

    if(current_profile == 0x13)
    {
        ; /* formatted DVD-RW will get blanked to sequential state */
    }

    switch(disc_state)
    {
    case burn::BURN_DISC_BLANK:
        std::cout << "Media already blank" << std::endl;
        strError = "Диск уже пустой";
        return -1;
    case burn::BURN_DISC_EMPTY:
        std::cout << "No media detected in drive\n";
        strError = "В CD-ROM нет диска";
        return -1;
    case burn::BURN_DISC_FULL:
        break;
    case burn::BURN_DISC_APPENDABLE:
        break;
    default:
        std::cout << "Unsuitable drive and media state" << std::endl;
        strError = "Не подходящий состояние диска";
        return -1;
    }

    if(!burn::burn_disc_erasable(m_drive)) {
        std::cout << "Media is not of erasable type" << std::endl;
        strError = "Диск не может быть отчищен";
        return -1;
    }

    burn::burn_disc_erase(m_drive, blank_fast);

    while(burn::burn_drive_get_status(m_drive, &p) != burn::BURN_DRIVE_IDLE)
    {
        if(p.sectors>0 && p.sector>=0) /* display 1 to 99 percent */
            percent = 1.0 + ((double) p.sector+1.0)
                    / ((double) p.sectors) * 98.0;
        progress(percent);
        std::cout << "Blanking: " << percent  << "% done"<< std::endl;
        sleep(1);
    }

    return 1;
}

std::string Burn::getLastError()
{
    return strError;
}

