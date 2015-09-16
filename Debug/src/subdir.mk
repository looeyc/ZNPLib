################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/znpapi_af.c \
../src/znpapi_common.c \
../src/znpapi_config.c \
../src/znpapi_simple.c \
../src/znpapi_sys.c \
../src/znpapi_zdo.c \
../src/znpcom.c \
../src/znpcomhal.c \
../src/znpframe.c \
../src/znpinit.c \
../src/znpmsg.c \
../src/znpmsgfifo.c 

OBJS += \
./src/znpapi_af.o \
./src/znpapi_common.o \
./src/znpapi_config.o \
./src/znpapi_simple.o \
./src/znpapi_sys.o \
./src/znpapi_zdo.o \
./src/znpcom.o \
./src/znpcomhal.o \
./src/znpframe.o \
./src/znpinit.o \
./src/znpmsg.o \
./src/znpmsgfifo.o 

C_DEPS += \
./src/znpapi_af.d \
./src/znpapi_common.d \
./src/znpapi_config.d \
./src/znpapi_simple.d \
./src/znpapi_sys.d \
./src/znpapi_zdo.d \
./src/znpcom.d \
./src/znpcomhal.d \
./src/znpframe.d \
./src/znpinit.d \
./src/znpmsg.d \
./src/znpmsgfifo.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/chengm/myLinux/eclipse_prjs/ZNPLib/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


