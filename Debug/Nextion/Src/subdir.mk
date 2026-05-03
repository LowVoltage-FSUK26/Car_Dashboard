################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Nextion/Src/Nextion_prog.c 

OBJS += \
./Nextion/Src/Nextion_prog.o 

C_DEPS += \
./Nextion/Src/Nextion_prog.d 


# Each subdirectory must supply rules for building sources it contributes
Nextion/Src/%.o Nextion/Src/%.su Nextion/Src/%.cyclo: ../Nextion/Src/%.c Nextion/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/User/STM32CubeIDE/workspace_1.19.0/Dashboard_2/Dashboard/Inc" -I"C:/Users/User/STM32CubeIDE/workspace_1.19.0/Dashboard_2/Nextion/Inc" -I"C:/Users/User/STM32CubeIDE/workspace_1.19.0/Dashboard_2/PROX/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Nextion-2f-Src

clean-Nextion-2f-Src:
	-$(RM) ./Nextion/Src/Nextion_prog.cyclo ./Nextion/Src/Nextion_prog.d ./Nextion/Src/Nextion_prog.o ./Nextion/Src/Nextion_prog.su

.PHONY: clean-Nextion-2f-Src

