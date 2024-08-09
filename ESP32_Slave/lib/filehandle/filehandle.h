
#ifndef FILEHANLE_H
#define FILEHANLE_H
#include "FS.h"
class FileHanle
{

public:
    static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
    static void createDir(fs::FS &fs, const char *path);

    static void writeFile(fs::FS &fs, const char *path, const char *message);

    static void readFile(fs::FS &fs, const char *path);
};

#endif