################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/main.cpp 

C_SRCS += \
../Core/Src/adc.c \
../Core/Src/aes.c \
../Core/Src/aes_util.c \
../Core/Src/cencode.c \
../Core/Src/encrypt.c \
../Core/Src/fatfs_sd.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/iwdg.c \
../Core/Src/rtc.c \
../Core/Src/spi.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/usart.c 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/aes.d \
./Core/Src/aes_util.d \
./Core/Src/cencode.d \
./Core/Src/encrypt.d \
./Core/Src/fatfs_sd.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/iwdg.d \
./Core/Src/rtc.d \
./Core/Src/spi.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/usart.d 

OBJS += \
./Core/Src/adc.o \
./Core/Src/aes.o \
./Core/Src/aes_util.o \
./Core/Src/cencode.o \
./Core/Src/encrypt.o \
./Core/Src/fatfs_sd.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/iwdg.o \
./Core/Src/main.o \
./Core/Src/rtc.o \
./Core/Src/spi.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/usart.o 

CPP_DEPS += \
./Core/Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L433xx -DAPP_CODE -DAWS_2_0 -c -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/LIBRARY/CUSTOM" -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/ArduinoJson-6.x" -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/FATFS/App" -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/FATFS/Target" -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/Middlewares/Third_Party/FatFs/src" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.cpp Core/Src/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32L433xx -DAPP_CODE -DAWS_2_0 -c -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/LIBRARY/CUSTOM" -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/ArduinoJson-6.x" -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/FATFS/App" -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/FATFS/Target" -I"C:/CWS/CWSI_2_1/CWS_USA/CWSI_2_11_USA/AWS_2.0_APP/Middlewares/Third_Party/FatFs/src" -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/aes.cyclo ./Core/Src/aes.d ./Core/Src/aes.o ./Core/Src/aes.su ./Core/Src/aes_util.cyclo ./Core/Src/aes_util.d ./Core/Src/aes_util.o ./Core/Src/aes_util.su ./Core/Src/cencode.cyclo ./Core/Src/cencode.d ./Core/Src/cencode.o ./Core/Src/cencode.su ./Core/Src/encrypt.cyclo ./Core/Src/encrypt.d ./Core/Src/encrypt.o ./Core/Src/encrypt.su ./Core/Src/fatfs_sd.cyclo ./Core/Src/fatfs_sd.d ./Core/Src/fatfs_sd.o ./Core/Src/fatfs_sd.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/iwdg.cyclo ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/iwdg.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32l4xx_hal_msp.cyclo ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_hal_msp.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

