#include "menu.h"
#include "archivio.h"
#include "general.h"

#include <stdio.h>
#include <string.h>

void menu_principale() {
    printf("\n===== MENU PRINCIPALE =====\n");
    printf("1. Accesso Utente\n");
    printf("2. Accesso Amministratore\n");
    printf("3. Esci dal programma\n");
}

void menu_admin() {
    printf("\n===== Archivio (Admin) =====\n");
    printf("1. Aggiungi un record\n");
    printf("2. Visualizza archivio\n");
    printf("3. Modifica un record\n");
    printf("4. Cancellazione fisica\n");
    printf("5. Cancellazione logica\n");
    printf("6. Ripristina record\n");
    printf("7. Logout (Torna al Menu Principale)\n");
}

void menu_user() {
    printf("\n===== Archivio (Utente) =====\n");
    printf("1. Aggiungi un record\n");
    printf("2. Visualizza archivio\n");
    printf("3. Logout (Torna al Menu Principale)\n");
}

void sessione_admin() {
    int choice = 0;
    int result = 0;

    // le menti piu' sagaci riconosceranno il riferimento nella seguente riga
    for (;;) {
        pulisci_schermo();
        menu_admin();
        read_int("Scelta: ", &choice);
        switch (choice) {
            // Aggiungi un record
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
                pausa_console();
                break;
            }
            // Visualizza archivio
            case 2:
                result = archivio_read_all(1);
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                }
                pausa_console();
                break;
            // Modifica un record
            case 3: {
                result = archivio_read_all(1);
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                    pausa_console();
                    break;
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                    pausa_console();
                    break;
                } else {
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
                    pausa_console();
                    break;
                }
            }
            // Cancellazione fisica
            case 4: {
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
                pausa_console();
                break;
            }
            // Cancellazione logica
            case 5: {
                result = archivio_read_all(1);
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                    pausa_console();
                    break;
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                    pausa_console();
                    break;
                } else {
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
                    pausa_console();
                    break;
                }
            }
            // Ripristina record
            case 6: {
                result = archivio_read_all(1);
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                    pausa_console();
                    break;
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                    pausa_console();
                    break;
                } else {
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
                    pausa_console();
                    break;
                }
            }
            // Logout
            case 7:
                printf("Uscita.\n");
                return;
            default:
                printf("Scelta non valida.\n");
        }
    }
}

void sessione_user() {
    int choice = 0;
    int result = 0;
    int in_esecuzione = 1; // flag per controllare il loop


    // ### DA VALUTARE QUALI OPERAZIONI PERMETTERE AGLI UTENTI (FORSE SOLO AGGIUNTA E VISUALIZZAZIONE?) ###
    do {
        // mostra il menu utente e legge la scelta
        pulisci_schermo();
        menu_user();
        read_int("Scelta: ", &choice);

        switch (choice) {
            // Aggiungi un record
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
                pausa_console();
                break;
            }
            // Visualizza archivio
            case 2:
                result = archivio_read_all(0);
                if (result < 0) {
                    printf("Archivio vuoto o inesistente.\n");
                } else if (result == 0) {
                    printf("Archivio vuoto.\n");
                }
                pausa_console();
                break;
            // Logout
            case 3:
                printf("Uscita.\n");
                in_esecuzione = 0; // imposta il flag a 0 per uscire dal loop
                break;
            // Scelta non valida
            default:
                printf("Scelta non valida.\n");
        }
    } while (in_esecuzione); // il loop continua finché in_esecuzione è vero
}