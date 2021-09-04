int handleCommandLineArgs(int argc, char *argv[], char **inpFilePath, char **outFilePath);
int getFileSize(FILE *filePath);
int readDataFromFile(FILE *inpFIle, char *rawData, int szFile);
int replaceChars(char fromChar, char toChar, char string[]);
int initStringPtrs(const char *rawData, const char *strings[], unsigned int szFile);
int cmpStrings(const void *str1, const void *str2);
int printStringsToFile(FILE *outFile, const char *strings[], int nStrings);