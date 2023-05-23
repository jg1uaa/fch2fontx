CC = cc

TARGET = fch2fontx
OBJ = memio.o parse.o load.o main.o convert.o
CFLAGS = -O2 -Wall

all:	$(TARGET)

$(TARGET):	$(OBJ)
	$(CC) -o $(TARGET) $(OBJ)

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(OBJ) $(TARGET)
