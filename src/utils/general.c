#include "general.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Librerie per la lettura della password senza echo a seconda del sistema operativo (Ringraziamo Gemini)
#ifdef _WIN32
    #include <conio.h> // Per _getch() su Windows
#else
    #include <termios.h> // Per disabilitare l'eco su Linux/MacOS
    #include <unistd.h>
#endif

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

void read_string(const char *prompt, char *buf, size_t size) {
    for (;;) {
        printf("%s", prompt);
        if (!fgets(buf, (int)size, stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(buf);
        if (buf[0] == '\0') {
            printf("Valore non valido. Riprova.\n");
            continue;
        }
        return;
    }
}

// Funzione per leggere una password senza echo a schermo e con supporto per cancellazione (backspace) (Ringraziamo Gemini)
void read_password(const char *prompt, char *password, size_t size) {
    printf("%s", prompt);
    fflush(stdout); // Assicura che il prompt venga stampato subito

#ifdef _WIN32
    // IMPLEMENTAZIONE WINDOWS (Stampa gli asterischi *)
    int i = 0;
    char c;
    int inserimento = 1; // Flag di controllo del loop

    while (inserimento) {
        c = _getch(); // Legge un carattere senza stamparlo a schermo
        
        if (c == '\r' || c == '\n') { // Pressione del tasto Invio
            password[i] = '\0';
            printf("\n");
            inserimento = 0; // Uscita dal loop
        } else if (c == '\b') { // Pressione del tasto Backspace (cancella)
            if ((size_t)i > 0) { // controlla se c'e' qualcosa da cancellare (cast a size_t per evitare warning)
                i--;
                printf("\b \b"); // Cancella l'asterisco visivamente
            }
        } else if ((size_t)i < size - 1) { // Qualsiasi altro carattere (se c'e' spazio) (cast a size_t per evitare warning)
            password[i++] = c; 
            printf("*"); // Stampa un asterisco al posto del carattere
        }
    }
#else
    // IMPLEMENTAZIONE LINUX/MACOS (Nasconde totalmente l'input, come fa "sudo")
    struct termios oldt, newt;
    
    // Ottiene le impostazioni attuali del terminale
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    
    // Disabilita il flag ECHO (quello che stampa i caratteri a schermo)
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Legge la stringa normalmente
    if (fgets(password, size, stdin) != NULL) {
        size_t len = strlen(password);
        if (len > 0 && password[len - 1] == '\n') {
            password[len - 1] = '\0';
        }
    }
    printf("\n");

    // Ripristina le impostazioni normali del terminale
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

int read_int(const char *prompt, int *out) {
    char line[64];
    char *end = NULL;
    long value;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(line);
        errno = 0;
        value = strtol(line, &end, 10);
        if (errno != 0 || end == line || *end != '\0') {
            printf("Numero non valido. Riprova.\n");
            continue;
        }
        *out = (int)value;
        return 1;
    }
}

int read_float(const char *prompt, float *out) {
    char line[64];
    char *end = NULL;
    float value;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            clearerr(stdin);
            continue;
        }
        trim_newline(line);
        errno = 0;
        value = strtof(line, &end);
        if (errno != 0 || end == line || *end != '\0') {
            printf("Numero non valido. Riprova.\n");
            continue;
        }
        *out = value;
        return 1;
    }
}

void print_generic(const void *data, PrintFn printer) {
    if (!data || !printer) {
        return;
    }
    printer(data);
}

void print_int(const void *data) {
    const int *value = (const int *)data;
    printf("%d\n", *value);
}

void print_float(const void *data) {
    const float *value = (const float *)data;
    printf("%.2f\n", *value);
}

void print_cstring(const void *data) {
    const char *value = (const char *)data;
    printf("%s\n", value ? value : "");
}
