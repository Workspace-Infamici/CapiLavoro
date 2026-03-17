#ifndef BRANZINO_ARCHIVIO_H
#define BRANZINO_ARCHIVIO_H

#define NAME_LEN 32
#define SURNAME_LEN 32
#define CLASS_LEN 4
#define MATRICOLA_LEN 16

typedef struct {
    char matricola[MATRICOLA_LEN];
    int cancellato; // 0 = non cancellato, 1 = cancellato (nuovo campo per cancellazione logica)
    char nome[NAME_LEN];
    char cognome[SURNAME_LEN];
    float stipendio;
    char classe[CLASS_LEN];
} Record;

int archivio_add(Record *r);
int archivio_read_all(void);
int archivio_update(const char *matricola, const Record *nuovo);
int archivio_delete_physical(const char *matricola);
int archivio_delete_logical(const char *matricola); // Funzione per cancellazione logica
int archivio_restore(const char *matricola);        // Funzione per ripristinare un record cancellato

#endif
