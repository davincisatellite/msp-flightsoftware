include $(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

SOURCES := $(wildcard ../src/*.c)
SOURCES += $(wildcard ../src/*.cpp)
SOURCES := $(filter-out ../src/main.%, $(SOURCES))

MAIN := $(wildcard ../src/main.cpp)
ifeq ($(MAIN),)
	MAIN := $(wildcard ../src/main.c)
endif
MAIN := $(patsubst %.c, %.obj, $(patsubst %.cpp, %.obj, $(notdir $(MAIN))))

OBJECTS := $(patsubst %.c, %.obj, $(patsubst %.cpp, %.obj, $(notdir $(SOURCES))))
OBJECTS += gcc_startup_msp432p401r_gcc.obj

NAME = msp-flightsoftware

CFLAGS = -I../src \
	"-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source" \
	"-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
	"-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/devices/msp432p4xx/driverlib" \
	-D__MSP432P401R__ \
	-DDeviceFamily_MSP432P401x \
	-mcpu=cortex-m4 \
	-march=armv7e-m \
	-mthumb \
	-std=c99 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-ffunction-sections \
	-fdata-sections \
	-g \
	-gstrict-dwarf \
	-Wall \
	"-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
	"-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -Wl,-T,../gcc/msp432p401r.lds \
	"-Wl,-Map,$(NAME).map" \
	"-L$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source" \
	-l:ti/display/lib/display.am4fg \
	-l:ti/grlib/lib/gcc/m4f/grlib.a \
	-l:third_party/spiffs/lib/gcc/m4f/spiffs.a \
	-l:ti/drivers/lib/drivers_msp432p401x.am4fg \
	-l:third_party/fatfs/lib/gcc/m4f/fatfs.a \
	-l:ti/devices/msp432p4xx/driverlib/gcc/msp432p4xx_driverlib.a \
	-march=armv7e-m \
	-mthumb \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-static \
	-Wl,--gc-sections \
	"-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard" \
	-lgcc \
	-lc \
	-lstdc++ \
	-lm \
	-lnosys \
	--specs=nano.specs

all: $(NAME).out

gcc_startup_msp432p401r_gcc.obj: ../gcc/startup_msp432p401r_gcc.c
	@ echo Building $@ from $^
	@ $(CC) $(CFLAGS) $< -c -o $@

hashes:
	@ rm -f ../src/hashes.h
	@ scripts/generate-hashes.sh ../src/hashes.h ../src/*

gitinfo:
	@ rm -f ../src/gitinfo.h
	@ scripts/generate-gitinfo.sh ../src/gitinfo.h

%.obj: ../src/%.c
	@ echo Building $@ from $^
	@ $(CC) $(CFLAGS) $< -c -o $@

%.obj: ../src/%.cpp
	@ echo Building $@ from $^
	@ $(CC) $(CFLAGS) $< -c -o $@

%.obj: ../test/%.cpp
	@ echo Building test $@ from $^
	@ $(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(MAIN) $(OBJECTS)
	@ echo Linking $@ with $^
	@ $(LNK) $^ $(LFLAGS) -o $(NAME).out

test/%: %.obj $(OBJECTS) DWire_test_examples.obj
	@ echo Linking test $@ with $^
	@ $(CC) $^ $(LFLAGS) -o $(notdir $@).out

clean:
	@ echo Cleaning...
	@ $(RM) *.obj > $(DEVNULL) 2>&1
	@ $(RM) *.out > $(DEVNULL) 2>&1
	@ $(RM) *.map > $(DEVNULL) 2>&1