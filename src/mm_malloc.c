#include <unistd.h> // Para sbrk
#include <string.h> // Para memcpy y memset
#include "mm_malloc.h"

// Inicio de la lista enlazada del heap
void *base = NULL;

void *my_malloc(size_t size) {
    // TODO: Implementar First-Fit o Best-Fit
    // Verificar si el tamaño es 0, en ese caso devolver NULL.
    if(size == 0)
        return NULL;

    // Alinear el tamaño a multiplos de 8
    size = (size + 7) & ~7;

    // 1. Verificar si hay un bloque libre del tamaño adecuado.
    block_meta *current = (block_meta *)base;
    block_meta *last = NULL;

    // 1. Buscar bloque libre
    while (current != NULL)
    {
        if (current->free && (current->size >= size))
        {
            current->free = 0;

            // Intentar fragmentar si sobra espacio suficiente para otro bloque + metadata
            if (current->size >= size + META_SIZE + 8)
            {
                block_meta *next = (block_meta *)((char *)current + META_SIZE + size);
                next->size = current->size - size - META_SIZE;
                next->next = current->next;
                next->free = 1;

                current->size = size;
                current->next = next;
            }
            return (void *)(current + 1);
        }
        last = current;
        current = current->next;
    }

    // 2. Si no, pedir espacio al OS con sbrk().
    block_meta *new_block = sbrk(0);
    if (sbrk(size + META_SIZE) == (void *)-1)
        return NULL;

    new_block->size = size;
    new_block->next = NULL;
    new_block->free = 0;

    if (base == NULL)
    {
        base = new_block;
    }
    else if (last != NULL)
    {
        last->next = new_block;
    }

    return (void *)(new_block + 1);
}

void my_free(void *ptr) {
    if (ptr == NULL)
        return;

    // Obtener metadata
    block_meta *meta = (block_meta *)(ptr - META_SIZE);

    // TODO: Marcar el bloque como libre.
    meta->free = 1;

    // TODO: Fusionar bloques adyacentes (Coalescing).
    block_meta *current = (block_meta *)base;
    // Fusionar siguiente bloque
    if ((meta->next != NULL) && meta->next->free)
    {
        meta->size += meta->next->size + META_SIZE;
        meta->next = meta->next->next;
    }

    // Fusionar bloque anterior
    while (current != NULL)
    {
        if (current->next == meta)
        {
            if(current->free)
            {
                current->size += meta->size + META_SIZE;
                current->next = meta->next;
            }
            break;
        }
        current = current->next; // Avanzar al siguiente bloque
    }
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    void *res = my_malloc(nmemb * size);

    // Verificar si my_malloc devolvió NULL antes de llamar a memset
    if (res == NULL)
        return NULL;

    memset(res, 0, nmemb * size);
    return res;
}

void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.

    if (ptr == NULL)
        return my_malloc(size);

    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    // Alinear el tamaño a multiplos de 8
    size = (size + 7) & ~7;

    // Obtener metadata
    block_meta *meta = (block_meta *)(ptr - META_SIZE);
    if(meta->size == size)
        return ptr;

    if(meta->size > size) {
        // Intentar fragmentar si sobra espacio suficiente para otro bloque + metadata
        if (meta->size >= size + META_SIZE + 8)
        {
            block_meta *next = (block_meta *)((char *)meta + META_SIZE + size);
            next->size = meta->size - size - META_SIZE;
            next->next = meta->next;
            next->free = 1;
            meta->next = next;
        }
        meta->size = size;
        return ptr;
    }

    void *new_ptr = my_malloc(size);
    // Verificar si my_malloc devolvió NULL antes de llamar a memset
    if (new_ptr == NULL)
        return NULL;

    memcpy(new_ptr, ptr, meta->size);
    my_free(ptr);
    return new_ptr;
}
