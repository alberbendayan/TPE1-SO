# Compilador y flags
CC = gcc
CFLAGS = -Wall -fsanitize=address -g

# Archivos fuente para cada programa
APP_SRCS = main.c sharedMemory.c
CHILD_SRCS = child.c
VIEW_SRCS = view.c sharedMemory.c

# Objetos generados para cada programa
APP_OBJS = $(APP_SRCS:.c=.o)
CHILD_OBJS = $(CHILD_SRCS:.c=.o)
VIEW_OBJS = $(VIEW_SRCS:.c=.o)

# Nombres de los programas finales
APP_TARGET = app
CHILD_TARGET = child
VIEW_TARGET = view

all: $(APP_TARGET) $(CHILD_TARGET) $(VIEW_TARGET)

$(APP_TARGET): $(APP_OBJS)
	$(CC) $(CFLAGS) $(APP_OBJS) -o $(APP_TARGET)

$(CHILD_TARGET): $(CHILD_OBJS)
	$(CC) $(CFLAGS) $(CHILD_OBJS) -o $(CHILD_TARGET)

$(VIEW_TARGET): $(VIEW_OBJS)
	$(CC) $(CFLAGS) $(VIEW_OBJS) -o $(VIEW_TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(APP_OBJS) $(CHILD_OBJS) $(VIEW_OBJS) $(APP_TARGET) $(CHILD_TARGET) $(VIEW_TARGET)