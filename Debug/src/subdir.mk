################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/EUCC.c \
../src/HAL_CAN_INIT.c \
../src/HAL_UART_out.c \
../src/main.c \
../src/stm32f429i_discovery.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c 

OBJS += \
./src/EUCC.o \
./src/HAL_CAN_INIT.o \
./src/HAL_UART_out.o \
./src/main.o \
./src/stm32f429i_discovery.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o 

C_DEPS += \
./src/EUCC.d \
./src/HAL_CAN_INIT.d \
./src/HAL_UART_out.d \
./src/main.d \
./src/stm32f429i_discovery.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DARM_MATH_CM4 -DTRACE -DSTM32F429xx -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4xx" -I"\\Mac\Home\Downloads\STM_6U_FPA\src" -I"\\Mac\Home\Downloads\STM_6U_FPA\system\src" -I"\\Mac\Home\Downloads\STM_6U_FPA\system\src\stm32f4xx" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


