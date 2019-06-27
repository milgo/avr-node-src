# avr-gcc makefile

# Typ AVRa
#MCU = atmega16

# Czêstotliwoœæ zegara
#F_CPU = 1000000

# Typ programatora
#AVRDUDE_PROGRAMMER = usbasp 
#AVRDUDE_PORT= usb

# Nazwa wynikowego pliku hex
TARGET = main

# Pliki Ÿród³owe
SOURCES = $(TARGET).c $(BOARD_DIR)/functions.c program.c $(BOARD_DIR)/uart.c variables.c crc.c hdlc.c spi.c winbond.c

CC = avr-gcc
OBJCOPY = avr-objcopy

CDEFS = -DF_CPU=$(F_CPU)UL -I$(BOARD_DIR) -I.

CFLAGS = $(CDEFS) 
CFLAGS += -mmcu=$(MCU) -g -Os -Wall

OBJECTS = $(SOURCES:.c=.o)
SIZE = avr-size

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex
	$(SIZE) --totals $(TARGET).elf

$(TARGET).elf:  $(OBJECTS)
	$(CC)  $(CFLAGS) $^ -o $@ -lm

$.o:%.c
	$(CC) $(CFLAGS) $< -o $@

program_flash: $(TARGET).hex
	avrdude -c $(AVRDUDE_PROGRAMMER) -P $(AVRDUDE_PORT) -p $(MCU) -b $(AVRDUDE_BAUD) -U flash:w:$(TARGET).hex 

clean:
	rm -f $(OBJECTS) 
	rm -f $(TARGET).elf
	rm -f $(TARGET).hex

.PHONY: clean program_flash
