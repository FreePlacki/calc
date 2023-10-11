#include <stdio.h>
#include <stdlib.h>

void print_help(char* name) {
    fprintf(stderr, "Uruchamianie:\n\t%s ", name);
    fprintf(stderr, "<ścieżka do pliku wejściowego> [ścieżka do pliku wyjściowego]\n\n");
    fprintf(stderr, "Plik wejściowy musi być w formacie `*.txt`\n");
    fprintf(stderr, "Plik wyjściowy jest opcjonalny ");
    fprintf(stderr, "(zostanie stworzony na podstawie nazwy pliku wejściowego `out_*.txt`)\n");
}

int main(int argc, char** argv) {
    if (argc == 1 || argc > 3) {
        print_help(argv[0]);
        exit(1);
    }


    FILE* in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
        fprintf(stderr, "Nie można otworzyć pliku `%s`\n", argv[1]);
        exit(1);
    }
    // TODO
    FILE* out_file = fopen(argc == 3 ? argv[2] : "out_.txt", "w");

    
    // Liczby mają długość 40 więc powinniśmy zmieścić się w 100 znakach na linię
    char buffer[100];
    while (fgets(buffer, sizeof(buffer), in_file) != NULL) {
        printf("LINE: %s", buffer);
        fprintf(out_file, "LINE: %s", buffer);
    }

    fclose(in_file);
    return 0;
}
