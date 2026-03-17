#include "Branzino/archivio.h"
#include "utils/general.h"

#include <stdio.h>
#include <string.h>

#define ADMIN_PASSWORD "admin123"

static void menu_user(void) {
    printf("\n===== Archivio (User) =====\n");
    printf("1. Aggiungi un record\n");
    printf("2. Visualizza archivio (solo non cancellati)\n");
    printf("3. Modifica un record\n");
    printf("4. Cancellazione logica\n");
    printf("5. Login admin\n");
    printf("6. Esci\n");
}

static void menu_admin(void) {
    printf("\n===== Archivio (Admin) =====\n");
    printf("1. Visualizza archivio (anche cancellati)\n");
    printf("2. Cancellazione fisica\n");
    printf("3. Ripristina record\n");
    printf("4. Logout\n");
}

static int admin_login(void) {
    char password[64];
    read_string("Password admin: ", password, sizeof(password));
    return strcmp(password, ADMIN_PASSWORD) == 0;
}

static void run_admin(void) {
    int choice = 0;
    int result = 0;

    for (;;) {
        menu_admin();
        read_int("Scelta: ", &choice);
        switch (choice) {
            case 1:
                result = archivio_read_all();
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                }
                break;
            case 2: {
                char matricola[MATRICOLA_LEN];
                read_string("Matricola da cancellare (fisica): ", matricola, sizeof(matricola));
                result = archivio_delete_physical(matricola);
                if (result == 1) {
                    printf("Record cancellato fisicamente.\n");
                } else if (result == 0) {
                    printf("Matricola non trovata.\n");
                } else {
                    printf("Errore durante la cancellazione.\n");
                }
                break;
            }
            case 3: {
                char matricola[MATRICOLA_LEN];
                read_string("Matricola da ripristinare: ", matricola, sizeof(matricola));
                result = archivio_restore(matricola);
                if (result == 1) {
                    printf("Record ripristinato.\n");
                } else if (result == 0) {
                    printf("Matricola non trovata o non cancellata.\n");
                } else {
                    printf("Errore durante il ripristino.\n");
                }
                break;
            }
            case 4:
                printf("Logout admin.\n");
                return;
            default:
                printf("Scelta non valida.\n");
        }
    }
}

int main(void) {
    int choice = 0;
    int result = 0;

    // le menti piu' sagaci riconosceranno il riferimento nella seguente riga
    for (;;) {
        menu_user();
        read_int("Scelta: ", &choice);
        switch (choice) {
            case 1: {
                Record r;
                read_string("Nome: ", r.nome, sizeof(r.nome));
                read_string("Cognome: ", r.cognome, sizeof(r.cognome));
                read_float("Stipendio: ", &r.stipendio);
                read_string("Classe: ", r.classe, sizeof(r.classe));
                r.cancellato = 0;

                result = archivio_add(&r);
                if (result == 1) {
                    printf("Record aggiunto. Matricola: %s\n", r.matricola);
                } else if (result == 0) {
                    printf("Matricola gia' presente.\n");
                } else {
                    printf("Errore scrittura.\n");
                }
                break;
            }
            case 2:
                result = archivio_read_all();
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                }
                break;
            case 3: {
                char matricola[MATRICOLA_LEN];
                Record nuovo;
                read_string("Matricola da modificare: ", matricola, sizeof(matricola));
                read_string("Nome: ", nuovo.nome, sizeof(nuovo.nome));
                read_string("Cognome: ", nuovo.cognome, sizeof(nuovo.cognome));
                read_float("Stipendio: ", &nuovo.stipendio);
                read_string("Classe: ", nuovo.classe, sizeof(nuovo.classe));
                strncpy(nuovo.matricola, matricola, sizeof(nuovo.matricola));
                nuovo.matricola[sizeof(nuovo.matricola) - 1] = '\0';
                nuovo.cancellato = 0;

                result = archivio_update(matricola, &nuovo);
                if (result == 1) {
                    printf("Record modificato.\n");
                } else if (result == 0) {
                    printf("Matricola non trovata.\n");
                } else {
                    printf("Errore scrittura.\n");
                }
                break;
            }
            case 4: {
                char matricola[MATRICOLA_LEN];
                read_string("Matricola da cancellare (logica): ", matricola, sizeof(matricola));
                result = archivio_delete_logical(matricola);
                if (result == 1) {
                    printf("Record cancellato logicamente.\n");
                } else if (result == 0) {
                    printf("Matricola non trovata o gia' cancellata.\n");
                } else {
                    printf("Errore durante la cancellazione.\n");
                }
                break;
            }
            case 5:
                if (admin_login()) {
                    run_admin();
                } else {
                    printf("Password errata.\n");
                }
                break;
            case 6:
                printf("Uscita.\n");
                return 0;
            default:
                printf("Scelta non valida.\n");
        }
    }
}
