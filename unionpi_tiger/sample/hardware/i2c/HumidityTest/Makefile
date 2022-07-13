TOPDIR := ..
SRCDIR := .
TARGET := test

LDFLAGS := -L$(TARGET_DIR)/usr/lib 
#LDFLAGS += -lavutil -lavformat -lavcodec -lamcodec \
#				-lamplayer -lamadec -lasound -lamstreaming -lamavutils
#LDFLAGS += -lwfd -lwifiplayer -lfound -lutils -lstagefright -lcorkscrew 
LDFLAGS += -L$(TARGET_DIR)/lib -lm -ldl -lrt -lpthread

-include Makefile.rule

.PHONY : all clean

all :
	@ echo MAKE $(TARGET) START
	$(CXX) $(CPPFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
	@ echo MAKE $(TARGET) DONE
clean:
	@ rm -rf *.o
	@ rm -rf $(TARGET)
	@ echo CLEAN DONE



