# Nombre del ejecutable
TARGET = matmulseq_file

# Archivos fuente
SRCS = matmulseq_file.c

# Compilador
CC = gcc

# Opciones de compilación
CFLAGS = -g

# Regla principal para construir el proyecto
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regla all para construir el proyecto
all: $(TARGET)

# Regla para limpiar
clean:
	rm -f $(TARGET)