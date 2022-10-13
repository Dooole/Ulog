INSTALL_DIR ?= /usr/bin/

CP ?= cp -rf
RM ?= rm -f
CHMOD ?= chmod 755

CFLAGS += -I../lib -Wall -D_DEFAULT_SOURCE -D_GNU_SOURCE
LDFLAGS += -L../lib -lulog

TARGET := ulog
CLIENT1 := client1
CLIENT2 := client2

all: $(TARGET) $(CLIENT1) $(CLIENT2)

$(TARGET): ulog.o
	$(CC) $^ $(LDFLAGS) -o $@

$(CLIENT1): client1.o
	$(CC) $^ $(LDFLAGS) -o $@

$(CLIENT2): client2.o
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(CLIENT1) $(CLIENT2) ./*.o

install:
	$(CHMOD) $(TARGET) $(CLIENT1) $(CLIENT2)
	$(CP) $(TARGET) $(CLIENT1) $(CLIENT2) $(INSTALL_DIR)

uninstall:
	$(RM) $(INSTALL_DIR)/$(TARGET)
	$(RM) $(INSTALL_DIR)/$(CLIENT1)
	$(RM) $(INSTALL_DIR)/$(CLIENT2)
