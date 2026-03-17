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

static void ensure_rng_seeded(void) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
}

static int record_exists(FILE *fp, const char *matricola) {
    Record r;

    fseek(fp, 0, SEEK_SET);
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
    FILE *fp = fopen(ARCHIVE_PATH, "ab+");
    if (!fp) {
        return -1;
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

    if (fwrite(&to_write, sizeof(Record), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 1;
}

int archivio_read_all(void) {
    Record r;
    int count = 0;
    FILE *fp = fopen(ARCHIVE_PATH, "rb");
    if (!fp) {
        return -1;
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
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    // controllo di sicurezza per apertura del file (-1 rappresenta un errore)
    if (!fp) {
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
            fclose(fp);
            return 1; // Successo
        }
    }

    fclose(fp);
    return 0; // se il while si conclude senza restituire niente, il record non e' stato trovato o e' gia' stato cancellato
}

int archivio_restore(const char *matricola) {
    Record r;
    FILE *fp = fopen(ARCHIVE_PATH, "r+b");
    if (!fp) {
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
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}
