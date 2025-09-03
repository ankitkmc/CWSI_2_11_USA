/*
 * aes_util.c
 *
 *  Created on: Jun 11, 2025
 *      Author: saddam.hussain
 */

#include "aes.h"
#include <stdlib.h>
#include <string.h>

size_t encrypted_len = 0;

// WARNING: Hardcoded key and IV - only for demonstration/testing!
static const uint8_t aes_key[16] = {
    0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
    0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81
};
static const uint8_t aes_iv[16] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
};

// PKCS#7 padding
static uint8_t* pkcs7_pad(const uint8_t* in, size_t in_len, size_t* out_len) {
    size_t pad = AES_BLOCKLEN - (in_len % AES_BLOCKLEN);
    *out_len = in_len + pad;

    uint8_t* buf = (uint8_t*)malloc(*out_len);
    if (!buf) return NULL;

    memcpy(buf, in, in_len);
    memset(buf + in_len, pad, pad);

    return buf;
}

uint8_t* aes_cbc_encrypt(const uint8_t* data, size_t data_len, size_t* out_len) {
    if (!data || data_len == 0 || !out_len) return NULL;

    size_t padded_len = 0;
    uint8_t* padded_buf = pkcs7_pad(data, data_len, &padded_len);
    if (!padded_buf) return NULL;

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, aes_key, aes_iv);
    AES_CBC_encrypt_buffer(&ctx, padded_buf, padded_len);

    *out_len = padded_len;
    return padded_buf; // Return encrypted buffer directly
}


uint8_t* aes_cbc_decrypt(const uint8_t* encrypted_data, size_t encrypted_len, size_t* out_len) {
    if (!encrypted_data || encrypted_len == 0 || (encrypted_len % AES_BLOCKLEN != 0) || !out_len)
        return NULL;

    // Allocate buffer and copy encrypted data
    uint8_t* buf = (uint8_t*)malloc(encrypted_len);
    if (!buf) return NULL;
    memcpy(buf, encrypted_data, encrypted_len);

    // AES Decryption
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, aes_key, aes_iv);
    AES_CBC_decrypt_buffer(&ctx, buf, encrypted_len);

    // Remove PKCS#7 padding
    uint8_t pad = buf[encrypted_len - 1];
    if (pad > AES_BLOCKLEN) {
        free(buf);
        return NULL; // Invalid padding
    }

    *out_len = encrypted_len - pad;

    // Optional: null-terminate if treating as string
    // buf[*out_len] = '\0'; // Only if needed

    return buf;
}


