#include "CDImage.hpp"

using namespace std;

CDImage::CDImage() : file() {
}

CDImage::~CDImage() {
    file.close();
}

void CDImage::open(string filePath) {
    file.open(filePath, ios::binary);
    if (!file.is_open()) {
        printError("Unable to load CD-ROM image file");
    }
}
