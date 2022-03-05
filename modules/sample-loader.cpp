#include "Arduino.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

struct sample_loader_t
{
    File root;
    File directory;
    int position;

    void setup()
    {
        root = SD.open("/samples");
        position = 0;
        navigate_down();
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

        while (!directory.isDirectory())
        {
            navigate_down();

            return;
        }

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
        position = 0;
        
        for (int i = 0; i < last_position - 1; i++)
        {
            navigate_down();
        }
    }

    const char *get_file_path()
    {
        File file = directory.openNextFile();
        return file.name();
    }
};

sample_loader_t sample_loader;