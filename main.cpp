#include <cstdio>
#include <cstdlib>
#include <cstring>

//#define INPUT_FILE_PATH "/home/ilya/CLionProjects/String-sorter/data/test-file.txt"
// const char file[] = "asd";
#define INPUT_FILE_PATH "/home/ilya/CLionProjects/String-sorter/data/origin.txt"
#define OUTPUT_FILE_PATH "/home/ilya/CLionProjects/String-sorter/data/sorted-file.txt"

const int MAX_STR_LEN = 500;
const int MAX_NSTRING_IN_FILE = 15000;

int cmpStrings(const void *str1, const void *str2); //!
int readStringsFromFile(const char filePath[], char strings[MAX_NSTRING_IN_FILE][MAX_STR_LEN]);
void printStringsToFile(const char filePath[], char strings[MAX_NSTRING_IN_FILE][MAX_STR_LEN], int nStrings);

int main() {
    printf("Program sorts strings in provided file and prints them to output one.\n\n");

    char strings[MAX_NSTRING_IN_FILE][MAX_STR_LEN] = {}; //!
    /*
     * 1. Считать все в один большой массив:
     * 2.
     */
    int nStrings = readStringsFromFile(INPUT_FILE_PATH, strings);
    qsort(strings, nStrings, sizeof(strings[0]), cmpStrings);
    printStringsToFile(OUTPUT_FILE_PATH, strings, nStrings);

    return 0;
}

int cmpStrings(const void *str1, const void *str2)  {
    return strcmp((const char *)str1, (const char *)str2); //STRNCMP
}

int readStringsFromFile(const char filePath[], char strings[MAX_NSTRING_IN_FILE][MAX_STR_LEN]) {
    FILE *inpFile = fopen(filePath, "r");
    if (!inpFile) {
        printf("File didn't open. Provided path: %s\n", filePath);
        return -1; //!
    }

    int i = 0;
    while (fgets(strings[i], MAX_STR_LEN, inpFile)) //!
        ++i;

    fclose(inpFile);

    return i + 1;
}

void printStringsToFile(const char filePath[], char strings[MAX_NSTRING_IN_FILE][MAX_STR_LEN], int nStrings) {
    FILE *outFile = fopen(filePath, "w");
    if (!outFile) {
        printf("File didn't open. Provided path: %s\n", filePath);
        abort();
    }

    for (int i = 0; i < nStrings; ++i) {
        fprintf(outFile, "%s", strings[i]);
    }

    fclose(outFile);
}
