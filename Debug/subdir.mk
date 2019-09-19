################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BCM.c \
../CPU_sleep.c \
../DIO.c \
../UART.c \
../UART_cfg.c \
../main.c 

OBJS += \
./BCM.o \
./CPU_sleep.o \
./DIO.o \
./UART.o \
./UART_cfg.o \
./main.o 

C_DEPS += \
./BCM.d \
./CPU_sleep.d \
./DIO.d \
./UART.d \
./UART_cfg.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


