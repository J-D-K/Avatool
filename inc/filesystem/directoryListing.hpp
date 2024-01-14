#pragma once
#include <string>
#include <vector>

class directoryListing
{
    public:
        directoryListing(const std::string &directoryPath);

        bool itemAtIsDirectory(int index);
        int getDirectoryListingCount() { return m_DirectoryList.size(); }

        std::string getDirectoryItemAt(int index) { return m_DirectoryList.at(index); }
        std::string getDirectoryItemAtFilename(int index);
        std::string getDirectoryItemAtExtension(int index);

        void reload();

    private:
        std::string m_DirectoryPath;
        std::vector<std::string> m_DirectoryList;
};