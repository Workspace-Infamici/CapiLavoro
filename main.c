#include "Branzino/archivio.h"
#include "utils/general.h"
#include "utils/menu.h"

#include <stdio.h>
#include <string.h> // per il confronto delle password

int main(void) {
    int scelta = 0;
    int in_esecuzione = 1; // flag di controllo del loop

    // il loop continua finché in_esecuzione è vero (!= 0)
    do {
        menu_principale();
        read_int("Scelta: ", &scelta);
        
        switch (scelta) {
            case 1:
                printf("Benvenuto Utente!\n");
                sessione_user();
                break;
            case 2:
                char password[32];
                read_password("Inserisci password Admin: ", password, sizeof(password));
                
                // password hardcodata per semplicita', in futuro da sostituire con autenticazione piu' robusta (hash?)
                if (strcmp(password, "mannoale123") == 0) {
                    printf("\nAccesso Admin consentito.\n");
                    sessione_admin();
                } else {
                    printf("\nAccesso Negato: password errata!\n");
                }
                break;
            case 3:
                printf("Hai scelto di uscire dal programma.\n");
                in_esecuzione = 0; // imposta il flag a 0 per uscire dal loop
                break;
            default:
                printf("Scelta non valida.\n");
        }
    } while (in_esecuzione);

    return 0;
}
