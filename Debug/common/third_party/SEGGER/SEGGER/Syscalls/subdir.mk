################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c 

OBJS += \
./common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o 

C_DEPS += \
./common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d 


# Each subdirectory must supply rules for building sources it contributes
common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o: /Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c common/third_party/SEGGER/SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I/Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/FreeRTOS/include -I/Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/FreeRTOS/portable/GCC/ARM_CM4F -I/Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/SEGGER/OS -I/Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/SEGGER/SEGGER -I/Users/robert/Documents/RTOS-Udemy/RTOS_workspace/common/third_party/SEGGER/config -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-common-2f-third_party-2f-SEGGER-2f-SEGGER-2f-Syscalls

clean-common-2f-third_party-2f-SEGGER-2f-SEGGER-2f-Syscalls:
	-$(RM) ./common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.cyclo ./common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d ./common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o ./common/third_party/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.su

.PHONY: clean-common-2f-third_party-2f-SEGGER-2f-SEGGER-2f-Syscalls

