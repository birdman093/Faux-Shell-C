CC 			=	gcc -std=gnu99
CFLAGS 		=	-g -Wall
LDFlags 	=
OBJFILES 	=	Main.o DollarSign.o UserCommand.h
TARGET 		=	smallssh

all: $(TARGET)

$(TARGET) : $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILEs) $(TARGET) *~