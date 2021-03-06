#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cwctype>
#include "stringSorter.hpp"

int main(int argc, const char *argv[]) {
    printf("Program sorts strings in the provided file and prints them to the output one.\n\n");

    const char *inpFilePath = nullptr, *outFilePath = nullptr;
    SORT_FLAGS order = ASCENDING_ORDER, side = RIGHT_SIDE;
    int errorCode = handleCommandLineArgs(argc, argv, &side, &order, &inpFilePath, &outFilePath);
    if (errorCode) {
        return ARGUMENTS_HANDLING_ERROR;
    }

    FILE *inpFile = fopen(inpFilePath, "r");
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", inpFilePath);
        return FILE_OPENING_ERROR;
    }

    long signedSzFile = getFileSize(inpFile);
    if (signedSzFile == -1) {
        return SIZE_GETTING_ERROR;
    }
    size_t szFile = (size_t) signedSzFile;

    // +1 due to possible absence \n at the end of the file.
    char *rawData = (char*) calloc(szFile + 1, sizeof(char));
    if (!rawData) {
        printf("Memory wasn't allocated.\n");
        return MEM_ALLOCATING_ERROR;
    }

    readDataFromFile(inpFile, rawData, &szFile);
    fclose(inpFile);

    // +1 due to the string at the beginning of the file
    size_t nStrings = replaceChars('\n', '\0', rawData) + 1;
    // nStrings + 1 for nullptr at the end of array for security purposes
    stringData *strings = (stringData*) calloc(nStrings + 1, sizeof(stringData));

    // szFile + 1 is correct because of alloc(szFile + 1)
    int nInitStrings = initStringPtrs(rawData, strings, szFile + 1);
    assert(nInitStrings == nStrings);

    int (*qsortCmp)(const void*, const void*) = nullptr;
    if      (order == ASCENDING_ORDER  && side == LEFT_SIDE)  qsortCmp = cmpStringsLeft;
    else if (order == ASCENDING_ORDER  && side == RIGHT_SIDE) qsortCmp = cmpStringsRight;
    else if (order == DESCENDING_ORDER && side == LEFT_SIDE)  qsortCmp = cmpStringsLeftReverse;
    else if (order == DESCENDING_ORDER && side == RIGHT_SIDE) qsortCmp = cmpStringsRightReverse;
    assert(qsortCmp);
    qsort(strings, nStrings, sizeof(strings[0]), qsortCmp);

    FILE *outFile = fopen(outFilePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", outFilePath);
        return FILE_OPENING_ERROR;
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
 * Parse command line arguments.
 * @param argc, @param argv - main() args
 * @param key - flag to find in command line
 * @param useNextArg - whether to return argument(if it exists) after key
 * @return index in argv of found key(or next argument) or -1 if error has occurred.
 */
int parseCommandLineArgs(int argc, const char *argv[], const char *key, bool useNextArg) {
    assert(argv);
    assert(key);

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], key)) {
            if (useNextArg) {
                if (i == argc - 1) {
                    printf("Argument after %s was expected.\n", key);
                    return -1;
                }
                else {
                    return i + 1;
                }
            }
            return i;
        }
    }

    return -1;
}

/**
 * Handles command line arguments.
 * @param argc, @param argv main() args
 * @param side, @param order - SORT_FLAGS params to manage sorting parameters
 * @param inpFilePath, @param outFilePath - paths to files
 * @return 0 if arguments were handled successfully, 1 in other cases.
 */
int handleCommandLineArgs(int argc, const char *argv[], SORT_FLAGS *side, SORT_FLAGS *order, const char **inpFilePath,
                          const char **outFilePath) {
    assert(inpFilePath);
    assert(outFilePath);

    int sideArgPosition  = parseCommandLineArgs(argc, argv, "--side",  true);
    int orderArgPosition = parseCommandLineArgs(argc, argv, "--order", true);
    int inArgPosition    = parseCommandLineArgs(argc, argv, "--in",    true);
    int outArgPosition   = parseCommandLineArgs(argc, argv, "--out",   true);
    int helpArgPosition  = parseCommandLineArgs(argc, argv, "--help",  false);

    if (sideArgPosition != -1) {
        if (!strcmp(argv[sideArgPosition], "left")) {
            *side = LEFT_SIDE;
        }
        else if (!strcmp(argv[sideArgPosition], "right")) {
            *side = RIGHT_SIDE;
        }
        else {
            printf("Incorrect sorting side was entered. left/right is available.\n");
            return 1;
        }
    }

    if (orderArgPosition != -1) {
        if (!strcmp(argv[orderArgPosition], "ascent")) {
            *order = ASCENDING_ORDER;
        }
        else if (!strcmp(argv[orderArgPosition], "descent")) {
            *order = DESCENDING_ORDER;
        }
        else {
            printf("Incorrect sorting order was entered. ascent/descent is available.\n");
            return 1;
        }
    }

    if (helpArgPosition != -1) {
        printf("Usage: ./String_sorter [--side left/right] [--order ascent/descent] --in input_file --out output_file.\n");
    }

    if (inArgPosition != -1) {
        *inpFilePath = argv[inArgPosition];
    }
    else {
        printf("Input file argument was not found.\n");
        return 1;
    }

    if (outArgPosition != -1) {
        *outFilePath = argv[outArgPosition];
    }
    else {
        printf("Output file argument was not found.\n");
        return 1;
    }

    return 0;
}

/**
 * Gets size of the provided file in bytes.
 * @param filePath - path to file
 * @return size in bytes or -1 if error was found.
 */
long getFileSize(FILE *inpFile) {
    fseek(inpFile, 0, SEEK_END);
    long szFile = ftell(inpFile);
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
void readDataFromFile(FILE *inpFile, char *rawData, size_t *szFile) {
    assert(inpFile);
    assert(rawData);

    // fread on Windows returns readingResult <= szFile due to deleting /r from stream
    size_t readingResult = fread(rawData, sizeof(char), *szFile, inpFile);
    *szFile = readingResult;
    *(rawData + readingResult) = '\0';

    rewind(inpFile);
}

/**
 * Replaces chars in the provided string.
 * @param fromChar - char which will be replaced assuming it is not \0
 * @param toChar  - char to replace with
 * @param string - pointer to string which will be changed
 * @return number of changed symbols.
 */
size_t replaceChars(char fromChar, char toChar, char string[]) {
    assert(fromChar != '\0');
    assert(string);

    size_t nStrings = 0;
    int i = 0;
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
int initStringPtrs(char *rawData, stringData strings[], size_t szFile) {
    assert(rawData);
    assert(strings);

    int nInits = 0;

    strings[nInits++].str = rawData;
    for (size_t i = 1; i < szFile - 1; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits].str = rawData + i + 1;
            strings[nInits - 1].length = size_t(strings[nInits].str - strings[nInits - 1].str);
            ++nInits;
        }
    }
    strings[nInits - 1].length = size_t(rawData + szFile - strings[nInits-1].str);

    return nInits;
}

/**
 * Left-side tring comparator.
 */
int cmpStringsLeft(const void *str1, const void *str2)  {
    return strcmp(((const stringData*) str1)->str, ((const stringData*) str2)->str);
}

/**
 * Left-side reverse string comparator.
 */
int cmpStringsLeftReverse(const void *str1, const void *str2) {
    return -cmpStringsLeft(str1, str2);
}

/**
 * Gets index of first/last nonpunct symbol
 * @param str - stringData struct
 * @param reverseOrder - whether to check string in reverse order
 * @return 0 if string == "\0", 0/last index if string is consists of only punct symbols.
 */
size_t getFirstNonPunctIndex(const stringData* str, bool reverseOrder) {
    assert(str);
    assert(str->str);
    assert(str->length > 0);

    if (str->length == 1) {
        return 0;
    }

    if (reverseOrder) {
        const char *curPos = str->str + str->length;
        size_t nChecks = 1;
        while (nChecks++ < str->length) {
            if (!iswpunct((wint_t) *(curPos - nChecks))) {
                return str->length - nChecks;
            }
        }
        return str->length - 1;
    }
    else {
        const char *curPos = str->str;
        size_t nChecks = 0;
        while (nChecks < str->length - 1) {
            if (!iswpunct((wint_t) *(curPos + nChecks))) {
                return nChecks;
            }
            ++nChecks;
        }
        return 0;
    }
}

/**
 * strcmp analog with comparing strings in reverse order.
 * @param str1, @param str2 - string pointers
 * @param length1, @param length2 - string lengths with \0
 * @return 1 if str1 > str2, 0 if str1 == str2, -1 if str1 < str2.
 */
int strcmpReverseOrder(const stringData *str1, const stringData *str2) {
    assert(str1->str);
    assert(str1->length >= 0);
    assert(str2->str);
    assert(str2->length >= 0);

    size_t str1punctIdx = getFirstNonPunctIndex(str1, true),
           str2punctIdx = getFirstNonPunctIndex(str2, true);
    size_t minIndex = (str1punctIdx < str2punctIdx) ? str1punctIdx : str2punctIdx;
    size_t nChecks = 0;
    char *curStr1ptr = str1->str + str1punctIdx,
         *curStr2ptr = str2->str + str2punctIdx;
    while (nChecks++ <= minIndex) {
        char symbol1 = *(curStr1ptr--);
        char symbol2 = *(curStr2ptr--);

        if (symbol1 > symbol2) return 1;
        if (symbol1 < symbol2) return -1;
    }

    if (str1->length > str2->length) {
        return 1;
    }
    if (str2->length < str1->length) {
        return -1;
    }

    return 0;
}

/**
 * Right-side string comparator.
 */
int cmpStringsRight(const void *str1, const void *str2)  {
    return strcmpReverseOrder((const stringData*) str1, (const stringData*) str2);
}

/**
 * Right-side reverse string comparator.
 */
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
int printStringsToFile(FILE *outFile, const stringData strings[], size_t nStrings) {
    assert(outFile);
    assert(strings);

    for (size_t i = 0; i < nStrings - 1; ++i) {
        fprintf(outFile, "%s\n", strings[i].str);
    }
    fprintf(outFile, "%s", strings[nStrings - 1].str);

    return 0;
}
