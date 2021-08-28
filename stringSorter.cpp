#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "stringSorter.h"

const char TEST_INPUT_FILE_PATH[] = "/home/ilya/CLionProjects/String-sorter/data/test-file.txt";
const char INPUT_FILE_PATH[] = "/home/ilya/CLionProjects/String-sorter/data/origin.txt";
const char OUTPUT_FILE_PATH[] = "/home/ilya/CLionProjects/String-sorter/data/sorted-file.txt";

/*
* 1. Считать все в один большой массив:
* 2. \n -> \0, посчитаем их и создадим массив указателей на \0
* 3. отсортируем их
* calloc/malloc char* char *
* f(), for () if ()
* consts? &? int / unsigned int
*/

int main() {
    printf("Program sorts rawData in provided file and prints them to output one.\n\n");

    char *rawData;
    unsigned int szFile = readDataFromFile(TEST_INPUT_FILE_PATH, rawData);

    int nStrings = replaceChars('\n', '\0', rawData) + 1; // +1 due to the beginning of the file

    char **strings = (char**) calloc(nStrings, sizeof(char));
    initStringPtrs(rawData, strings, nStrings, szFile);

    //qsort(strings, nStrings, sizeof(strings[0]), cmpStrings);

    printStringsToFile(OUTPUT_FILE_PATH, strings, nStrings);

    free(strings);
    free(rawData);

    return 0;
}

int cmpStrings(const void *str1, const void *str2)  {
    return strcmp((const char *)str1, (const char *)str2);
}

unsigned int readDataFromFile(const char filePath[], char* & rawData) {
    FILE *inpFile = fopen(filePath, "r");
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", filePath);
        abort();
    }

    fseek(inpFile, 0, SEEK_END);
    unsigned int szFile = ftell(inpFile);
    rewind(inpFile);

    rawData = (char*) calloc(szFile, sizeof(char));
    if (!rawData) {
        printf("Memory wasn't allocated.\n");
        abort();
    }

    unsigned int readingResult = fread(rawData, sizeof(char), szFile, inpFile);
    if (readingResult != szFile) {
        printf("Data from file wasn't read.\n");
        abort();
    }

    fclose(inpFile);

    return szFile;
}

int replaceChars(char fromChar, char toChar, char* &string) {
    assert(fromChar != '\0');

    int i = 0, nStrings = 0;
    while (string[i] != '\0') {
        if (string[i] == fromChar) {
            string[i] = toChar;
            nStrings++;
        }
        i++;
    }

    return nStrings;
}

void initStringPtrs(char *rawData, char **&strings, int nStrings, int szFile) {
    int nInits = 0;

    strings[nInits++] = &rawData[0];
    for (int i = 1; i < szFile - 1; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits++] = &rawData[i + 1];
        }
    }
    assert(nInits == nStrings);
}

void printStringsToFile(const char filePath[], char **strings, int nStrings) {
    FILE *outFile = fopen(filePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", filePath);
        abort();
    }

    for (int i = 0; i < nStrings; ++i) {
        fprintf(outFile, "%s", strings[i]);
    }

    fclose(outFile);
}
