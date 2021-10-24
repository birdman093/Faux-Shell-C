CC 			=	gcc -std=gnu99
CFLAGS 		=	-g -Wall
LDFlags 	=
OBJFILES 	=	Main.o DollarSign.o Exit.o Status.o ChangeDirectory.o ExecComm.o Redirection.o CheckBgTerm.o UserCommand.h BGProcesses.h
TARGET 		=	smallsh

all: $(TARGET)

$(TARGET) : $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILEs) $(TARGET) *~