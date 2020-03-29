#include "CDImage.hpp"

using namespace std;

CDImage::CDImage() : file(), logger(LogLevel::NoLog), logicalBlockAddressing(0) {
}

CDImage::~CDImage() {
    file.close();
}

void CDImage::open(std::filesystem::path filePath) {
    file.open(filePath, ios::binary | ios::ate);
    if (!file.is_open()) {
        logger.logError("Unable to load CD-ROM image file");
    }
    std::streampos size = file.tellg();
    file.seekg(0, ios::beg);
    int sectorSize = sizeof(CDSector);
    logicalBlockAddressing = size / sectorSize;
}

CDSector CDImage::readSector(uint32_t location) {
    // TODO: there is a two seconds pre-gap at the first index of the single-track data CDs *only*
    location -= (2 * SectorsPerSecond);

    CDSector sector;
    file.seekg(location * sizeof(CDSector), file.beg);
    file.read(reinterpret_cast<char *>(&sector), sizeof(sector));
    return sector;
}

unsigned int CDImage::getLBA() {
    return logicalBlockAddressing;
}
