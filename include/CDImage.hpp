#pragma once
#include <string>
#include <fstream>
#include <cstdint>

const uint32_t SecondsPerMinute = 60;
const uint32_t SectorsPerSecond = 75;

/*
Mode2/Form1 (CD-XA)
000h 0Ch  Sync
00Ch 4    Header (Minute,Second,Sector,Mode=02h)
010h 4    Sub-Header (File, Channel, Submode AND DFh, Codinginfo)
014h 4    Copy of Sub-Header
018h 800h Data (2048 bytes)
818h 4    EDC (checksum accross [010h..817h])
81Ch 114h ECC (error correction codes)
*/
struct CDSector {
    uint8_t sync[12];
    uint8_t header[4];
    uint8_t subheader[4];
    uint8_t subheaderCopy[4];
    uint8_t data[2048];
    uint8_t EDC[4];
    uint8_t ECC[276];
};

class CDImage {
    std::ifstream file;
public:
    CDImage();
    ~CDImage();

    void open(std::string filePath);
    CDSector readSector(uint32_t location);
};
