#ifndef BRANZINO_ARCHIVIO_H
#define BRANZINO_ARCHIVIO_H
#include <stdint.h> // per uint32_t

#define NAME_LEN 32
#define SURNAME_LEN 32
#define CLASS_LEN 4
#define MATRICOLA_LEN 16

// Struttura per rappresentare un record nell'archivio
typedef struct {
    char matricola[MATRICOLA_LEN];
    int cancellato; // 0 = non cancellato, 1 = cancellato (nuovo campo per cancellazione logica)
    char nome[NAME_LEN];
    char cognome[SURNAME_LEN];
    float stipendio;
    char classe[CLASS_LEN];
} Record;

// Struttura per l'header del file, che contiene informazioni sul numero totale di record e su quelli cancellati
typedef struct {
    // il tipo uint32_t e' un intero senza segno a 32 bit garantisce la dimensione fissa su piu' sistemi operativi
    uint32_t record_totali;     // Numero totale di record (inclusi quelli cancellati)
    uint32_t record_cancellati; // Numero di record cancellati (logicamente)
} Header;

int archivio_add(Record *r);
int archivio_read_all(int is_admin);
int archivio_update(const char *matricola, const Record *nuovo);
int archivio_delete_physical(const char *matricola);
int archivio_delete_logical(const char *matricola); // Funzione per cancellazione logica
int archivio_restore(const char *matricola);        // Funzione per ripristinare un record cancellato


#endif