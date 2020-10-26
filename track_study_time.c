/*
Made by: Jordan Winkler
Sat Oct 24 13:24:06 EDT 2020

A simple program to track study time, so it can be parsed by
org mode in emacs.

To compile:
    This lang is gnu C. Use gcc to compile it. Designed to run
    on debain based distros. Will also run on WSL.

Use:
    - Use without arguments to get an hr:min:sec timer.
    - Use with name of study topic to get a named timer
      in a timestamped csv file..
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
    char* el_file_name;
    char* csv_file_name;
    if(argc == 2)
    {
        // random juggling to make name
        struct tm* timeinfo;
        timeinfo = localtime(&start_time);
        char buffer[80];
        strftime(buffer, 80, "_%A:%Y:%j:%X", timeinfo);

        file_name = concat(argv[1], buffer);
        el_file_name = concat(file_name, ".el"); // emacs lisp file
        csv_file_name = concat(file_name, ".csv"); // csv file
        printf("tracking: %s\n", argv[1]);
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

    // C-z to stop display, C-c to kill program
    while(1)
    {
        current_time = time(NULL);
        diff = difftime(current_time, start_time);

        if(argc == 2)
        {
            // to print to screen
            //{
            /*
            system("clear"); // portable screen clearing is a pain
            // screen print
            printf(
                "[%s]%i:%i:%i\n", argv[1],
                hours(diff),
                minutes(diff),
                seconds(diff));
            */
            //}

            // csv file print
            //{
            fp = fopen(csv_file_name, "w+");
            fprintf(fp,
                    "%s,%i,%i,%i\n", 
                    argv[1],
                    hours(diff),
                    minutes(diff),
                    seconds(diff));
            fclose(fp); // bit of processor waste
            //}

            // lisp file print
            //{
            fp = fopen(el_file_name, "w+");
            fprintf(fp,
                    "(+ %i (/ %i 60.0))\n", 
                    //argv[1],
                    hours(diff),
                    minutes(diff)
                    //seconds(diff)
                    );
            fclose(fp); // bit of processor waste
            //}
        }
        else
            printf("%i:%i:%i\n",
                   hours(diff),
                   minutes(diff),
                   seconds(diff));
        sleep(1);
    }

    // clean up, that never happens
    free(file_name);
    fclose(fp);
    free(file_name);
}
