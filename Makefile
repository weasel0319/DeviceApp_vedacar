.SUFFIXES : .c .o 

OBJ1 = device_app.o makeconnection.o makethread.o serverconnection.o
SRC1 = $(OBJ1:.o=.c)
CC = aarch64-linux-gnu-gcc

LDFLAGS1 = 
INC = -I include

.PHONY: clean

all: device_app

device_app: $(OBJ1)
	$(CC) -o $@ $(OBJ1) $(LDFLAGS1)

.c.o:
	$(CC) -c $(INC) $< 

clean:
	rm *.o device_app 

dep: 
	gccmakedep $(INC) $(SRC1)