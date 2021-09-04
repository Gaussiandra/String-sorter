#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "stringSorter.hpp"

const char DEFAULT_OUTPUT_FILE_PATH[] = "./sorted-file.txt";

/*
* consts? &? int / unsigned int
 * readmi
 * doxygen
 * исправления из первого задания
*/

int main(int argc, char *argv[]) {
    printf("Program sorts strings in the provided file and prints them to the output one.\n\n");

    char *inpFilePath = nullptr, *outFilePath = (char*)DEFAULT_OUTPUT_FILE_PATH;
    int returnStatus = handleCommandLineArgs(argc, argv, &inpFilePath, &outFilePath);
    if (returnStatus) {
        return 1;
    }

    char *rawData;
    int szFile = readDataFromFile(inpFilePath, &rawData);
    if (szFile == -1) {
        return 1;
    }

    // +1 due to the string at the beginning of the file
    int nStrings = replaceChars('\n', '\0', rawData) + 1;

    char **strings = (char**) calloc(nStrings, sizeof(char*));
    initStringPtrs(rawData, strings, nStrings, szFile);

    qsort(strings, nStrings, sizeof(strings[0]), cmpStrings);

    printStringsToFile(outFilePath, strings, nStrings);

    free(strings);
    free(rawData);

    return 0;
}

int handleCommandLineArgs(int argc, char *argv[], char **inpFilePath, char **outFilePath) {
    switch(argc) {
        case 1: {
            printf("Input file name was not entered.");
            return 1;
        }
        case 2: {
            *inpFilePath = argv[1];
            return 0;
        }
        case 3: {
            *inpFilePath = argv[1];
            *outFilePath = argv[2];
            return 0;
        }
        default: {
            printf("Too many arguments. Usage: ./String_sorter input_file [output_file].");
            return 1;
        }
    }
}

int getFileSize(const char filePath[]) {
    FILE *inpFile = fopen(filePath, "r");
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", filePath);
        return -1;
    }

    fseek(inpFile, 0, SEEK_END);
    int szFile = (int)ftell(inpFile);

    fclose(inpFile);
    return szFile;
}

int readDataFromFile(const char filePath[], char **rawData) {
    FILE *inpFile = fopen(filePath, "r");
    assert(inpFile);

    int fileSizeReturnValue = getFileSize(filePath);
    if (fileSizeReturnValue == -1) {
        return -1;
    }
    // +1 due to possible absence \n at the end of the file.
    int szFile = fileSizeReturnValue + 1;
    *rawData = (char*) calloc(szFile, sizeof(char));
    if (!*rawData) {
        printf("Memory wasn't allocated.\n");
        return -1;
    }

    unsigned int readingResult = fread(*rawData, sizeof(char), szFile, inpFile);
    printf("%d %d", readingResult, szFile);
    if (readingResult != szFile) {
        printf("Data from file wasn't read.\n");
        return -1;
    }

    fclose(inpFile);

    return szFile;
}

int replaceChars(char fromChar, char toChar, char *string) {
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

void initStringPtrs(char *rawData, char **strings, int nStrings, unsigned int szFile) { // const rawData
    int nInits = 0;

    strings[nInits++] = &rawData[0];
    for (int i = 1; i < szFile; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits++] = &rawData[i + 1]; // i + 1 is correct because of alloc(szFile + 1)
        }
    }
    assert(nInits == nStrings);
}

int cmpStrings(const void *str1, const void *str2)  {
    return strcmp(*(const char **)str1, *(const char **)str2);
}

int printStringsToFile(const char filePath[], char **strings, int nStrings) {
    FILE *outFile = fopen(filePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", filePath);
        return -1;
    }

    for (int i = 0; i < nStrings; ++i) {
        fprintf(outFile, "%s\n", strings[i]);
    }

    fclose(outFile);
    return 0;
}
