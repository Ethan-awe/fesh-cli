#include "fesh.h"
#include <cstring>
#include <array>

namespace fesh {

// ---------------- S盒表（4比特） ----------------
static const uint8_t SBOX[16] = {
    0x6, 0x4, 0xC, 0x5,
    0x0, 0x7, 0x2, 0xE,
    0x1, 0xF, 0x3, 0xD,
    0x8, 0xA, 0x9, 0xB
};

static const uint8_t INV_SBOX[16] = {
    0x4, 0x8, 0x6, 0xA,
    0x1, 0x3, 0x0, 0x5,
    0xC, 0xE, 0xD, 0xF,
    0x2, 0xB, 0x7, 0x9
};

// ------------------ 工具函数 ------------------

static void SubBytes(Block128& state) {
    for (int bit = 0; bit < 32; ++bit) {
        uint8_t nibble = 0;
        for (int i = 0; i < 4; ++i)
            nibble |= ((state[i] >> bit) & 1) << (3 - i);

        uint8_t sub = SBOX[nibble];

        for (int i = 0; i < 4; ++i) {
            state[i] &= ~(1U << bit);
            state[i] |= ((sub >> (3 - i)) & 1) << bit;
        }
    }
}

static void InvSubBytes(Block128& state) {
    for (int bit = 0; bit < 32; ++bit) {
        uint8_t nibble = 0;
        for (int i = 0; i < 4; ++i)
            nibble |= ((state[i] >> bit) & 1) << (3 - i);

        uint8_t sub = INV_SBOX[nibble];

        for (int i = 0; i < 4; ++i) {
            state[i] &= ~(1U << bit);
            state[i] |= ((sub >> (3 - i)) & 1) << bit;
        }
    }
}

static void MixWords(Block128& state) {
    Block128 temp = state;
    state[0] = temp[0] ^ (temp[1] << 1 | temp[1] >> 31);
    state[1] = temp[1] ^ (temp[2] << 2 | temp[2] >> 30);
    state[2] = temp[2] ^ (temp[3] << 3 | temp[3] >> 29);
    state[3] = temp[3] ^ (temp[0] << 4 | temp[0] >> 28);
}

static void InvMixWords(Block128& state) {
    Block128 temp = state;
    state[0] = temp[0] ^ (temp[1] >> 1 | temp[1] << 31);
    state[1] = temp[1] ^ (temp[2] >> 2 | temp[2] << 30);
    state[2] = temp[2] ^ (temp[3] >> 3 | temp[3] << 29);
    state[3] = temp[3] ^ (temp[0] >> 4 | temp[0] << 28);
}

// ------------------ 密钥扩展 ------------------

void KeySchedule(const uint8_t key[16], RoundKeys& rk) {
    // 初始密钥 → Block128
    for (int i = 0; i < 4; ++i) {
        rk[0][i] =
            (key[4 * i + 0] << 24) |
            (key[4 * i + 1] << 16) |
            (key[4 * i + 2] << 8)  |
            (key[4 * i + 3]);
    }

    // 简化：每轮密钥 = 前一轮密钥 + round常量（异或）
    for (int r = 1; r <= 16; ++r) {
        for (int i = 0; i < 4; ++i)
            rk[r][i] = rk[r - 1][i] ^ (0x9e377900 + r + i);  // 常量可调整
    }
}

// ------------------ 加解密主流程 ------------------

void EncryptBlock(const uint8_t plaintext[16], uint8_t ciphertext[16], const RoundKeys& rk) {
    Block128 state;
    for (int i = 0; i < 4; ++i)
        state[i] =
            (plaintext[4 * i + 0] << 24) |
            (plaintext[4 * i + 1] << 16) |
            (plaintext[4 * i + 2] << 8)  |
            (plaintext[4 * i + 3]);

    // 初始轮密钥
    for (int i = 0; i < 4; ++i)
        state[i] ^= rk[0][i];

    for (int r = 1; r <= 16; ++r) {
        SubBytes(state);
        // MixWords(state);
        for (int i = 0; i < 4; ++i)
            state[i] ^= rk[r][i];
    }

    for (int i = 0; i < 4; ++i) {
        ciphertext[4 * i + 0] = (state[i] >> 24) & 0xFF;
        ciphertext[4 * i + 1] = (state[i] >> 16) & 0xFF;
        ciphertext[4 * i + 2] = (state[i] >> 8)  & 0xFF;
        ciphertext[4 * i + 3] = (state[i] >> 0)  & 0xFF;
    }
}

void DecryptBlock(const uint8_t ciphertext[16], uint8_t plaintext[16], const RoundKeys& rk) {
    Block128 state;
    for (int i = 0; i < 4; ++i)
        state[i] =
            (ciphertext[4 * i + 0] << 24) |
            (ciphertext[4 * i + 1] << 16) |
            (ciphertext[4 * i + 2] << 8)  |
            (ciphertext[4 * i + 3]);

    for (int r = 16; r >= 1; --r) {
        for (int i = 0; i < 4; ++i)
            state[i] ^= rk[r][i];
        // InvMixWords(state);
        InvSubBytes(state);
    }

    for (int i = 0; i < 4; ++i)
        state[i] ^= rk[0][i];

    for (int i = 0; i < 4; ++i) {
        plaintext[4 * i + 0] = (state[i] >> 24) & 0xFF;
        plaintext[4 * i + 1] = (state[i] >> 16) & 0xFF;
        plaintext[4 * i + 2] = (state[i] >> 8)  & 0xFF;
        plaintext[4 * i + 3] = (state[i] >> 0)  & 0xFF;
    }
}

} // namespace fesh
