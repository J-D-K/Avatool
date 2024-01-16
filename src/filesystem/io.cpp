#include <fstream>
#include <vector>
#include <sys/stat.h>
#include "filesystem/directoryListing.hpp"
#include "filesystem/io.hpp"

void copyFile(const std::string &sourcePath, const std::string &destinationPath)
{
    std::ifstream source(sourcePath, std::ios::binary);
    std::ofstream destination(destinationPath, std::ios::binary);

    // Since we're just dealing with jpegs, read whole thing to RAM then out
    std::vector<char> fileBuffer((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
    destination.write(fileBuffer.data(), fileBuffer.size());
}

void copyDirectory(const std::string &sourcePath, const std::string &destinationPath)
{
    directoryListing sourceList(sourcePath);

    int sourceCount = sourceList.getDirectoryListingCount();
    for(int i = 0; i < sourceCount; i++)
    {
        if(sourceList.itemAtIsDirectory(i))
        {
            //New paths
            std::string newSource = sourcePath + sourceList.getDirectoryItemAt(i) + "/";
            std::string newDestination = destinationPath + sourceList.getDirectoryItemAt(i) + "/";

            //Use substring to create sub directory
            mkdir(newDestination.substr(0, newDestination.npos - 1).c_str(), 0777);

            //Recursion
            copyDirectory(newSource, newDestination);
        }
        else
        {
            //Full paths
            std::string fullSource = sourcePath + sourceList.getDirectoryItemAt(i);
            std::string fullDestination = destinationPath + sourceList.getDirectoryItemAt(i);

            //Copy it over
            copyFile(fullSource, fullDestination);
        }
    }
}