.PHONY: all clean install

all:
	$(MAKE) -C ./lib
	$(MAKE) -C ./src

clean:
	$(MAKE) -C ./lib clean
	$(MAKE) -C ./src clean

install:
	$(MAKE) -C ./lib install
	$(MAKE) -C ./src install

uninstall:
	$(MAKE) -C ./lib uninstall
	$(MAKE) -C ./src uninstall