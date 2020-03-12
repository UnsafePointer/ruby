#include "CDImage.hpp"

using namespace std;

CDImage::CDImage() : file(), logger(LogLevel::NoLog) {
}

CDImage::~CDImage() {
    file.close();
}

void CDImage::open(std::filesystem::path filePath) {
    file.open(filePath, ios::binary);
    if (!file.is_open()) {
        logger.logError("Unable to load CD-ROM image file");
    }
}

CDSector CDImage::readSector(uint32_t location) {
    // TODO: there is a two seconds pre-gap at the first index of the single-track data CDs *only*
    location -= (2 * SectorsPerSecond);

    CDSector sector;
    file.seekg(location * sizeof(CDSector), file.beg);
    file.read(reinterpret_cast<char *>(&sector), sizeof(sector));
    return sector;
}
