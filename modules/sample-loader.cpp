#include "Arduino.h"
#include <../modules/player.cpp>

struct sample_loader_t
{
    int position;
    File root;
    File directory;
    File files[4];

    void setup()
    {
        root = SD.open("/samples");
        for (int i = 0; i < position + 1; i++)
        {
            directory = root.openNextFile();
        }

        load_file_names();
        player.setup(files);
    }

    void navigate_down()
    {
        File last_directory = directory;
        directory = root.openNextFile();

        if (!directory)
        {
            directory = last_directory;
            return;
        }

        load_file_names();
        player.setup(files);

        position += 1;
    }

    void navigate_up()
    {
        if (position < 2)
        {
            return;
        }

        int last_position = position;
        root.rewindDirectory();
        directory = root.openNextFile();
        position = 0;

        for (int i = 0; i < last_position - 1; i++)
        {
            navigate_down();
        }
    }

    void load_file_names()
    {
        files[0] = directory.openNextFile();
        files[1] = directory.openNextFile();
        files[2] = directory.openNextFile();
        files[3] = directory.openNextFile();
    }
};

sample_loader_t sample_loader;