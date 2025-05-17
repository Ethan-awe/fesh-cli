#include "image.h"
#include <fstream>

namespace fesh::image {

bool LoadBMP(const std::string& path,
             std::vector<uint8_t>& header,
             std::vector<uint8_t>& data) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;

    header.resize(54);
    in.read(reinterpret_cast<char*>(header.data()), 54);

    in.seekg(0, std::ios::end);
    size_t file_size = in.tellg();
    size_t data_size = file_size - 54;

    data.resize(data_size);
    in.seekg(54);
    in.read(reinterpret_cast<char*>(data.data()), data_size);

    return true;
}

bool SaveBMP(const std::string& path,
             const std::vector<uint8_t>& header,
             const std::vector<uint8_t>& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;

    out.write(reinterpret_cast<const char*>(header.data()), header.size());
    out.write(reinterpret_cast<const char*>(data.data()), data.size());

    return true;
}

}
