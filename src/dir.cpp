#include <string>
#include <fstream>
#include <vector>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include "dir.h"
#include "cons.h"

extern console info;
extern font *shared;

dirList::dirList(const std::string& _path)
{
    path = _path;
    d = opendir(path.c_str());

    while((ent = readdir(d)))
        item.push_back(ent->d_name);

    closedir(d);
}

void dirList::reassign(const std::string& _path)
{
    path = _path;

    d = opendir(path.c_str());

    item.clear();

    while((ent = readdir(d)))
        item.push_back(ent->d_name);

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

    if(!f.is_open())
    {
        info.out(shared, std::string("    *ERROR* Opening #" + from + "# for reading!"));
        info.nl();
        return;
    }
    else if(!t.is_open())
    {
        info.out(shared, std::string("    *ERROR* Opening #" + to + "# for writing!"));
        info.nl();
        return;
    }

    f.seekg(0, f.end);
    size_t fileSize = f.tellg();
    f.seekg(0, f.beg);

    char tmp[32];
    sprintf(tmp, "%luKB.", fileSize / 1024);
    info.out(shared, " *Size*: " + std::string(tmp));

    uint8_t *buff = new uint8_t[0x80000];
    for(unsigned i = 0; i < fileSize; )
    {
        size_t tPos = t.tellp();

        f.read((char *)buff, 0x80000);
        t.write((char *)buff, f.gcount());

        size_t writeSize = (size_t)t.tellp() - tPos;

        if(writeSize < (unsigned)f.gcount())
        {
            char errorMess[128];
            sprintf(errorMess, "    *ERROR:* Read/Write mismatch. %lu/%lu.", f.gcount(), writeSize);
            info.nl();
            info.out(shared, errorMess);
            info.nl();
        }

        i += f.gcount();
    }

    delete[] buff;

    f.close();
    t.close();
}

void copyDirToDir(const std::string& from, const std::string& to)
{
    dirList list(from);
    char countStr[12];

    sprintf(countStr, "%u", list.getCount());
    info.out(shared, std::string("#" + from + "# opened. ^" + countStr + "^ items found."));
    info.nl();

    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newFrom = from + list.getItem(i) + "/";
            std::string newTo   = to + list.getItem(i);
            mkdir(newTo.c_str(), 0777);
            newTo += "/";

            copyDirToDir(newFrom, newTo);
            info.out(shared, "Finished with #" + from + list.getItem(i) + "#!");
            info.nl();
        }
        else
        {
            std::string fullFrom = from + list.getItem(i);
            std::string fullTo   = to   + list.getItem(i);
            char outLine[256];
            sprintf(outLine, "Copying _%s_ (%03d/%03d).", list.getItem(i).c_str(), i + 1, list.getCount());

            info.out(shared, outLine);
            copyFile(fullFrom, fullTo);
            info.out(shared, "    ^Done^!");
            info.nl();
        }
    }
}

void delDir(const std::string& path)
{
    dirList list(path);
    for(unsigned i = 0; i < list.getCount(); i++)
    {
        if(list.isDir(i))
        {
            std::string newPath = path + list.getItem(i) + "/";
            delDir(newPath);

            std::string delPath = path + list.getItem(i);
            rmdir(delPath.c_str());
        }
        else
        {
            std::string delPath = path + list.getItem(i);
            std::remove(delPath.c_str());
        }
    }

    rmdir(path.c_str());
}
