#include "modes.h"
#include <cstring>

namespace fesh::modes {

std::vector<uint8_t> PadPKCS7(const std::vector<uint8_t>& data) {
    size_t pad_len = 16 - (data.size() % 16);
    std::vector<uint8_t> padded = data;
    padded.insert(padded.end(), pad_len, static_cast<uint8_t>(pad_len));
    return padded;
}

std::vector<uint8_t> UnpadPKCS7(const std::vector<uint8_t>& padded) {
    if (padded.empty()) return {};
    uint8_t pad_len = padded.back();
    if (pad_len > 16 || pad_len == 0) return padded; // fallback
    return std::vector<uint8_t>(padded.begin(), padded.end() - pad_len);
}

std::vector<uint8_t> EncryptECB(const std::vector<uint8_t>& data, const uint8_t key[16]) {
    RoundKeys rk;
    KeySchedule(key, rk);
    std::vector<uint8_t> padded = PadPKCS7(data);
    std::vector<uint8_t> encrypted(padded.size());

    for (size_t i = 0; i < padded.size(); i += 16) {
        EncryptBlock(&padded[i], &encrypted[i], rk);
    }

    return encrypted;
}

std::vector<uint8_t> DecryptECB(const std::vector<uint8_t>& data, const uint8_t key[16]) {
    if (data.size() % 16 != 0) return {};
    RoundKeys rk;
    KeySchedule(key, rk);
    std::vector<uint8_t> decrypted(data.size());

    for (size_t i = 0; i < data.size(); i += 16) {
        DecryptBlock(&data[i], &decrypted[i], rk);
    }

    return UnpadPKCS7(decrypted);
}

std::vector<uint8_t> EncryptCBC(const std::vector<uint8_t>& data,
                                const uint8_t key[16],
                                const uint8_t iv[16]) {
    RoundKeys rk;
    KeySchedule(key, rk);

    std::vector<uint8_t> padded = PadPKCS7(data);
    std::vector<uint8_t> encrypted(padded.size());

    uint8_t block_in[16], block_out[16], prev_block[16];
    std::memcpy(prev_block, iv, 16);

    for (size_t i = 0; i < padded.size(); i += 16) {
        for (int j = 0; j < 16; ++j)
            block_in[j] = padded[i + j] ^ prev_block[j];

        EncryptBlock(block_in, block_out, rk);
        std::memcpy(&encrypted[i], block_out, 16);
        std::memcpy(prev_block, block_out, 16);
    }

    return encrypted;
}

std::vector<uint8_t> DecryptCBC(const std::vector<uint8_t>& ciphertext,
                                const uint8_t key[16],
                                const uint8_t iv[16]) {
    if (ciphertext.size() % 16 != 0) return {};

    RoundKeys rk;
    KeySchedule(key, rk);
    std::vector<uint8_t> decrypted(ciphertext.size());

    uint8_t block_in[16], block_out[16], prev_block[16];
    std::memcpy(prev_block, iv, 16);

    for (size_t i = 0; i < ciphertext.size(); i += 16) {
        std::memcpy(block_in, &ciphertext[i], 16);
        DecryptBlock(block_in, block_out, rk);
        for (int j = 0; j < 16; ++j)
            decrypted[i + j] = block_out[j] ^ prev_block[j];
        std::memcpy(prev_block, block_in, 16);
    }

    return UnpadPKCS7(decrypted);
}

}
