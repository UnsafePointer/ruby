#pragma once
#include <string>
#include <fstream>

class CDImage {
    std::ifstream file;
public:
    CDImage();
    ~CDImage();

    void open(std::string filePath);
};
