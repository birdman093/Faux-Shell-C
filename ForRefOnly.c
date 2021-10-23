#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

// Stores data regarding movie structure
struct movie {

    char* title;
    int year;
    char* language;
    double rating;
    struct movie *next;
};

// Creates directory named feathers.movies.XXXXX where XXXX is a random number
char* createDirectory() 
{
    long int randomNum = random();
    int randomFileDir = randomNum % 100000;
    char randomstr[6];
    sprintf(randomstr, "%i", randomFileDir);
    strcpy(pathname, "featheru.movies.");
    strcat(pathname, randomstr);
    mkdir(pathname,0750);
    printf("Created directory with name %s",pathname);
    return pathname;
}

void SpaceRemoval(char*, char*);
void yearFiles(struct movie**, int, char*);

// createMovie processes line of data based on comma and places data in movie struct
struct movie* createMovie(char* currLine) 
{
    struct movie* currMovie = malloc(sizeof(struct movie));
    
    char* token = strtok_r(currLine,",",&currLine);
    currMovie->title = calloc(strlen(token)+1, sizeof(char));
    strcpy(currMovie->title, token);

    token = strtok_r(NULL, " ,", &currLine);
    currMovie->year = atoi(token);

    token = strtok_r(NULL, ",", &currLine);
    char* tokenSpaceless = calloc(strlen(token)+1, sizeof(char));
    SpaceRemoval(token, tokenSpaceless);
    currMovie->language = calloc(strlen(tokenSpaceless)+1, sizeof(char));   
    strcpy(currMovie->language, tokenSpaceless);

    token = strtok_r(NULL, " ,", &currLine);
    currMovie->rating = strtod(token, NULL);

    currMovie->next = NULL;

    return currMovie;
}

// processCSV goes line by line through CSV file and calls createMovie
struct movie* processCSV(char* csvPath, int* earlyYear, int* lateYear)
{
    FILE* csvFile = fopen(csvPath, "r");

    char* currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    //char* token;

    struct movie* firstMovie = NULL;
    struct movie* currMovie = NULL;

    // skip header on first line
    nread = getline(&currLine, &len, csvFile);

    // upload information into struct
    while ((nread = getline(&currLine, &len, csvFile)) != -1) 
    {
        if (firstMovie == NULL) 
        {
            firstMovie = createMovie(currLine);
            currMovie = firstMovie;
            *earlyYear = currMovie->year;
            *lateYear = currMovie->year;
        } else {
            currMovie->next = createMovie(currLine);
            currMovie = currMovie->next;
            if (*earlyYear > currMovie->year) {
                *earlyYear = currMovie->year;
            } else if (*lateYear < currMovie->year) {
                *lateYear = currMovie->year;
            }
        }
    }

    free(currLine);
    fclose(csvFile);
    return firstMovie;
}

// Process File calles Process CSV to divide info into movie structs. Then filters the movie struct data into a
// sorted array by year.  
void processFile(char* fileName, char* pathName) {
    
    // track number of years captured by movies to generate an array to store the struct linked lists in
    int earlyYear = 0;
    int lateYear = 0;
    struct movie* firstMovie;
    firstMovie = processCSV(fileName, &earlyYear, &lateYear);
    int arraySize = lateYear - earlyYear + 1;
    struct movie** MovieYearArray = calloc(arraySize,sizeof(struct movie*));

    // Iterate through movies using currMovie, stop when Currmovie hits the end
    struct movie* currMovie = firstMovie;
    struct movie* indexMovie;
    struct movie* currYear;
    int index;
    
    while (currMovie != NULL) {
        // update currMovie, and update properties of indexMovie to disconnect from linked list
        indexMovie = currMovie;
        currMovie = currMovie->next;
        indexMovie->next = NULL;

        //Check Array value to see if pointer exists, if not initialize, if so then iterate through using currYear
        index = indexMovie->year - earlyYear;
        if (MovieYearArray[index] == NULL) {
            MovieYearArray[index] = indexMovie;
        } else {
            currYear = MovieYearArray[index];
            while(currYear->next != NULL) {
                currYear = currYear->next;
            }
            currYear->next = indexMovie;
        }
    }
    yearFiles(MovieYearArray, arraySize, pathName);
    free(MovieYearArray);
}