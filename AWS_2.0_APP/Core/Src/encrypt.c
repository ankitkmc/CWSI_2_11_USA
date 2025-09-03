/*
 * encrypt.c
 *
 *  Created on: Jun 25, 2025
 *      Author: saddam.hussain
 */

#include "usart.h"
#include "aes.h"
#include "cencode.h"
#include <string.h>


char encrypted_data[MAX_BUFFER_LEN];

//char* encrypted_data_1 = NULL;

void uart_send(char* msg) {
    HAL_UART_Transmit(&hlpuart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void base64_and_uart_send(uint8_t* input, size_t input_len) {
    char output_1[1024]; // Enough to hold Base64 of up to ~340 bytes of input
    base64_encodestate s;
    base64_init_encodestate(&s);

    int len = base64_encode_block((const char*)input, input_len, output_1, &s);
    len += base64_encode_blockend(output_1 + len, &s);
    output_1[len] = '\0';
//    if(encrypted_data_1!=NULL){
//    	memcpy(encrypted_data_1, output_1, len + 1);
//    }else{
//    	uart_send("\n***Base 64 failed due to memory allocation failed**\n");
//    }
    memcpy(encrypted_data, output_1, len + 1); //change

//    HAL_UART_Transmit(&hlpuart1, (uint8_t*)output_1, len, HAL_MAX_DELAY);
//    uart_send("\n*****************New Encrypted*************\n");
//    HAL_UART_Transmit(&hlpuart1, (uint8_t*)encrypted_data, len, HAL_MAX_DELAY);
}

void main_encrypt_and_send(char* msg) {
    size_t encrypted_len = 0;  // decrypted_len = 0;
    uint8_t encrypted[MAX_BUFFER_LEN] = {0};

//    uint8_t decrypted[MAX_BUFFER_LEN] = {0};
//    uart_send("\n*****************AES*************\n");
    // Encrypt (use buffer version)
    uint8_t* encrypted_ptr = aes_cbc_encrypt((const uint8_t*)msg, strlen(msg), &encrypted_len);
    if (!encrypted_ptr) {
        uart_send("\r\nEncryption failed! in main_encrypt due to ptr\r\n");
        return;
    }

    if (encrypted_len > MAX_BUFFER_LEN) {
        uart_send("\r\nEncryption failed! in main_encrypt due buffer size\r\n");
        return;
    }

    memcpy(encrypted, encrypted_ptr, encrypted_len); // Copy to static buffer if malloc is used internally
//    memcpy(encrypt_msg, encrypted, encrypted_len);
    uart_send("Encrypted Base64:\r\n");
    base64_and_uart_send(encrypted, encrypted_len);
    uart_send("\r\n");

}


