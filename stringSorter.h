unsigned int readDataFromFile(const char filePath[], char* &rawData);
int replaceChars(char fromChar, char toChar, char* &string);
void initStringPtrs(const char *rawData, char *&strings, int nStrings, int szFile);
int cmpStrings(const void *str1, const void *str2); //!
void printStringsToFile(const char filePath[], char *strings, int nStrings);