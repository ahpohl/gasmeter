# DEVICE ....... The AVR device you compile for
# CLOCK ........ Target AVR clock rate in Hertz
# OBJECTS ...... The object files created from your source files. This list is
#                usually the same as the list of source files with suffix ".o".
# PROGRAMMER ... Options to avrdude which define the hardware you use for
#                uploading to the AVR and the interface where this hardware
#                is connected.
# FUSES ........ Parameters for avrdude to flash the fuses appropriately.

AVRDUDE    = /usr/bin/avrdude
DEVICE     = atmega328p
CLOCK      = 12000000UL
PROGRAMMER = -c buspirate -P /dev/ttyUSB1
UPLOAD     = -c arduino -P /dev/ttyUSB0
FUSES      = -U lfuse:w:0xE0:m -U hfuse:w:0xDC:m -U efuse:w:0xFD:m
LOCK       = -U lock:w:0x0F:m
UNLOCK     = -U lock:w:0x3F:m
BOOTLOADER = -U flash:w:bootloader/optiboot_atmega328.hex:i
OBJCOPY    = /usr/bin/avr-objcopy

######################################################################

# define the C compiler to use
CC = avr-gcc

# define any compile-time flags
CFLAGS = -Wall -g -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

# define any directories containing header files other than /usr/include
INCLUDES = -I./include

# define library paths in addition to /usr/lib
LFLAGS =

# define any libraries to link into executable:
LIBS =

# define src and obj directories
SRC_DIR = src

# define build directory
OBJ_DIR = build

# define the C source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# define the C object files 
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# define the executable file 
MAIN = pulse

###############
### targets ###
###############

.PHONY: build clean install

all: build $(MAIN)

build:
	-@ mkdir -p $(OBJ_DIR)

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ_DIR)/$(MAIN).elf $(OBJS) $(LFLAGS) $(LIBS)
	$(OBJCOPY) -j .text -j .data -O ihex $(OBJ_DIR)/$(MAIN).elf $(OBJ_DIR)/$(MAIN).hex

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

fuses:
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) $(FUSES)

lock:
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) $(LOCK)

unlock:
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) $(UNLOCK)

bootloader: fuses
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) $(UNLOCK) $(BOOTLOADER) $(LOCK)

flash: all fuses
	$(AVRDUDE) $(PROGRAMMER) -p $(DEVICE) -U flash:w:$(OBJ_DIR)/$(MAIN).hex:i

upload: all
	$(AVRDUDE) $(UPLOAD) -p $(DEVICE) -U flash:w:$(OBJ_DIR)/$(MAIN).hex:i

clean:
	-@ $(RM) $(OBJS) $(OBJ_DIR)/$(MAIN).elf $(OBJ_DIR)/$(MAIN).hex *~

