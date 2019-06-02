# avr-gcc makefile

# Typ AVRa
#MCU = atmega16

# Czêstotliwoœæ zegara
#F_CPU = 1000000

# Typ programatora
#AVRDUDE_PROGRAMMER = usbasp 
#AVRDUDE_PORT= usb

# Nazwa wynikowego pliku hex
PROJECT = main

# Pliki Ÿród³owe
SOURCES = $(PROJECT).c $(BOARD_DIR)/functions.c program.c $(BOARD_DIR)/uart.c variables.c crc.c hdlc.c spi.c winbond.c

CC = avr-gcc
OBJCOPY = avr-objcopy

CDEFS = -DF_CPU=$(F_CPU)UL -I$(BOARD_DIR) -I.

CFLAGS = $(CDEFS) 
CFLAGS += -mmcu=$(MCU) -g -Os -Wall

OBJECTS = $(SOURCES:.c=.o)

$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -O ihex -R .eeprom $(PROJECT).out $(PROJECT).hex

$(PROJECT).out:  $(OBJECTS)
	$(CC)  $(CFLAGS) $^ -o $@ -lm

$.o:%.c
	$(CC) $(CFLAGS) $< -o $@

program_flash: $(PROJECT).hex
	avrdude -c $(AVRDUDE_PROGRAMMER) -P $(AVRDUDE_PORT) -p $(MCU) -b $(AVRDUDE_BAUD) -U flash:w:$(PROJECT).hex 

clean:
	rm -f $(OBJECTS) 
	rm -f $(PROJECT).out
	rm -f $(PROJECT).hex

.PHONY: clean program_flash
