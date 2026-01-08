################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Paho/Src/MQTTConnectClient.c \
../Core/Paho/Src/MQTTConnectServer.c \
../Core/Paho/Src/MQTTDeserializePublish.c \
../Core/Paho/Src/MQTTFormat.c \
../Core/Paho/Src/MQTTPacket.c \
../Core/Paho/Src/MQTTSerializePublish.c \
../Core/Paho/Src/MQTTSubscribeClient.c \
../Core/Paho/Src/MQTTSubscribeServer.c \
../Core/Paho/Src/MQTTUnsubscribeClient.c \
../Core/Paho/Src/MQTTUnsubscribeServer.c 

OBJS += \
./Core/Paho/Src/MQTTConnectClient.o \
./Core/Paho/Src/MQTTConnectServer.o \
./Core/Paho/Src/MQTTDeserializePublish.o \
./Core/Paho/Src/MQTTFormat.o \
./Core/Paho/Src/MQTTPacket.o \
./Core/Paho/Src/MQTTSerializePublish.o \
./Core/Paho/Src/MQTTSubscribeClient.o \
./Core/Paho/Src/MQTTSubscribeServer.o \
./Core/Paho/Src/MQTTUnsubscribeClient.o \
./Core/Paho/Src/MQTTUnsubscribeServer.o 

C_DEPS += \
./Core/Paho/Src/MQTTConnectClient.d \
./Core/Paho/Src/MQTTConnectServer.d \
./Core/Paho/Src/MQTTDeserializePublish.d \
./Core/Paho/Src/MQTTFormat.d \
./Core/Paho/Src/MQTTPacket.d \
./Core/Paho/Src/MQTTSerializePublish.d \
./Core/Paho/Src/MQTTSubscribeClient.d \
./Core/Paho/Src/MQTTSubscribeServer.d \
./Core/Paho/Src/MQTTUnsubscribeClient.d \
./Core/Paho/Src/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Paho/Src/%.o Core/Paho/Src/%.su Core/Paho/Src/%.cyclo: ../Core/Paho/Src/%.c Core/Paho/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F207xx -c -I../Core/Inc -I"C:/Users/jyothish.tr/Documents/GitHub/GestLink/Code/Core/Paho/Inc" -I../Drivers/STM32F2xx_HAL_Driver/Inc -I../Drivers/STM32F2xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F2xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Middlewares/Third_Party/LwIP/src/include -I../Middlewares/Third_Party/LwIP/system -I../LWIP/App -I../LWIP/Target -I../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../Middlewares/Third_Party/LwIP/src/include/lwip -I../Middlewares/Third_Party/LwIP/src/include/lwip/apps -I../Middlewares/Third_Party/LwIP/src/include/lwip/priv -I../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../Middlewares/Third_Party/LwIP/src/include/netif -I../Middlewares/Third_Party/LwIP/src/include/posix -I../Middlewares/Third_Party/LwIP/src/include/posix/sys -I../Middlewares/Third_Party/LwIP/system/arch -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Paho-2f-Src

clean-Core-2f-Paho-2f-Src:
	-$(RM) ./Core/Paho/Src/MQTTConnectClient.cyclo ./Core/Paho/Src/MQTTConnectClient.d ./Core/Paho/Src/MQTTConnectClient.o ./Core/Paho/Src/MQTTConnectClient.su ./Core/Paho/Src/MQTTConnectServer.cyclo ./Core/Paho/Src/MQTTConnectServer.d ./Core/Paho/Src/MQTTConnectServer.o ./Core/Paho/Src/MQTTConnectServer.su ./Core/Paho/Src/MQTTDeserializePublish.cyclo ./Core/Paho/Src/MQTTDeserializePublish.d ./Core/Paho/Src/MQTTDeserializePublish.o ./Core/Paho/Src/MQTTDeserializePublish.su ./Core/Paho/Src/MQTTFormat.cyclo ./Core/Paho/Src/MQTTFormat.d ./Core/Paho/Src/MQTTFormat.o ./Core/Paho/Src/MQTTFormat.su ./Core/Paho/Src/MQTTPacket.cyclo ./Core/Paho/Src/MQTTPacket.d ./Core/Paho/Src/MQTTPacket.o ./Core/Paho/Src/MQTTPacket.su ./Core/Paho/Src/MQTTSerializePublish.cyclo ./Core/Paho/Src/MQTTSerializePublish.d ./Core/Paho/Src/MQTTSerializePublish.o ./Core/Paho/Src/MQTTSerializePublish.su ./Core/Paho/Src/MQTTSubscribeClient.cyclo ./Core/Paho/Src/MQTTSubscribeClient.d ./Core/Paho/Src/MQTTSubscribeClient.o ./Core/Paho/Src/MQTTSubscribeClient.su ./Core/Paho/Src/MQTTSubscribeServer.cyclo ./Core/Paho/Src/MQTTSubscribeServer.d ./Core/Paho/Src/MQTTSubscribeServer.o ./Core/Paho/Src/MQTTSubscribeServer.su ./Core/Paho/Src/MQTTUnsubscribeClient.cyclo ./Core/Paho/Src/MQTTUnsubscribeClient.d ./Core/Paho/Src/MQTTUnsubscribeClient.o ./Core/Paho/Src/MQTTUnsubscribeClient.su ./Core/Paho/Src/MQTTUnsubscribeServer.cyclo ./Core/Paho/Src/MQTTUnsubscribeServer.d ./Core/Paho/Src/MQTTUnsubscribeServer.o ./Core/Paho/Src/MQTTUnsubscribeServer.su

.PHONY: clean-Core-2f-Paho-2f-Src

