#pragma once
#include <cstdint>
#include <array>

namespace fesh {

using Block128 = std::array<uint32_t, 4>;           // 一个块是4个32位字
using RoundKeys = std::array<Block128, 17>;         // 16轮+1个初始轮密钥（FESH-128）

void KeySchedule(const uint8_t key[16], RoundKeys& rk);
void EncryptBlock(const uint8_t plaintext[16], uint8_t ciphertext[16], const RoundKeys& rk);
void DecryptBlock(const uint8_t ciphertext[16], uint8_t plaintext[16], const RoundKeys& rk);

}  // namespace fesh
