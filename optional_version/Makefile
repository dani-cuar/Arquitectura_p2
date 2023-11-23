# Nombre de la biblioteca compartida
TARGET = libmatrixmul.so

# Nombre del archivo fuente
SRC = optional_version.c

# Opciones del compilador
CFLAGS = -fPIC -shared

# Regla predeterminada
all: $(TARGET)

# Regla para construir la biblioteca compartida
$(TARGET):
	gcc $(CFLAGS) -o $(TARGET) $(SRC)

# Regla para limpiar archivos compilados
clean:
	rm -f $(TARGET)
