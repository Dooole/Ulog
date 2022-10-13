.PHONY: all clean install

all:
	make -C ./lib
	make -C ./src

clean:
	make -C ./lib clean
	make -C ./src clean

install:
	make -C ./lib install
	make -C ./src install
