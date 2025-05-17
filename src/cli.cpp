#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <cstring>
#include "fesh.h"
#include "modes.h"
#include "image.h"

std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i + 1 < hex.length(); i += 2) {
        uint8_t byte = static_cast<uint8_t>(std::stoi(hex.substr(i, 2), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

void generate_random_bytes(uint8_t* out, size_t len) {
    std::random_device rd;
    for (size_t i = 0; i < len; ++i) out[i] = static_cast<uint8_t>(rd());
}

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << "Usage:\n";
        std::cout << "  fesh_cli.exe encrypt|decrypt --mode ecb|cbc --in <infile> --out <outfile> [--key <hex>] [--iv <hex>]\n";
        return 1;
    }

    std::string operation = argv[1];
    std::string mode, infile, outfile;
    std::string key_hex, iv_hex;

    // 简单参数解析
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mode")     mode     = argv[++i];
        else if (arg == "--in")  infile   = argv[++i];
        else if (arg == "--out") outfile  = argv[++i];
        else if (arg == "--key") key_hex  = argv[++i];
        else if (arg == "--iv")  iv_hex   = argv[++i];
    }

    // 解析 key / iv 或生成随机
    uint8_t key[16], iv[16];
    if (!key_hex.empty())
        std::memcpy(key, hex_to_bytes(key_hex).data(), 16);
    else
        generate_random_bytes(key, 16);

    if (!iv_hex.empty())
        std::memcpy(iv, hex_to_bytes(iv_hex).data(), 16);
    else
        generate_random_bytes(iv, 16);

    // 判断是否是 BMP 图像
    bool is_bmp = infile.size() >= 4 && infile.substr(infile.size() - 4) == ".bmp";

    std::vector<uint8_t> data, header;

    if (is_bmp) {
        if (!fesh::image::LoadBMP(infile, header, data)) {
            std::cerr << "Failed to load BMP file.\n";
            return 1;
        }
    } else {
        std::ifstream in(infile, std::ios::binary);
        if (!in) { std::cerr << "Failed to open input file.\n"; return 1; }
        data = std::vector<uint8_t>(std::istreambuf_iterator<char>(in), {});
    }

    std::vector<uint8_t> result;

    if (operation == "encrypt") {
        if (mode == "ecb")      result = fesh::modes::EncryptECB(data, key);
        else if (mode == "cbc") result = fesh::modes::EncryptCBC(data, key, iv);
        else { std::cerr << "Unknown mode.\n"; return 1; }
    } else if (operation == "decrypt") {
        if (mode == "ecb")      result = fesh::modes::DecryptECB(data, key);
        else if (mode == "cbc") result = fesh::modes::DecryptCBC(data, key, iv);
        else { std::cerr << "Unknown mode.\n"; return 1; }
    } else {
        std::cerr << "Unknown operation: use encrypt or decrypt\n";
        return 1;
    }

    if (is_bmp) {
        fesh::image::SaveBMP(outfile, header, result);
    } else {
        std::ofstream out(outfile, std::ios::binary);
        out.write(reinterpret_cast<const char*>(result.data()), result.size());
    }

    std::cout << "Done: " << outfile << std::endl;
    return 0;
}
