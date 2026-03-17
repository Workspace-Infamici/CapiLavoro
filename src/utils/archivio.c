#include "archivio.h"

#include <stdio.h>

void print_record(const Record *r) {
    printf("Matricola: %s | Nome: %s | Cognome: %s | Stipendio: %.2f | Classe: %s\n",
           r->matricola, r->nome, r->cognome, r->stipendio, r->classe);
}
