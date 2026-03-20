# ==========================================
#   OS PROJECT 1: ULA-MALLOC Makefile
# ==========================================

# Compilador y Flags
CC = gcc
# -Wall -Wextra: Activa todos los warnings
# -Werror: Trata los warnings como errores (Código sucio no compila)
# -g: Añade símbolos de debug para GDB/Valgrind
# -Iinclude: Busca headers en la carpeta include/
CFLAGS = -Wall -Wextra -g -Iinclude -std=c11 -D_DEFAULT_SOURCE

# Directorios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Archivos fuente y objetos
SRCS = $(SRC_DIR)/mm_malloc.c $(SRC_DIR)/mm_test.c
OBJS = $(OBJ_DIR)/mm_malloc.o $(OBJ_DIR)/mm_test.o

# Nombre del ejecutable final
TARGET = test_runner

# Regla por defecto (la que se ejecuta al escribir 'make')
all: $(TARGET)

# Regla para linkear el ejecutable final
$(TARGET): $(OBJS)
	@echo "🔗 Linkeando $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^
	@echo "✅ Compilación exitosa. Ejecuta ./$(TARGET) para probar."

# Regla genérica para compilar archivos .c a .o
# Crea el directorio obj/ si no existe
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "🔨 Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar archivos temporales
clean:
	@echo "🧹 Limpiando archivos de compilación..."
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "✨ Directorio limpio."

# Regla para ejecutar los tests directamente
run: all
	@echo "🚀 Ejecutando tests..."
	./$(TARGET)

# Regla para debug con Valgrind (Detecta fugas de memoria)
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

.PHONY: all clean run valgrind
