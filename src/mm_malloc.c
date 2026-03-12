#include <unistd.h> // Para sbrk
#include "mm_malloc.h"

// Inicio de la lista enlazada del heap
void *base = NULL;

void *my_malloc(size_t size) {
    // TODO: Implementar First-Fit o Best-Fit
    // 1. Verificar si hay un bloque libre del tamaño adecuado.
    // 2. Si no, pedir espacio al OS con sbrk().

    block_meta *current = (block_meta *)base;
    while (current != NULL)
    {
        if (current->free && (current->size >= size))
        {
            // Encontramos un bloque libre adecuado
            current->free = 0; // Marcar como ocupado
            return (void *)(current + 1); // Retornar el espacio después de la metadata
        }
        current = current->next; // Avanzar al siguiente bloque
    }

    // Ubicar inicio del nuevo bloque de memoria y solicitarlo al sistema operativo
    void *start_ptr = sbrk(0);
    if (sbrk(size + META_SIZE) == (void *)-1)
    {
        perror("Error al asignar memoria");
        return NULL;
    }

    // Apuntador al inicio del bloque a retornar
    void *ptr = start_ptr + META_SIZE;

    // Crear metadata del bloque y almacenarla al inicio del bloque asignado
    block_meta meta = {size, base, 0, 42};
    base = start_ptr;
    *(block_meta *)start_ptr = meta;
    return ptr;
}

void my_free(void *ptr) {
    // Obtener metadata
    block_meta *meta = (block_meta *)(ptr - META_SIZE);

    // TODO: Marcar el bloque como libre.
    meta->free = 1;

    // TODO: Fusionar bloques adyacentes (Coalescing).
    block_meta *current = (block_meta *)base;
    if ((meta->next != NULL) && meta->next->free)
    {
        meta->size += meta->next->size;
        meta->next = meta->next->next;
    }

    while (current != NULL)
    {
        if (current->next == meta)
        {
            if(current->free)
            {
                current->size += meta->size;
                current->next = meta->next;
            }
            break;
        }
        current = current->next; // Avanzar al siguiente bloque
    }
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    return NULL;
}

void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.
    return NULL;
}
