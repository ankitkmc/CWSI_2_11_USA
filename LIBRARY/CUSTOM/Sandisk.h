/*
 * Sandisk.h
 *
 *  Created on: Sep 8, 2025
 *      Author: rahul.wadhwa
 */

#ifndef SANDISK_H_
#define SANDISK_H_

#if defined(APP_CODE)

#include "INCLUDER.h"

//#ifdef __cplusplus
//extern "C" {
//#endif
#include "string.h"
#include "stdio.h"
#include "gpio.h"
#include "fatfs.h"
#include "spi.h"
#include "fatfs_sd.h"
#include "cstdio"

uint16_t file_flag;
uint32_t totalparse;
DWORD offset = 0;
DWORD offset3 = 0;
const Diskio_drvTypeDef SD_Driver = {
    SD_disk_initialize,
    SD_disk_status,
    SD_disk_read,
#if _USE_WRITE
    SD_disk_write,
#endif
#if _USE_IOCTL
    SD_disk_ioctl,
#endif
};

char buffer[2048];
string json;
string json3="";
string json_sd;

UINT bytesRead;
string sd_offset = "offset.txt";

class SD2 {
public:
    FATFS fs;
    FIL file;
    FRESULT fresult;
    UINT br, bw;
    DWORD lineCountValue = 0;
    static DWORD offsetPos;



    // Mount SD card safely
    FRESULT mountSD() {
        char path[] = "0:";

        FATFS_UnLinkDriver(path); // Optional cleanup
        if (FATFS_LinkDriver(&SD_Driver, path) != 0) {
            both_debug.Print2("Driver link failed\n");
            return FR_DISK_ERR;
        }

        HAL_Delay(10); // Ensure hardware is ready
        return f_mount(&fs, path, 1);
    }

    // Unmount SD card
    void unmountSD() {
        f_mount(NULL, "0:", 1);
    }

    // Check if file is empty or missing
    bool isEmpty(const string& filename) {
        FILINFO finfo;
        fresult = mountSD();
        if (fresult != FR_OK) return true;

        FRESULT res = f_stat(("0:/" + filename).c_str(), &finfo);
        unmountSD();

        return (res == FR_NO_FILE || finfo.fsize == 0);
    }

    // Read one JSON line with offset tracking
    string readJsonLine(const string& filename, DWORD updatePointer = 0) {
        refresh_counter();
        char line[1024];
        string jsonLine = "";

        if (updatePointer) {
            offsetPos = (updatePointer > offsetPos) ? 0 : offsetPos - updatePointer;
            handleOffset(sd_offset,to_string(offsetPos));
            return "";
        }

        fresult = mountSD();
        if (fresult == FR_OK) {
            if (f_open(&file, ("0:/" + filename).c_str(), FA_READ) == FR_OK) {
                f_lseek(&file, offsetPos);
                both_debug.Print2("\r\n-->line offset: " + d_t_s((double)offsetPos));
                if (f_gets(line, sizeof(line), &file)) {
                    jsonLine = string(line);
                    offsetPos = f_tell(&file);
                    both_debug.Print2(":" + jsonLine);
                }
                f_close(&file);
            }
            unmountSD();
        }
        unmountSD();
        return jsonLine;
    }

    // Read multiple JSON lines up to 2700 bytes
    string readJsonLine2k(const string& filename) {
        string buffer = "[";
        DWORD totalSize = 0;
        bool first = true;
        uint8_t lineCount = 0;

        while (true) {
            string line = readJsonLine(filename);
            if (line.empty()) {
                both_debug.Print2("\r\n Empty Line");
                if (lineCount == 0) Network.isDataAvailable = 0;
                updateLinecount(lineCount);
                break;
            } else {
                lineCount++;
                updateLinecount(lineCount);
            }

            DWORD lineSize = line.length();
            if (totalSize + lineSize > 2700) {
                readJsonLine(filename, lineSize); // update pointer by providing line size
                Network.isDataAvailable = 1;
                updateLinecount(--lineCount);
                break;
            }

            if (!first) buffer += ",";
            else first = false;

            buffer += line;
            totalSize += lineSize;
            both_debug.Print2("\r\nTotal size: " + d_t_s(totalSize));
        }

        if (lineCount == 1 && Network.isDataAvailable == 0) buffer.pop_back();
        buffer += "]";
        return buffer;
    }

    // Write data to file
    void write4(const string& data, const string& filename) {
        refresh_counter();
        fresult = mountSD();
        if (fresult == FR_OK) {
            if (f_open(&file, ("0:/" + filename).c_str(), FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
                f_lseek(&file, f_size(&file)); // move to end
                string withNewline = data + "\n";
                both_debug.Print2(filename + ":" + data+"\n");
                f_write(&file, withNewline.c_str(), withNewline.size(), &bw);
                f_close(&file);
            }
            unmountSD();
        }
    }

    // Line count utilities
    DWORD updateLinecount(DWORD line) {
        lineCountValue = line;
        return lineCountValue;
    }

    DWORD linecount() {
        return lineCountValue;
    }

    DWORD handleOffset(const string& filename, const string& data) {
        FIL file;
        char buffer[32];
        DWORD previousOffset = 0;

        // Mount SD card
        FRESULT res = mountSD();
        if (res != FR_OK) {
            both_debug.Print2("Mount failed\n");
            return 0;
        }

        // Step 1: Read previous offset
        res = f_open(&file, ("0:/" + filename).c_str(), FA_READ);
        if (res == FR_OK) {
            if (f_gets(buffer, sizeof(buffer), &file)) {
                previousOffset = atoi(buffer);
            }
            f_close(&file);
        }
        if (!data.empty()){
        // Step 2: Overwrite with new offset
        res = f_open(&file, ("0:/" + filename).c_str(), FA_CREATE_ALWAYS | FA_WRITE);
        if (res == FR_OK) {
            res = f_write(&file, data.c_str(), data.length(), &bw);
            f_close(&file);
            both_debug.Print2("Offset updated to: " + data);
        } else {
            both_debug.Print2("Failed to write new offset");
        }
        }
        unmountSD();
        return previousOffset;
    }
    void deletefile2(const string& filename){
   		 FIL file1;
   		 FRESULT fresult;
   		 fresult = mountSD();
   		 if(fresult == FR_OK){
   		 fresult = f_open(&file1, ("0:/" + filename).c_str(),  FA_CREATE_ALWAYS | FA_WRITE);
   		 // Move current write pointer to start
   		    if(fresult == FR_OK){
   		    both_debug.Print2("\n"+filename+": data is cleared. \n");
   		    	f_close(&file1);
   		    }
   		    else
   		    {
   		    	both_debug.Print2("\n Mount Error : File not Deleting.  \n");
   		    }
   		fresult = f_mount(NULL, "/", 1);
   	 }
   	 }

};

// Static member initialization
DWORD SD2::offsetPos = 0;

// Global instance
//SD2 sd_card2;


extern SD2 sd_card2;

//#endif
//#ifdef __cplusplus
//}
#endif
#endif
