#include "archivio.h"
#include "../utils/archivio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *ARCHIVE_PATH = "src/commons/Archivio.dat";         // aggiornato path perche' mancava src/
static const char *ARCHIVE_TMP_PATH = "src/commons/Archivio_tmp.dat"; // aggiornato path perche' mancava src/

#define MATRICOLA_MIN 10000
#define MATRICOLA_MAX 99999


// Funzione per assicurarsi che il generatore di numeri casuali sia inizializzato solo una volta
static void ensure_rng_seeded(void) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
}

// Funzione per verificare se una matricola esiste già nell'archivio
static int record_exists(FILE *fp, const char *matricola) {
    Record r;

    fseek(fp, sizeof(Header), SEEK_SET);
    clearerr(fp);
    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (strcmp(r.matricola, matricola) == 0) {
            return 1;
        }
    }
    return 0;
}

int archivio_add(Record *r) {
    Record to_write;
    Header h = {0, 0}; // Inizializzo un header a zero per sicurezza (Parametri: record_totali e record_cancellati)

    FILE *fp = fopen(ARCHIVE_PATH, "rb+"); // Provo ad aprire in modalità lettura/scrittura
    if (!fp) {
        // Se rb+ fallisce, il file non esiste, quindi lo creo e lo apro in modalità scrittura/lettura (wb+)
        fp = fopen(ARCHIVE_PATH, "wb+");
        if (!fp) {
            return -1;
        }
        fwrite(&h, sizeof(Header), 1, fp); // essendo un file nuovo, scrivo l'header iniziale con valori a zero
    } else {
        // Se il file esiste già, leggo l'header per avere i contatori aggiornati
        if (fread(&h, sizeof(Header), 1, fp) != 1) {
            // Reset a 0 se la lettura dell'header fallisce
            h.record_totali = 0;
            h.record_cancellati = 0;
        }
    }

    if (!r) {
        fclose(fp);
        return -1;
    }
    ensure_rng_seeded();
    for (;;) {
        char candidate[MATRICOLA_LEN];
        int number = MATRICOLA_MIN + (rand() % (MATRICOLA_MAX - MATRICOLA_MIN + 1));
        snprintf(candidate, sizeof(candidate), "stu%d", number);
        if (!record_exists(fp, candidate)) {
            strncpy(r->matricola, candidate, sizeof(r->matricola));
            r->matricola[sizeof(r->matricola) - 1] = '\0';
            break;
        }
    }

    to_write = *r;
    to_write.cancellato = 0;

    fseek(fp, 0, SEEK_END); // Posiziono il puntatore alla fine del file per aggiungere il nuovo record
    if (fwrite(&to_write, sizeof(Record), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    h.record_totali++; // Incremento il contatore dei record totali (inclusi quelli cancellati)
    fseek(fp, 0, SEEK_SET); // Torno all'inizio del file per aggiornare l'header
    fwrite(&h, sizeof(Header), 1, fp); // Aggiorno l'header con i nuovi contatori

    fclose(fp);
    return 1;
}

int archivio_read_all(int is_admin) {
    Header h;
    Record r;
    int count = 0;
    int percentuale_cancellati = 0;

    FILE *fp = fopen(ARCHIVE_PATH, "rb");
    if (!fp) {
        return -1;
    }

    // Leggo l'header per ottenere i contatori aggiornati. Se la lettura fallisce, inizializzo i contatori a zero
    if (fread(&h, sizeof(Header), 1, fp) != 1) {
        h.record_totali = 0;
        h.record_cancellati = 0;
    }

    if (is_admin == 1) {
        // Calcolo la percentuale di record cancellati (logicamente) rispetto al totale, evitando la divisione per zero
        if (h.record_totali > 0) {
            percentuale_cancellati = (h.record_cancellati * 100) / h.record_totali;
        }
        printf("---- Statistiche ----\n");
        printf("Totale record: %u (Cancellati: %u) | Percentuale record cancellati: %d%%\n\n", h.record_totali, h.record_cancellati, percentuale_cancellati);
    }

    printf("---- Archivio ----\n");
    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (r.cancellato != 0) {
            printf("[CANCELLATO] ");
        }
        print_record(&r);
        count++;
    }
    fclose(fp);
    return count;
}

int archivio_update(const char *matricola, const Record *nuovo) {
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    if (!fp) {
        return -1;
    }

    // Salto l'header
    fseek(fp, sizeof(Header), SEEK_SET);

    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (strcmp(r.matricola, matricola) == 0 && r.cancellato == 0) {
            if (!nuovo) {
                fclose(fp);
                return -1;
            }
            r = *nuovo;
            strncpy(r.matricola, matricola, sizeof(r.matricola));
            r.matricola[sizeof(r.matricola) - 1] = '\0';
            r.cancellato = 0;

            if (fseek(fp, -(long)sizeof(Record), SEEK_CUR) != 0) {
                fclose(fp);
                return -1;
            }
            if (fwrite(&r, sizeof(Record), 1, fp) != 1) {
                fclose(fp);
                return -1;
            }
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

// Da modificare (trasformare in compattazione?)
int archivio_delete_physical(const char *matricola) {
    int found = 0;
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "rb");
    FILE *tmp = NULL;

    if (!fp) {
        return -1;
    }

    tmp = fopen(ARCHIVE_TMP_PATH, "wb");
    if (!tmp) {
        fclose(fp);
        return -1;
    }

    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (strcmp(r.matricola, matricola) == 0) {
            found = 1;
            continue;
        }
        fwrite(&r, sizeof(Record), 1, tmp);
    }

    fclose(fp);
    fclose(tmp);

    if (!found) {
        remove(ARCHIVE_TMP_PATH);
        return 0;
    }

    remove(ARCHIVE_PATH);
    // rename rinomina il tmp path in quello che nel programma viene usato. Con lo 0 va tutto bene,
    // mentre con un valore diverso, e' un casino
    if (rename(ARCHIVE_TMP_PATH, ARCHIVE_PATH) != 0) {
        return -1;
    }

    return 1;
}

int archivio_delete_logical(const char *matricola) {
    Header h;
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    // controllo di sicurezza per apertura del file (-1 rappresenta un errore)
    if (!fp) {
        return -1;
    }

    // Leggo l'header per ottenere i contatori aggiornati
    if (fread(&h, sizeof(Header), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        // trova il record con la matricola specificata e che non e' gia' cancellato
        if (strcmp(r.matricola, matricola) == 0 && r.cancellato == 0) {
            r.cancellato = 1; // imposta il "flag" di cancellazione logica (1 = cancellato)

            // torna indietro di una posizione per sovrascrivere sul file (disco)
            if (fseek(fp, -(long)sizeof(Record), SEEK_CUR) != 0) {
                // controllo di sicurezza per fseek, se fallisce chiude il file e restituisce -1
                fclose(fp);
                return -1;
            }
            if (fwrite(&r, sizeof(Record), 1, fp) != 1) {
                // controllo di sicurezza per fwrite, se fallisce chiude il file e restituisce -1
                fclose(fp);
                return -1;
            }

            // Aggiorna l'header incrementando il contatore dei record cancellati (logicamente)
            h.record_cancellati++;
            fseek(fp, 0, SEEK_SET); // Torno a inizio file
            fwrite(&h, sizeof(Header), 1, fp);

            fclose(fp);
            return 1; // Successo
        }
    }

    fclose(fp);
    return 0; // se il while si conclude senza restituire niente, il record non e' stato trovato o e' gia' stato cancellato
}

int archivio_restore(const char *matricola) {
    Header h;
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    if (!fp) {
        return -1;
    }

    // Leggo l'header per ottenere i contatori aggiornati
    if (fread(&h, sizeof(Header), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (strcmp(r.matricola, matricola) == 0 && r.cancellato == 1) {
            r.cancellato = 0;

            if (fseek(fp, -(long)sizeof(Record), SEEK_CUR) != 0) {
                fclose(fp);
                return -1;
            }
            if (fwrite(&r, sizeof(Record), 1, fp) != 1) {
                fclose(fp);
                return -1;
            }

            // Aggiorno l'header decrementando il contatore dei record cancellati (logicamente)
            if (h.record_cancellati > 0) h.record_cancellati--; // Decremento solo se c'e' almeno un record cancellato, per evitare di andare in negativo
            fseek(fp, 0, SEEK_SET);
            fwrite(&h, sizeof(Header), 1, fp);

            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}
