#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "stringSorter.hpp"

const char DEFAULT_OUTPUT_FILE_PATH[] = "./sorted-file.txt";

int main(int argc, char *argv[]) {
    printf("Program sorts strings in the provided file and prints them to the output one.\n\n");

    char *inpFilePath = nullptr, *outFilePath = (char*)DEFAULT_OUTPUT_FILE_PATH;
    int errorCode = handleCommandLineArgs(argc, argv, &inpFilePath, &outFilePath);
    if (errorCode) {
        return 1;
    }

    FILE *inpFile = fopen(inpFilePath, "r");
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", inpFilePath);
        return 1;
    }

    size_t szFile = getFileSize(inpFile);
    if (szFile == -1) {
        return 1;
    }

    // +1 due to possible absence \n at the end of the file.
    char *rawData = (char*) calloc(szFile + 1, sizeof(char));
    if (!rawData) {
        printf("Memory wasn't allocated.\n");
        return 1;
    }

    if (readDataFromFile(inpFile, rawData, szFile)) {
        return 1;
    }
    fclose(inpFile);

    // +1 due to the string at the beginning of the file
    int nStrings = replaceChars('\n', '\0', rawData) + 1;

    const char **strings = (const char**) calloc(nStrings, sizeof(const char*));

    // szFile + 1 is correct because of alloc(szFile + 1)
    int nInitStrings = initStringPtrs(rawData, strings, szFile + 1);
    assert(nInitStrings == nStrings);

    qsort(strings, nStrings, sizeof(strings[0]), cmpStrings);

    FILE *outFile = fopen(outFilePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", outFilePath);
        return -1;
    }
    printStringsToFile(outFile, strings, nStrings);
    fclose(outFile);

    free(strings);
    strings = nullptr;
    free(rawData);
    rawData = nullptr;

    return 0;
}

/**
 * Handles command line arguments.
 * @param argc, @param argv main() args
 * @param inpFilePath, @param outFilePath - paths to files
 * @return 0 if arguments were handled successfully, 1 in other cases.
 */
int handleCommandLineArgs(int argc, char *argv[], char **inpFilePath, char **outFilePath) {
    assert(inpFilePath);
    assert(outFilePath);

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

/**
 * Gets size of the provided file in bytes.
 * @param filePath - path to file
 * @return size in bytes or -1 if error was found.
 */
size_t getFileSize(FILE *inpFile) {
    rewind(inpFile);
    fseek(inpFile, 0, SEEK_END);
    size_t szFile = ftell(inpFile);
    rewind(inpFile);

    return szFile;
}

/**
 * Reads all data from the provided file to given pointer assuming memory has been already allocated.
 * @param inpFIle - path to file to read from
 * @param rawData - pointer to allocated memory
 * @param szFile - size of allocated memory
 * @return 0 if data was read successfully, -1 in other cases.
 */
int readDataFromFile(FILE *inpFile, char *rawData, size_t szFile) {
    assert(inpFile);
    assert(rawData);

    size_t readingResult = fread(rawData, sizeof(char), szFile, inpFile);
    if (readingResult != szFile) {
        printf("Data from file wasn't read.\n");
        return -1;
    }

    rewind(inpFile);
    return 0;
}

/**
 * Replaces chars in the provided string.
 * @param fromChar - char which will be replaced assuming it is not \0
 * @param toChar  - char to replace with
 * @param string - pointer to string which will be changed
 * @return number of changed symbols.
 */
int replaceChars(char fromChar, char toChar, char string[]) {
    assert(fromChar != '\0');
    assert(string);

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

/**
 * Initializes pointers assuming strings was divided by \0.
 * @param rawData - pointer to data with all strings
 * @param strings - pointer to allocated array of string pointers
 * @param nStrings - number of strings to be sure data was initialized correctly
 * @param szFile - size of allocated data
 * @return number of initialized strings.
 */
int initStringPtrs(const char *rawData, const char *strings[], size_t szFile) {
    assert(rawData);
    assert(strings);

    int nInits = 0;

    strings[nInits++] = rawData;
    for (int i = 1; i < szFile - 1; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits++] = rawData + i + 1;
        }
    }

    return nInits;
}

/**
 * Comparator for qsort function.
 */
int cmpStrings(const void *str1, const void *str2)  {
    return strcmp(*(const char **)str1, *(const char **)str2);
}

/**
 * Prints strings to file
 * @param outFile - path to output file
 * @param strings - array of char*(pointers to strings)
 * @param nStrings - number of strings
 * @return 0 if data was printed successfully, -1 if other cases.
 */
int printStringsToFile(FILE *outFile, const char *strings[], int nStrings) {
    assert(outFile);
    assert(strings);

    for (int i = 0; i < nStrings - 1; ++i) {
        fprintf(outFile, "%s\n", strings[i]);
    }
    fprintf(outFile, "%s", strings[nStrings - 1]);

    return 0;
}
