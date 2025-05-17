#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>  // 用于高精度计时器
#include "FESH128_128.cpp"

#define u8 unsigned char
#define u32 unsigned int
#define KEYLENGTH 16
#define ROUNDS 16

void save_round_keys(u32* SK, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Cannot open %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < (ROUNDS + 1) * 4; i++) {
        fprintf(fp, "%08x", SK[i]);
        if ((i + 1) % 4 == 0)
            fprintf(fp, "\n");
        else
            fprintf(fp, " ");
    }

    fclose(fp);
}

int read_text_file(const char* filename, u8** buffer, int* length) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("Cannot open %s for reading.\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    rewind(fp);

    *buffer = (u8*)malloc(*length);
    if (!*buffer) {
        fclose(fp);
        return -1;
    }

    fread(*buffer, 1, *length, fp);
    fclose(fp);
    return 0;
}

int read_hex_key(const char* filename, u8* key, int key_len) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return -1;

    char hex[KEYLENGTH * 2 + 1] = { 0 };
    fscanf(fp, "%32s", hex);
    fclose(fp);

    for (int i = 0; i < key_len; i++) {
        sscanf(hex + i * 2, "%2hhx", &key[i]);
    }

    return 0;
}

void write_hex_file(const char* filename, u8* data, int length) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Cannot open %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < length; i++) {
        fprintf(fp, "%02x", data[i]);
    }

    fclose(fp);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: encrypt <input_text.txt> <key.txt> <ECB|CBC>\n");
        return -1;
    }

    const char* input_text_file = argv[1];
    const char* input_key_file = argv[2];
    const char* mode = argv[3];
    const char* output_cipher_file = "data/c.txt";

    u8* plaintext = NULL;
    int text_len = 0;
    if (read_text_file(input_text_file, &plaintext, &text_len) != 0) {
        printf("Failed to read plaintext.\n");
        return -1;
    }

    u8 key[KEYLENGTH];
    if (read_hex_key(input_key_file, key, KEYLENGTH) != 0) {
        printf("Failed to read key.\n");
        free(plaintext);
        return -1;
    }

    int padded_len = ((text_len + 15) / 16) * 16;
    u8* padded = (u8*)calloc(padded_len, 1);
    memcpy(padded, plaintext, text_len);
    free(plaintext);

    u8* ciphertext = (u8*)malloc(padded_len);
    int out_len = 0;

    u32 round_keys[(ROUNDS + 1) * 4];
    Key_Schedule_u(key, KEYLENGTH, 0, round_keys);
    save_round_keys(round_keys, "data/r_key.txt");

    // 高精度计时
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    if (strcmp(mode, "ECB") == 0) {
        Crypt_Enc_Block(padded, padded_len, ciphertext, &out_len, key, KEYLENGTH);
    } else if (strcmp(mode, "CBC") == 0) {
        Crypt_Enc_Block_CBC(padded, padded_len, ciphertext, &out_len, key, KEYLENGTH);
    } else {
        printf("Mode must be ECB or CBC.\n");
        free(padded);
        free(ciphertext);
        return -1;
    }

    QueryPerformanceCounter(&end);
    free(padded);

    write_hex_file(output_cipher_file, ciphertext, out_len);
    free(ciphertext);

    // 计算毫秒和吞吐量
    double elapsed_ms = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
    double throughput_MBps = (padded_len / 1024.0 / 1024.0) / (elapsed_ms / 1000.0);

    printf("Encryption completed!\n");
    printf("Time: %.3f ms\n", elapsed_ms);
    printf("Throughput: %.3f MB/s\n", throughput_MBps);

    return 0;
}
