MCU=attiny10
F_CPU=1000000L
CC=avr-gcc
OBJCOPY=avr-objcopy
SIZE=avr-size
AVRDUDE=avrdude
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I. 
TARGET=main
SRCS = main.c 
PROGRAMMER=usbasp
PORT=usb
BAUD=57600

all:
	${CC} ${CFLAGS} -o ${TARGET}.o ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.o ${TARGET}.hex
	${SIZE} -C --mcu=${MCU} ${TARGET}.o

flash:
	${AVRDUDE} -p${MCU} -P${PORT} -b${BAUD} ${PREVENT_ERASE} -c${PROGRAMMER} -Uflash:w:${TARGET}.hex:i

info:
	${AVRDUDE} -p${MCU} -c${PROGRAMMER}

clean:
	rm -f *.c~ *.h~ *.o *.hex

