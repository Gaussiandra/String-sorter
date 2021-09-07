#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "stringSorter.hpp"

const char DEFAULT_OUTPUT_FILE_PATH[] = "./sorted-file.txt";

// норм парсинг
// комментарии менторов

int main(int argc, char *argv[]) {
    printf("Program sorts strings in the provided file and prints them to the output one.\n\n");

    char *inpFilePath = nullptr, *outFilePath = (char*)DEFAULT_OUTPUT_FILE_PATH;
    SORT_FLAGS order = UNKNOWN, side = UNKNOWN;
    int errorCode = handleCommandLineArgs(argc, argv, &side, &order, &inpFilePath, &outFilePath);
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
    stringData *strings = (stringData*) calloc(nStrings, sizeof(stringData));

    // szFile + 1 is correct because of alloc(szFile + 1)
    int nInitStrings = initStringPtrs(rawData, strings, szFile + 1);
    assert(nInitStrings == nStrings);

    int (*qsortCmp)(const void*, const void*) = nullptr;
    if      (order == ASCENDING_ORDER  && side == LEFT_SIDE)  qsortCmp = cmpStringsLeft;
    else if (order == ASCENDING_ORDER  && side == RIGHT_SIZE) qsortCmp = cmpStringsRight;
    else if (order == DESCENDING_ORDER && side == LEFT_SIDE)  qsortCmp = cmpStringsLeftReverse;
    else if (order == DESCENDING_ORDER && side == RIGHT_SIZE) qsortCmp = cmpStringsRightReverse;
    assert(qsortCmp);
    qsort(strings, nStrings, sizeof(strings[0]), qsortCmp);

    FILE *outFile = fopen(outFilePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", outFilePath);
        return -1;
    }
    printStringsToFile(outFile, strings, nStrings);
    fclose(outFile);

    free((void *) strings);
    strings = nullptr;
    free(rawData);
    rawData = nullptr;

    return 0;
}

/**
 * Handles command line arguments.
 * @param argc, @param argv main() args
 * @param side, @param order - SORT_FLAGS params to manage sorting parameters
 * @param inpFilePath, @param outFilePath - paths to files
 * @return 0 if arguments were handled successfully, 1 in other cases.
 */
int handleCommandLineArgs(int argc, char *argv[], SORT_FLAGS *side, SORT_FLAGS *order, char **inpFilePath,
                          char **outFilePath) {
    assert(inpFilePath);
    assert(outFilePath);

    if (argc == 5) {
        if (*argv[1] == *(char*)"left") {
            *side = LEFT_SIDE;
        }
        else if (*argv[1] == *(char*)"right") {
            *side = RIGHT_SIZE;
        }
        else {
            printf("Incorrect sorting side was entered. left/right is available.");
            return 1;
        }

        if (*argv[2] == *(char*)"ascent") {
            *order = ASCENDING_ORDER;
        }
        else if (*argv[2] == *(char*)"descent") {
            *order = DESCENDING_ORDER;
        }
        else {
            printf("Incorrect sorting order was entered. ascent/descent is available.");
            return 1;
        }

        *inpFilePath = argv[3];
        *outFilePath = argv[4];
        return 0;
    }

    printf("Number of arguments is incorrect. Usage: ./String_sorter left/right ascent/descent input_file output_file.");
    return 1;
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
 * @param strings - array of stringData struct with length and str fields
 * @param nStrings - number of strings to be sure data was initialized correctly
 * @param szFile - size of allocated data
 * @return number of initialized strings.
 */
int initStringPtrs(const char *rawData, stringData strings[], size_t szFile) { // почему тут в strings можно присваивание сделать...
    assert(rawData);
    assert(strings);

    int nInits = 0;

    strings[nInits++].str = (char*) rawData; // он создаст копию и её присвоит или всё равно даст указатель на константу
    for (int i = 1; i < szFile - 1; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits].str = (char*) rawData + i + 1;
            strings[nInits - 1].length = int(strings[nInits].str - strings[nInits - 1].str);
            ++nInits;
        }
    }
    strings[nInits - 1].length = int(rawData + szFile - strings[nInits-1].str);

    return nInits;
}

int cmpStringsLeft(const void *str1, const void *str2)  {
    return strcmp(((stringData*) str1)->str, ((stringData*) str2)->str);
}

int cmpStringsLeftReverse(const void *str1, const void *str2) {
    return -cmpStringsLeft(str1, str2);
}

/**
 * strcmp analog with comparing strings in reverse order.
 * @param str1, @param str2 - string pointers
 * @param length1, @param length2 - string lengths with \0
 * @return 1 if str1 > str2, 0 if str1 == str2, -1 if str1 < str2.
 */
int strcmpReverseOrder(const char *str1, int length1, const char *str2, int length2) {
    assert(str1);
    assert(length1 >= 0);
    assert(str2);
    assert(length2 >= 0);

    int minLength = (length1 < length2) ? length1 : length2;

    for (int i = 0; i <= minLength; ++i) {
        const char symbol1 = *(str1 + length1 - i - 1),
                   symbol2 = *(str2 + length2 - i - 1);

        if (symbol1 > symbol2) {
            return 1;
        }
        if (symbol1 < symbol2) {
            return -1;
        }

    }

    if (length1 > length2) {
        return 1;
    }
    if (length2 < length1) {
        return -1;
    }

    return 0;
}

int cmpStringsRight(const void *str1, const void *str2)  {
    return strcmpReverseOrder(((stringData *) str1)->str,
                              ((stringData *) str1)->length,
                              ((stringData *) str2)->str,
                              ((stringData *) str2)->length);
}

int cmpStringsRightReverse(const void *str1, const void *str2) {
    return -cmpStringsRight(str1, str2);
}

/**
 * Prints strings to file
 * @param outFile - path to output file
 * @param strings - array of char*(pointers to strings)
 * @param nStrings - number of strings
 * @return 0 if data was printed successfully, -1 if other cases.
 */
int printStringsToFile(FILE *outFile, const stringData strings[], int nStrings) {
    assert(outFile);
    assert(strings);

    for (int i = 0; i < nStrings - 1; ++i) {
        fprintf(outFile, "%s\n", strings[i].str);
    }
    fprintf(outFile, "%s", strings[nStrings - 1].str);

    return 0;
}
