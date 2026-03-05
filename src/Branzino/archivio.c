#include "archivio.h"
#include "../utils/archivio.h"

#include <stdio.h>

static const char *ARCHIVE_PATH = "src/commons/Archivio.dat";           // aggiornato path perche' mancava src/
static const char *ARCHIVE_TMP_PATH = "src/commons/Archivio_tmp.dat";   // aggiornato path perche' mancava src/

static int record_exists(FILE *fp, int matricola) {
    Record r;

    fseek(fp, 0, SEEK_SET);
    clearerr(fp);
    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (r.matricola == matricola) {
            return 1;
        }
    }
    return 0;
}

int archivio_add(const Record *r) {
    FILE *fp = fopen(ARCHIVE_PATH, "ab+");
    if (!fp) {
        return -1;
    }
    if (!r) {
        fclose(fp);
        return -1;
    }
    if (record_exists(fp, r->matricola)) {
        fclose(fp);
        return 0;
    }
    if (fwrite(r, sizeof(Record), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 1;
}

int archivio_read_all() {
    Record r;
    int count = 0;
    FILE *fp = fopen(ARCHIVE_PATH, "rb");
    if (!fp) {
        return -1;
    }

    printf("---- Archivio ----\n");
    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        print_record(&r);
        count++;
    }
    fclose(fp);
    return count;
}

int archivio_update(int matricola, const Record *nuovo) {
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    if (!fp) {
        return -1;
    }

    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        if (r.matricola == matricola) {
            if (!nuovo) {
                fclose(fp);
                return -1;
            }
            r = *nuovo;
            r.matricola = matricola;

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

int archivio_delete_physical(int matricola) {
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
        if (r.matricola == matricola) {
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
    //rename rinomina il tmp path in quello che nel programma viene usato. Con lo 0 va tutto bien, mentre con un valore diverso, è un casino
    if (rename(ARCHIVE_TMP_PATH, ARCHIVE_PATH) != 0) {
        return -1;
    }

    return 1;
}

// funzione per cancellazione logica di un record
int archivio_delete_logical(int matricola) {
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    // controllo di sicurezza per apertura del file (-1 rappresenta un errore)
    if (!fp) {
        return -1;
    }

    while (fread(&r, sizeof(Record), 1, fp) == 1) {
        // trova il record con la matricola specificata e che non è già cancellato
        if (r.matricola == matricola && r.cancellato == 0) {
            r.cancellato = 1; // imposta il "flag" di cancellazione logica (1 = cancellato)
            
            // torna indietro di una posizione per sovrascrivere sul file (disco)
            if (fseek(fp, -(long)sizeof(Record), SEEK_CUR) != 0) { // controllo di sicurezza per fseek, se fallisce, chiude il file e restituisce -1
                fclose(fp);
                return -1;
            }
            if (fwrite(&r, sizeof(Record), 1, fp) != 1) { // controllo di sicurezza per fwrite, se fallisce, chiude il file e restituisce -1
                fclose(fp);
                return -1;
            }
            fclose(fp);
            return 1; // Successo
        }
    }

    fclose(fp);
    return 0; // se il while si conclude senza restituire niente, il record non è stato trovato o è già stato cancellato
}