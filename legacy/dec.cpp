#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>  // 高精度计时
#include "FESH128_128.cpp"

#define u8 unsigned char
#define u32 unsigned int
#define KEYLENGTH 16
#define ROUNDS 16

int read_hex_file(const char* filename, u8** buffer, int* length) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open %s\n", filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int hex_len = ftell(fp);
    rewind(fp);

    char* hex_string = (char*)malloc(hex_len + 1);
    fread(hex_string, 1, hex_len, fp);
    hex_string[hex_len] = '\0';
    fclose(fp);

    *length = hex_len / 2;
    *buffer = (u8*)malloc(*length);
    for (int i = 0; i < *length; i++) {
        sscanf(hex_string + i * 2, "%2hhx", &((*buffer)[i]));
    }

    free(hex_string);
    return 0;
}

int read_text_file(const char* filename, u8** buffer, int* length) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) return -1;

    fseek(fp, 0, SEEK_END);
    *length = ftell(fp);
    rewind(fp);

    *buffer = (u8*)malloc(*length);
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

// void write_text_file(const char* filename, u8* data, int length) {
//     FILE* fp = fopen(filename, "wb");
//     if (!fp) return;
//     fwrite(data, 1, length, fp);
//     fclose(fp);
// }
void write_text_file(const char* filename, u8* data, int length) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("Failed to open %s for writing.\n", filename);
        return;
    }

    // 写入 UTF-8 BOM：EF BB BF
    const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    fwrite(bom, 1, 3, fp);

    // 写入解密明文内容
    fwrite(data, 1, length, fp);
    fclose(fp);
}


int compare(u8* a, u8* b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: decrypt <ciphertext.txt> <key.txt> <ECB|CBC>\n");
        return -1;
    }

    const char* cipher_file = argv[1];
    const char* key_file = argv[2];
    const char* mode = argv[3];
    const char* output_file = "data/d.txt";

    u8 key[KEYLENGTH];
    if (read_hex_key(key_file, key, KEYLENGTH) != 0) {
        printf("Failed to read key.\n");
        return -1;
    }

    u8* ciphertext = NULL;
    int cipher_len = 0;
    if (read_hex_file(cipher_file, &ciphertext, &cipher_len) != 0) {
        printf("Failed to read ciphertext.\n");
        return -1;
    }

    u8* decrypted = (u8*)malloc(cipher_len);
    int out_len = 0;

    // 使用高精度计时器
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    if (strcmp(mode, "ECB") == 0) {
        Crypt_Dec_Block(ciphertext, cipher_len, decrypted, &out_len, key, KEYLENGTH);
    } else if (strcmp(mode, "CBC") == 0) {
        Crypt_Dec_Block_CBC(ciphertext, cipher_len, decrypted, &out_len, key, KEYLENGTH);
    } else {
        printf("Mode must be ECB or CBC.\n");
        free(ciphertext);
        free(decrypted);
        return -1;
    }

    QueryPerformanceCounter(&end);
    double elapsed_ms = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
    double throughput = (cipher_len / 1024.0 / 1024.0) / (elapsed_ms / 1000.0);

    printf("Decryption completed!\n");
    printf("Time: %.3f ms\n", elapsed_ms);
    printf("Throughput: %.3f MB/s\n", throughput);

    // 写解密结果
    write_text_file(output_file, decrypted, out_len);

    // 自动比对原始明文
    u8* original = NULL;
    int original_len = 0;
    if (read_text_file("data/m.txt", &original, &original_len) == 0) {
        int cmp_len = (original_len < out_len) ? original_len : out_len;
        if (compare(decrypted, original, cmp_len)) {
            printf("Plaintext matches original input.\n");
        } else {
            printf("Plaintext does NOT match.\n");
        }
        free(original);
    } else {
        printf("Cannot open original plaintext for comparison.\n");
    }

    free(ciphertext);
    free(decrypted);

    return 0;
}
