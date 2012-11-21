CFLAGS=-O2 -Wall
CC=$(CROSS_COMPILE)gcc
BIN=record replay

default: $(BIN)

install_android: $(BIN)
	adb remount
	adb push $(BIN) /system/bin/

clean:
	rm -f $(BIN)

