/*
Made by: Jordan Winkler
Sat Oct 24 13:24:06 EDT 2020

A simple program to track study time, so it can be parsed by
org mode in emacs.

To compile:
    This lang is gnu C. Use gcc to compile it.

Use:
    - Use without arguments to get an hr:min:sec timer.
    - Use with name of study topic to get a named timer.
      This will also write to a timestamped csv file by
      the same name.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char** argv)
{
    time_t start_time = time(NULL);
    time_t current_time = time(NULL);
    double diff; // diff of start of program to call
    FILE* fp; // for potential csv file later

    char* concat(const char* s1, const char* s2)
    {
        char* result = malloc(strlen(s1) + strlen(s2) +
                              1); // +1 for the null-terminator
        // in real code you would check for errors in malloc here
        strcpy(result, s1);
        strcat(result, s2);
        return result;
    }

    char* file_name;
    if(argc == 2)
    {
        struct tm* timeinfo;
        timeinfo = localtime(&start_time);
        char buffer[80];
        strftime(buffer, 80, "%A:%Y:%j:%X", timeinfo);
        file_name = concat(argv[1], buffer);
        file_name = concat(file_name, ".csv"); // lazy leak
        fp = fopen(file_name, "w+");
    }

    int hours(double diff)
    {
        diff = diff / (60 * 60);
        return (int) diff;
    }

    int minutes(double diff)
    {
        diff = (int)diff % (60 * 60);
        diff = diff / 60;
        return (int) diff;
    }

    int seconds(double diff)
    {
        diff = (int)diff % (60);
        return (int) diff;
    }

    while(1)
    {
        current_time = time(NULL);
        diff = difftime(current_time, start_time);

        system("clear");
        if(argc == 2)
        {
            printf(
                "[%s]%i:%i:%i\n", argv[1],
                hours(diff),
                minutes(diff),
                seconds(diff));
            fp = fopen(file_name, "w+");
            fprintf(fp,
                    "[%s],%i,%i,%i\n", argv[1],
                    hours(diff),
                    minutes(diff),
                    seconds(diff));
            fclose(fp); // bit of processor waste
        }
        else
            printf("%i:%i:%i\n",
                   hours(diff),
                   minutes(diff),
                   seconds(diff));
        sleep(1);
    }

    // clean up
    free(file_name);
    fclose(fp);
    free(file_name);
}
