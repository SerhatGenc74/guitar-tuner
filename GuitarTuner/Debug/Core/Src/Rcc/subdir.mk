################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Rcc/peripheral_setup.c 

OBJS += \
./Core/Src/Rcc/peripheral_setup.o 

C_DEPS += \
./Core/Src/Rcc/peripheral_setup.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Rcc/%.o Core/Src/Rcc/%.su Core/Src/Rcc/%.cyclo: ../Core/Src/Rcc/%.c Core/Src/Rcc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Rcc

clean-Core-2f-Src-2f-Rcc:
	-$(RM) ./Core/Src/Rcc/peripheral_setup.cyclo ./Core/Src/Rcc/peripheral_setup.d ./Core/Src/Rcc/peripheral_setup.o ./Core/Src/Rcc/peripheral_setup.su

.PHONY: clean-Core-2f-Src-2f-Rcc

