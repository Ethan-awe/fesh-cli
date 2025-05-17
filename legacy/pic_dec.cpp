#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FESH128_128.cpp"

#define u8 unsigned char
#define HEADER_SIZE 54

int read_hex_key(const char* filename, u8* key, int key_len) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open key file: %s\n", filename);
        return -1;
    }

    char hex[KEYLENGTH * 2 + 1] = { 0 };
    fscanf(fp, "%32s", hex);
    fclose(fp);

    for (int i = 0; i < key_len; i++) {
        sscanf(hex + i * 2, "%2hhx", &key[i]);
    }
    return 0;
}

void decrypt_bmp(const char* input_bmp, const char* output_bmp, u8* key, int mode) {
    FILE *in_fp = NULL, *out_fp = NULL;
    u8 header[HEADER_SIZE];

    fopen_s(&in_fp, input_bmp, "rb");
    if (!in_fp) {
        printf("Failed to open encrypted BMP: %s\n", input_bmp);
        return;
    }

    fread(header, 1, HEADER_SIZE, in_fp);
    fseek(in_fp, 0, SEEK_END);
    int file_size = ftell(in_fp);
    int data_size = file_size - HEADER_SIZE;
    fseek(in_fp, HEADER_SIZE, SEEK_SET);

    u8* encrypted = (u8*)malloc(data_size);
    fread(encrypted, 1, data_size, in_fp);
    fclose(in_fp);

    int padded_size = ((data_size + 15) / 16) * 16;
    u8* padded = (u8*)calloc(padded_size, 1);
    memcpy(padded, encrypted, data_size);
    free(encrypted);

    u8* decrypted = (u8*)malloc(padded_size);
    int out_len = 0;

    if (mode == 1) {
        Crypt_Dec_Block_CBC(padded, padded_size, decrypted, &out_len, key, 16);
        printf("CBC decryption done.\n");
    } else {
        Crypt_Dec_Block(padded, padded_size, decrypted, &out_len, key, 16);
        printf("ECB decryption done.\n");
    }

    fopen_s(&out_fp, output_bmp, "wb");
    if (!out_fp) {
        printf("Failed to open output BMP: %s\n", output_bmp);
        free(padded);
        free(decrypted);
        return;
    }

    fwrite(header, 1, HEADER_SIZE, out_fp);
    fwrite(decrypted, 1, data_size, out_fp);
    fclose(out_fp);

    free(padded);
    free(decrypted);
    printf("Decrypted image written to: %s\n", output_bmp);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: pic_dec <input_image.bmp> <ECB|CBC>\n");
        return -1;
    }

    const char* input_file = argv[1];
    const char* mode_str = argv[2];
    int mode = (strcmp(mode_str, "CBC") == 0) ? 1 : 0;

    u8 key[16];
    if (read_hex_key("data/key.txt", key, 16) != 0) {
        return -1;
    }

    char output_file[100];
    sprintf(output_file, "fig/d_%s.bmp", mode_str);

    decrypt_bmp(input_file, output_file, key, mode);
    return 0;
}
