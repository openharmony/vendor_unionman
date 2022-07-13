TOPDIR := ..
SRCDIR := .

MAKE = make
CXX = aarch64-linux-gnu-gcc
CPPFLAGS += -I$(TOPDIR)/include
RM_F := rm

CPPFLAGS += -Wall

CPPFLAGS += -g

LDFLAGS := -L. -lm -ldl -lrt -lpthread

-include Makefile.rule

.PHONY : all clean

all :
	$(CXX) $(CPPFLAGS) canconfig.c libsocketcan.c -o canconfig $(LDFLAGS)
	$(CXX) $(CPPFLAGS) candump.c -o candump $(LDFLAGS)
	$(CXX) $(CPPFLAGS) canecho.c -o canecho $(LDFLAGS)
	$(CXX) $(CPPFLAGS) cansend.c -o cansend $(LDFLAGS)
	$(CXX) $(CPPFLAGS) cansequence.c -o cansequence $(LDFLAGS)
clean:
	@$(RM_F) -rf *.o
	@$(RM_F) -rf canconfig candump canecho cansend cansequence
	@ echo CLEAN DONE



