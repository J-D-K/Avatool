#include <dirent.h>
#include <sys/stat.h>
#include "filesystem/directoryListing.hpp"

static std::string getExtensionFromString(const std::string &str)
{
    size_t extensionBegin = str.find_last_of('.');
    if(extensionBegin != str.npos)
    {
        return str.substr(extensionBegin + 1, str.npos);
    }
    return "";
}

directoryListing::directoryListing(const std::string &directoryPath) : m_DirectoryPath(directoryPath)
{
    //These two functions would basically be identical so
    reload();
}

bool directoryListing::itemAtIsDirectory(int index)
{
    struct stat s;
    std::string fullPathToItem = m_DirectoryPath + m_DirectoryList.at(index);
    if (stat(fullPathToItem.c_str(), &s) == 0 && S_ISDIR(s.st_mode))
    {
        return true;
    }
    return false;
}

std::string directoryListing::getDirectoryItemAtFilename(int index)
{
    size_t filenameEnd = m_DirectoryList.at(index).find_last_of(".");
    if (filenameEnd != m_DirectoryList.at(index).npos)
    {
        return m_DirectoryList.at(index).substr(0, filenameEnd);
    }
    return "";
}

std::string directoryListing::getDirectoryItemAtExtension(int index)
{
    return getExtensionFromString(m_DirectoryList.at(index));
}

void directoryListing::reload()
{
    //Clear current list
    m_DirectoryList.clear();

    //Open directory
    DIR *directory = opendir(m_DirectoryPath.c_str());
    dirent *directoryEntry;

    //Loop until failure to read
    while ((directoryEntry = readdir(directory)))
    {
        //Only use *.jpg
        if(getExtensionFromString(directoryEntry->d_name) == "jpg")
        {
            m_DirectoryList.push_back(std::string(directoryEntry->d_name));
        }
    }
    closedir(directory);
}