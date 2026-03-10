#include "Branzino/archivio.h"
#include "utils/general.h"

#include <stdio.h>

static void menu(void) {
    printf("\n===== Archivio (Admin) =====\n");
    printf("1. Aggiungi un record\n");
    printf("2. Visualizza archivio\n");
    printf("3. Modifica un record\n");
    printf("4. Cancellazione fisica\n");
    printf("5. Cancellazione logica\n");
    printf("6. Ripristina record\n");
    printf("7. Esci\n");
}

int main(void) {
    int choice = 0;
    int result = 0;

    // le menti piu' sagaci riconosceranno il riferimento nella seguente riga
    for (;;) {
        menu();
        read_int("Scelta: ", &choice);
        switch (choice) {
            case 1: {
                Record r;
                read_int("Matricola: ", &r.matricola);
                read_string("Nome: ", r.nome, sizeof(r.nome));
                read_string("Cognome: ", r.cognome, sizeof(r.cognome));
                read_float("Stipendio: ", &r.stipendio);
                read_string("Classe: ", r.classe, sizeof(r.classe));
                r.cancellato = 0;

                result = archivio_add(&r);
                if (result == 1) {
                    printf("Record aggiunto.\n");
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
                int matricola = 0;
                Record nuovo;
                read_int("Matricola da modificare: ", &matricola);
                read_string("Nome: ", nuovo.nome, sizeof(nuovo.nome));
                read_string("Cognome: ", nuovo.cognome, sizeof(nuovo.cognome));
                read_float("Stipendio: ", &nuovo.stipendio);
                read_string("Classe: ", nuovo.classe, sizeof(nuovo.classe));
                nuovo.matricola = matricola;
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
                int matricola = 0;
                read_int("Matricola da cancellare (fisica): ", &matricola);
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
            case 5: {
                int matricola = 0;
                read_int("Matricola da cancellare (logica): ", &matricola);
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
            case 6: {
                int matricola = 0;
                read_int("Matricola da ripristinare: ", &matricola);
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
            case 7:
                printf("Uscita.\n");
                return 0;
            default:
                printf("Scelta non valida.\n");
        }
    }
}
