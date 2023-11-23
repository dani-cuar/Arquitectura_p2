# Nombre del ejecutable
TARGET = coarse_grain_version

# Archivos fuente
SRCS = coarse_grain_version.c

# Compilador
CC = gcc

# Opciones de compilación
CFLAGS = -g

# Regla principal para construir el proyecto (depende de SRCS)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regla all para construir el proyecto
all: $(TARGET)

# Regla para limpiar
clean:
	rm -f $(TARGET)

