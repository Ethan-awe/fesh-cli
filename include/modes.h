#pragma once
#include <vector>
#include <cstdint>
#include "fesh.h"

namespace fesh::modes {

// PKCS#7 填充（自动补满16字节）
std::vector<uint8_t> PadPKCS7(const std::vector<uint8_t>& data);

// 去除填充
std::vector<uint8_t> UnpadPKCS7(const std::vector<uint8_t>& padded);

// ECB 模式加解密（支持任意长度）
std::vector<uint8_t> EncryptECB(const std::vector<uint8_t>& data, const uint8_t key[16]);
std::vector<uint8_t> DecryptECB(const std::vector<uint8_t>& data, const uint8_t key[16]);

std::vector<uint8_t> EncryptCBC(const std::vector<uint8_t>& data,
                                const uint8_t key[16],
                                const uint8_t iv[16]);

std::vector<uint8_t> DecryptCBC(const std::vector<uint8_t>& ciphertext,
                                const uint8_t key[16],
                                const uint8_t iv[16]);

}
