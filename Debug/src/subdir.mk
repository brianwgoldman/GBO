################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Configuration.cpp \
../src/Crossover.cpp \
../src/Evaluation.cpp \
../src/HillClimb.cpp \
../src/Util.cpp \
../src/main.cpp 

OBJS += \
./src/Configuration.o \
./src/Crossover.o \
./src/Evaluation.o \
./src/HillClimb.o \
./src/Util.o \
./src/main.o 

CPP_DEPS += \
./src/Configuration.d \
./src/Crossover.d \
./src/Evaluation.d \
./src/HillClimb.d \
./src/Util.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++11 -O0 -g3 -pg -pedantic -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


