#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <cstring>
#include "fesh.h"
#include "modes.h"
#include "image.h"
// #include <iomanip>   // std::setw, std::setfill
// #include <algorithm> // std::min
// #include <algorithm>



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
    // 必须先判断是否为单独的 --gen-key 命令
    if (argc == 2 && std::string(argv[1]) == "--gen-key") {
        uint8_t key[16], iv[16];
        std::random_device rd;
        for (int i = 0; i < 16; ++i) {
            key[i] = static_cast<uint8_t>(rd());
            iv[i]  = static_cast<uint8_t>(rd());
        }

        auto print_hex = [](const char* label, const uint8_t* data) {
            std::cout << label;
            for (int i = 0; i < 16; ++i)
                std::cout << std::hex << std::setw(2) << std::setfill('0') << int(data[i]);
            std::cout << std::endl;
        };

        print_hex("Generated KEY: ", key);
        print_hex("Generated IV:  ", iv);
        return 0;
    }

    if (argc >= 4 && std::string(argv[1]) == "--verify") {
    std::string mode, infile, key_hex, iv_hex;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mode")     mode    = argv[++i];
        else if (arg == "--in")  infile  = argv[++i];
        else if (arg == "--key") key_hex = argv[++i];
        else if (arg == "--iv")  iv_hex  = argv[++i];
    }

    if (mode.empty() || infile.empty() || key_hex.empty() || (mode == "cbc" && iv_hex.empty())) {
        std::cerr << "Missing required arguments for --verify.\n";
        return 1;
    }

    // 读取输入数据
    std::ifstream in(infile, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }
    std::vector<uint8_t> plain(std::istreambuf_iterator<char>(in), {});

    uint8_t key[16], iv[16];
    std::memcpy(key, hex_to_bytes(key_hex).data(), 16);
    if (mode == "cbc") std::memcpy(iv, hex_to_bytes(iv_hex).data(), 16);

    std::vector<uint8_t> encrypted, decrypted;

    if (mode == "ecb") {
        encrypted = fesh::modes::EncryptECB(plain, key);
        decrypted = fesh::modes::DecryptECB(encrypted, key);
    } else if (mode == "cbc") {
        encrypted = fesh::modes::EncryptCBC(plain, key, iv);
        decrypted = fesh::modes::DecryptCBC(encrypted, key, iv);
    } else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    bool match = (plain == decrypted);
    std::cout << "Verification result: " << (match ? "[OK] success" : "[X] failed") << std::endl;
    return 0;
}


    // 其他命令解析逻辑
    if (argc < 6) {
        std::cout << "Usage:\n";
        std::cout << "  fesh_cli.exe encrypt|decrypt --mode ecb|cbc --in <infile> --out <outfile> [--key <hex>] [--iv <hex>]\n";
        return 1;
    }

    // ... 后面保持不变

    std::string operation = argv[1];
    std::string mode, infile, outfile;
    std::string key_hex, iv_hex;
    bool dump_hex = false;


    // 简单参数解析
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mode")     mode     = argv[++i];
        else if (arg == "--in")  infile   = argv[++i];
        else if (arg == "--out") outfile  = argv[++i];
        else if (arg == "--key") key_hex  = argv[++i];
        else if (arg == "--iv")  iv_hex   = argv[++i];
        else if (arg == "--hex-dump") dump_hex = true;

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

//     if (dump_hex) {
//     std::cout << "Hex dump (first 64 bytes):\n";
//     for (size_t i = 0; i < std::min<size_t>(64, result.size()); ++i) {
//         std::cout << std::hex << std::setw(2) << std::setfill('0')
//                   << static_cast<int>(result[i]) << " ";
//         if ((i + 1) % 16 == 0) std::cout << "\n";
//     }
//     if (result.size() < 64) std::cout << "\n";
// }

    auto print_hex = [](const char* label, const uint8_t* data) {
    std::cout << label;
    for (int i = 0; i < 16; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << int(data[i]);
    std::cout << std::endl;
};

print_hex("Using KEY: ", key);
if (mode == "cbc") print_hex("Using IV:  ", iv);


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
    std::cout << "Encrypted size: " << result.size() << "\n";

if (dump_hex) {
    std::cout << "Hex dump (first 64 bytes):\n";
    for (size_t i = 0; i < std::min<size_t>(64, result.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(result[i]) << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }

    // 🔧 保底换行
    std::cout << std::endl;
}

    return 0;
}
