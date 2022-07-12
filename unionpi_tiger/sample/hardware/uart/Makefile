TOPDIR := ..
SRCDIR := .
TARGET := uart_test

LDFLAGS := -L$(TARGET_DIR)/usr/lib 
#LDFLAGS += -lavutil -lavformat -lavcodec -lamcodec \
#				-lamplayer -lamadec -lasound -lamstreaming -lamavutils
#LDFLAGS += -lwfd -lwifiplayer -lfound -lutils -lstagefright -lcorkscrew 
LDFLAGS += -L$(TARGET_DIR)/lib -lm -ldl -lrt -lpthread

-include Makefile.rule

.PHONY : all clean

all : $(TARGET)

clean:
	@ rm -rf *.o
	@ rm -rf uart_test
	@ echo CLEAN DONE
install:
	-install -m 777 ${TARGET} $(INSTALL_STAG)
	cp -dp $(INSTALL_STAG)/${TARGET} $(INSTALL_PREFIX)
	
$(TARGET): $(OBJS)
	@ echo MAKE $(TARGET) START
	$(CXX) $(CPPFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
	@ echo MAKE $(TARGET) DONE

