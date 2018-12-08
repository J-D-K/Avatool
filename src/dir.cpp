#include <string>
#include <cstring>
#include <fstream>
#include <vector>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "dir.h"
#include "cons.h"

extern console info;
extern font *shared;

//Modified to only save *.jpg
void dirList::assign(const std::string& _path)
{
    path = _path;

    d = opendir(path.c_str());

    item.clear();

    while((ent = readdir(d)))
    {
        char ext[4];
        memcpy(ext, &ent->d_name[strlen(ent->d_name) - 4], 4);
        if(strcmp(".jpg", ext) == 0)
            item.push_back(ent->d_name);
    }

    closedir(d);
}

void dirList::rescan()
{
    item.clear();
    d = opendir(path.c_str());

    while((ent = readdir(d)))
        item.push_back(ent->d_name);

    closedir(d);
}

std::string dirList::getItem(int index)
{
    return item[index];
}

bool dirList::isDir(int index)
{
    std::string fullPath = path + item[index];
    struct stat s;
    return stat(fullPath.c_str(), &s) == 0 && S_ISDIR(s.st_mode);
}

unsigned dirList::getCount()
{
    return item.size();
}

void copyFile(const std::string& from, const std::string& to)
{
    std::fstream f(from, std::ios::in | std::ios::binary);
    std::fstream t(to, std::ios::out | std::ios::binary);

    f.seekg(0, f.end);
    size_t fileSize = f.tellg();
    f.seekg(0, f.beg);

    uint8_t *buff = new uint8_t[0x80000];
    for(unsigned i = 0; i < fileSize; )
    {
        f.read((char *)buff, 0x80000);
        t.write((char *)buff, f.gcount());

        i += f.gcount();
    }

    delete[] buff;

    f.close();
    t.close();
}
