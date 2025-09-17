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

#ifdef __cplusplus
extern "C" {
#endif
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

int j1=0;
char buffer[2048];
string json;
string json3="";
string json_sd;
char buf[2048];
UINT bytesRead;

class SD2 {
  public:
	FATFS fs;
	FIL file;
	FILINFO fno;
	FRESULT fresult;// result
	FRESULT fresult123;
	FIL file123;
	UINT br, bw;  // File read/write count

		SD2(){
			fresult = f_mount(&fs, "/", 1);
		}
		~SD2(){
		fresult = f_mount(NULL, "/", 1);
		}

	void write3(){
			FIL file;
			if(fresult == FR_OK){
			    fresult = f_open(&file, "file121.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
			          f_puts("This data is from the FILE111.txt. And it was written using ...f_puts... ", &file);
			          fresult = f_close(&file); }
			}
	void write4(const char* data){
		refresh_counter();
		// SD1();
		fresult = f_mount(&fs, "/", 1);
		if(this->fresult == FR_OK){
			this->fresult = f_open(&(this->file), "file131.txt", FA_OPEN_APPEND | FA_READ | FA_WRITE);
			f_write(&(this->file), data, strlen(data), &bw); // Write string data
			//f_write(&(this->file), "\n", strlen("\n"), &bw); // Adding new line
			HAL_Delay(1000);
			this->fresult = f_close(&(this->file));// Close file

		}
		fresult = f_mount(NULL, "/", 1);
	}
	void read2(){
		refresh_counter();
		fresult = f_mount(&fs, "/", 1);
		if (both_debug.Both_read_check("\n Enter 0 to skip Reading SD card data ", 15, "0") != $EXPECTED_RESPONSE){
		if(this->fresult == FR_OK){
			this->fresult = f_open(&(this->file), "file131.txt", FA_READ);
			HAL_Delay(200);
		    this->fresult = f_read(&(this->file), buffer, sizeof(buffer) , &br);
		    HAL_Delay(1000);
		    both_debug.Print2("\n ");
		    both_debug.Print2(buffer);
		    HAL_Delay(1000);
		}
		if(this->fresult!=FR_OK){

			//HAL_UART_Transmit(&hlpuart1, (uint8_t*)("f_read error\r\n"), sizeof("f_read error\r\n")-1, 200);
			both_debug.Print2("\n f_read error\n");// Ankit
		}

	}
		fresult = f_mount(NULL, "/", 1); // unmount the sd card.
		HAL_Delay(1000);
	}
	string readJsonFromSD2(){ // read the entire file altogether

		FRESULT fresult2;
		this->fresult = f_mount(&fs, "/", 1);
		if(this->fresult == FR_OK){
	    if (f_open(&(this->file), "file131.txt", FA_READ) != FR_OK) {
	    	both_debug.Print2("\n f_read SD EXISTING DATA error\n");
	    	this->fresult = f_mount(NULL, "/", 1);
	        return "";              // open error or no file
	    }
	     both_debug.Print2("\n FETCHING SD CARD STORED DATA. \n");
	     fresult2 = f_open(&(this->file), "file131.txt", FA_READ);
	    // keep reading until EOF
	    do {
	      fresult2 = f_read(&(this->file), buf, sizeof(buf)-1, &bytesRead);
	      buf[bytesRead] = '\0';
	      json += buf;
	      both_debug.Print2(json);
	      HAL_Delay(500);
	    } while (bytesRead == sizeof(buf)-1);
	    HAL_Delay(1000);
	    fresult2 = f_close(&(this->file));
	    HAL_Delay(500);
	    this->fresult = f_mount(NULL, "/", 1);
	    both_debug.Print2("\n SD CARD STORED DATA IS SHOWN BELOW \n");
	    both_debug.Print2(json);
	    return json;
		}
		else {
			both_debug.Print2("\n f_read SD EXISTING DATA error\n");
			this->fresult = f_mount(NULL, "/", 1);
			 return "";
		}
	}
	string readJsonchunk2(size_t chunksize){
		refresh_counter();
		char buf3[chunksize + 1];
		FRESULT fresult3;
		UINT bytesRead3;
		int i=0;
			this->fresult = f_mount(&fs, "/", 1);
			if(this->fresult == FR_OK){
		    if (f_open(&(this->file), "file131.txt", FA_READ) != FR_OK) {
		    	both_debug.Print2("\n f_read SD EXISTING DATA error\n");
		    	this->fresult = f_mount(NULL, "/", 1);
		        return "";              // open error or no file
		    }
		    both_debug.Print2("\n FETCHING SD CARD STORED DATA in correct json chunks. \n");
		         fresult3 = f_open(&(this->file), "file131.txt", FA_READ);
		         if(fresult3 == FR_OK)
		        	 fresult3 = f_lseek(&(this->file),offset3);
		       do{
		        	 fresult3 = f_read(&(this->file), buf3, chunksize, &bytesRead3);
		        	 buf3[bytesRead3] = '\0';
		             json3 += buf3;
		        	  i++;
		        	  if(json3.length()>(chunksize/2)){
		        		 // if(json3[json3.length()-2] == '}' && json3[json3.length()-1]== '}')
		        		  if(json3[json3.length()-1]== '}')
		        		if(json3.length() < chunksize)
		        			{ j1++; }
		        		goto LABEL2;
		        	  }
		        	  HAL_Delay(500);
		         } while(bytesRead3 == chunksize);
		       LABEL2 :
			//   while(json3.size() >2 && !(json3[json3.size()-2] == '}' && json3.back() == '}')){
			   while(json3.size() >2 && !(json3.back() == '}')){
			   json3.pop_back();
			   }
		       both_debug.Print2(json3);
		       both_debug.Print2("\n");
		       file_flag++;
		          fresult3 = f_close(&(this->file));
		          this->fresult = f_mount(NULL, "/", 1);
		       return json3;
	     }
			else {
				both_debug.Print2("\n f_read SD EXISTING DATA error\n");
						this->fresult = f_mount(NULL, "/", 1);
						 return "";
			}

	}
	DWORD SDFileSize2(){
		refresh_counter();
		FILINFO fileInfo1;
		     DWORD sizeout1;
		     FRESULT res1 = f_stat("file131.txt", &fileInfo1);
		             if(res1 == FR_OK)
		             	sizeout1 = fileInfo1.fsize;
		             return sizeout1;
	}
	string readchunk2(size_t chunksize){ // READ THE FILE IN CHUNKS
	    char buf1[chunksize + 1];
	    UINT bytesRead1;

	    this->fresult = f_mount(&fs, "/", 1);
	    if(this->fresult == FR_OK){
	        if (f_open(&(this->file), "file131.txt", FA_READ) != FR_OK) {
	        	both_debug.Print2("\n f_read SD EXISTING DATA error\n");
	        	this->fresult = f_mount(NULL, "/", 1);
	            return "";              // open error or no file
	        }

	        both_debug.Print2("\n FETCHING SD CARD STORED DATA IN CHUNKS. \n");
	        fresult123 = f_open(&(this->file123), "file131.txt", FA_READ);
	        if(fresult123 == FR_OK)
	        fresult123 = f_lseek(&(this->file123), offset);
	        if(fresult123 == FR_OK)
	        fresult123 = f_read(&(this->file123), buf1, chunksize, &bytesRead1);
	        buf1[bytesRead1] = '\0';
	    }

	    file_flag++;
	   fresult123 = f_close(&(this->file123));
	   this->fresult = f_mount(NULL, "/", 1);
	   both_debug.Print2("\n");
	   both_debug.Print2(string(buf1, bytesRead1));
	    return string(buf1, bytesRead1);
	}
	 bool isEmpty2(){  // CHECK WHETHER FILE EXISTS OR NOT, OR BLANK FILE
		 refresh_counter();
		 FILINFO finfo;
		 FRESULT res;
		 this->fresult = f_mount(&fs, "/", 1);
		 if(this->fresult == FR_OK){
			 res = f_stat("file131.txt", &finfo);
			 if (res == FR_NO_FILE) {
			         // File doesn’t exist → “empty”
				 this->fresult = f_mount(NULL, "/", 1);
			         return true;
			     }
			     else if (res != FR_OK) {
			         // Other error: treat as empty or handle error
			    	 this->fresult = f_mount(NULL, "/", 1);
			    	 return true;
			     }

			     // finfo.fsize is file size in bytes
			 this->fresult = f_mount(NULL, "/", 1);
			     return (finfo.fsize == 0);

		 }
	 }
	 void deletefile2(){
		 refresh_counter();
		 FIL file1;
		 FRESULT fresult1;
		 fresult = f_mount(&fs, "/", 1);
		 if(this->fresult == FR_OK){
		 fresult1 = f_open(&file1, "file131.txt",  FA_CREATE_ALWAYS | FA_WRITE);
		 // Move current write pointer to start
		    if(fresult1 == FR_OK){
		    both_debug.Print2("\n SD CARD stored data is cleared and sd card is empty now. \n");
		    	f_close(&file1);
		    }
		    else
		    {
		    	both_debug.Print2("\n Error in Deleting the file \n");
		    }
		fresult = f_mount(NULL, "/", 1);
	 }
	 }

};
extern SD2 sd_card2;
#endif
#ifdef __cplusplus
}
#endif
#endif
