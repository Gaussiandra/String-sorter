int handleCommandLineArgs(int argc, char *argv[], char **inpFilePath, char **outFilePath);
size_t getFileSize(FILE *inpFile);
int readDataFromFile(FILE *inpFile, char *rawData, size_t szFile);
int replaceChars(char fromChar, char toChar, char string[]);
int initStringPtrs(const char *rawData, const char *strings[], size_t szFile);
int cmpStrings(const void *str1, const void *str2);
int printStringsToFile(FILE *outFile, const char *strings[], int nStrings);