#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_KEY 65536
#define WINDOW_BUFFER 256

#define B "\033[0;1m"
#define Y "\033[33m"
#define G "\033[36m"
#define T3 "\t\t\t"

typedef struct {
    unsigned short index;
    uint8_t len;
    uint8_t symbol;
} OUT;

void list(char* archive_name) {
    FILE* archive_file = fopen(archive_name, "rb");
    int lenname, count;
    while (fread(&lenname, sizeof(int), 1, archive_file) == 1) {
        char* filename = malloc(sizeof(char) * lenname);
        fread(filename, sizeof(char), lenname, archive_file);
        printf("%s\n", filename);
        fread(&count, sizeof(int), 1, archive_file);
        fseek(archive_file, sizeof(OUT) * count, SEEK_CUR);
        free(filename);
    }
    fclose(archive_file);
}

void archiving(char* filename, char* archive_name) {
    FILE* file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END); 
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *text = malloc(size);
    fread(text, size, 1, file);
    fclose(file);

    int ptr = 0, count = 0;
    FILE* archive_file = fopen(archive_name, "ab");
    OUT* archive = malloc(sizeof(OUT));
    while (ptr < size) {
        uint8_t key[WINDOW_KEY], buffer[WINDOW_BUFFER]; 
        int lkey = (ptr >= WINDOW_KEY) ? WINDOW_KEY : ptr;
        int lb = (ptr + WINDOW_BUFFER <= size) ? WINDOW_BUFFER : size - ptr ;
        for (int i = 0; i < lb; i++) {
            buffer[i] = text[ptr + i];
        }
        for (int i = 0; i < lkey; i++) {
            key[i] = text[ptr - lkey + i];
        }
        unsigned short index = 0;
        uint8_t len = 0;
        uint8_t lentemp;
        for (int i = 0; i < lkey; i++) {
            lentemp = 0;
            for (int j = 0; j < lb && i+j < lkey; j++) {
                if (key[i+j] == buffer[j]) lentemp++;
                else break;
            }
            if (lentemp > len) {
                len = lentemp;
                index = lkey - i - 1;
            }
        }
        archive = realloc(archive, (count + 1) * sizeof(OUT));
        archive[count] = (OUT){.index = index, .len = len, .symbol = buffer[len]};
        ptr += len + 1;
        count++;
    }
    int lenname = strlen(filename);
    fwrite(&lenname, sizeof(int), 1, archive_file);
    fwrite(filename, sizeof(char), lenname, archive_file);
    fwrite(&count, sizeof(int), 1, archive_file);
    fwrite(archive, sizeof(OUT), count, archive_file);
    free(text);
    free(archive);
    fclose(archive_file);
}

void addkey(FILE* file, uint8_t key[], uint8_t symbol) {
    fwrite(&symbol, sizeof(uint8_t), 1, file);
    for (int i = WINDOW_KEY - 1; i > 0; i--) key[i] = key[i-1];
    key[0] = symbol;
}

void unarchiving(char* archive_name) {
    FILE* archive_file = fopen(archive_name, "rb");
    int lenname, count;
    while (fread(&lenname, sizeof(lenname), 1, archive_file) == 1) {
        char* filename = malloc(sizeof(char) * lenname);
        fread(filename, sizeof(char), lenname, archive_file);
        FILE* file = fopen(filename, "wb");
        fread(&count, sizeof(count), 1, archive_file);
        uint8_t key[WINDOW_KEY] = {'\0'};
        OUT archive;
        for (int h = 0; h < count; h++) {
            fread(&archive, sizeof(archive), 1, archive_file);
            for (int i = 0; i < archive.len; i++) {
                addkey(file, key, key[archive.index]);
            }
            addkey(file, key, archive.symbol);
        }
        free(filename);   
        fclose(file);
    }
    fclose(archive_file);
}

void help(){
    printf(B"\nИспользование: ./main\n\n");
    printf(Y" -help "B T3 T3"Помощь\n\n");
    printf(Y" -a  "B G"[имя архива]"B Y" -f "B G"[файл1] [файл2] ..."B"\tАрхивация\n");
    printf(Y" -ad "B G"[имя архива]"B Y" -f "B G"[файл1] [файл2] ..."B"\tАрхивация + удаление файлoв\n\n");
    printf(Y" -u  "B G"[имя архива]"B T3"\tРазархивация\n");
    printf(Y" -ud "B G"[имя архива]"B T3"\tРазархивация + удаление архива\n\n");
    printf(Y" -l  "B G"[имя архива]"B T3"\tСписок файлов в архиве\n\n");
    printf(B"Для архива можно использовать любое расширение\n\n");
}

int main(int argc, char* argv[]) {
    if (argv[1] == NULL) {
        printf(B"Введите \"./main -help\" для получения инструкций\n");
    } else if (strcmp(argv[1], "-help") == 0) {
        help();
    } else if (strcmp(argv[1], "-a") == 0 && strcmp(argv[3], "-f") == 0) {
        for (int i = 4; i < argc; i++) {
            archiving(argv[i], argv[2]);
        }
    } else if (strcmp(argv[1], "-ad") == 0 && strcmp(argv[3], "-f") == 0) {
        for (int i = 4; i < argc; i++) {
            archiving(argv[i], argv[2]);
            remove(argv[i]);
        }
    } else if (strcmp(argv[1], "-u") == 0 && argc == 3) {
        unarchiving(argv[2]);
    } else if (strcmp(argv[1], "-ud") == 0 && argc == 3) {
        unarchiving(argv[2]);
        remove(argv[2]);
    } else if (strcmp(argv[1], "-l") == 0 && argc == 3) {
        list(argv[2]);
    } else {
        printf(B"Введите \"./main -help\" для получения инструкций\n");
    }
    return 0;
}

