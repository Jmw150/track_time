// code to track a work day

#include <stdio.h>   // io
#include <dirent.h>  // directory manipulations
#include <sys/types.h>
#include <sys/stat.h> // typical bash commands
#include <stdlib.h>  // malloc and free
#include <unistd.h>  // constants and types
#include <time.h>    // computer time
#include <string.h>  // string parsing functions
#include <iso646.h>  // predicate shorthand
#include <errno.h>   // error codes

// helper functions
// {

int make_directory(char* filename)
{
    return mkdir(filename, 0700);
}

int isdir(char* file_name)
{
    DIR* dir = opendir(file_name);

    if(dir)
    {
        closedir(dir);
        return 1;
    }
    else if(ENOENT == errno)
    {
        return 0;
    }
    else     // failed for some reason
    {
        return -1;
    }

    return 0;
}

// garbage collector
// {

// TODO: figure out why standard C functions are not working with the garbage collector
// or
//   TODO: make more portable versions of each of the C standard functions

// TODO: make number some register multiple
#define MEM_SIZE 500

char _gc_mem[MEM_SIZE];
int _mem_index = 0;
void* gc_malloc(size_t n)
{
    // location of free memory left
    if(n <= (MEM_SIZE - _mem_index))
    {
        _mem_index += n;
        return (void*) & (_gc_mem[n]);
    }
    else
    {
        return NULL;
    }
}

void gc_free()
{
    _mem_index = 0;
}

// }

void* safe_gc_malloc(size_t n)
{
    void* p = gc_malloc(n);
    if(p == NULL)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", n);
        abort();
    }
    return p;
}

void* safe_malloc(size_t n)
{
    void* p = malloc(n);
    if(p == NULL)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", n);
        abort();
    }
    return p;
}

// contains a malloc. Free the return after
char* concat(const char* s1, const char* s2)
{
    // +1 for the null-terminator
    char* result = safe_malloc(strlen(s1) + strlen(s2) + 1);

    // printf("strlen s1: %ld\n", strlen(s1));
    strcpy(result, s1);
    strcat(result, s2);

    return result;
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


// }

int main(int argc, char** argv)
{
    // TODO: get this to parse on start, make portable

    // declarations
    #define HOME "/home/jordan/backup/code/"
    time_t start_time = time(NULL);
    time_t current_time = time(NULL);
    double diff; // diff of start of program to call
    FILE* fp; // for potential csv file later

    char* file_name; // general name
    char* data_file_name; // general name
    char* el_file_name;  // extension added
    char* csv_file_name; // comma separated values
    char* py_file_name; // python

    // if study type was not given, do not save data
    // TODO: parse input for acceptable forms
    if(argc == 2)
    {
        char* name = argv[1];

        {
            char buffer[80];

            // some pointer juggling to make the name
            struct tm* timeinfo = localtime(&start_time);
            strftime(buffer, 80, "_%A:%Y:%j:%X", timeinfo);

            file_name = concat(name, buffer);
        }

        // make sure file exists
        if(not isdir(HOME "data"))
        {
            make_directory(HOME "data");
        }

        data_file_name = concat(HOME "data/",
                                file_name); // storing it in a folder
        el_file_name = concat(data_file_name,
                              ".el"); // emacs lisp file
        csv_file_name = concat(data_file_name, ".csv"); // csv file
        py_file_name = concat(data_file_name, ".py"); // csv file

        // leave a prompt so the user knows what is being tracked
        printf("tracking: %s\n", argv[1]);
    }


    // C-z to stop display, C-c to kill program
    // loop timediff calculation, and printing
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
            
            // python file print
            //{
            fp = fopen(py_file_name, "w+");
            fprintf(fp,
                    "{ \"filename\" : \"%s\", \"hours\" : %i, \"minutes\" : %i, \"seconds\" : %i }\n",
                    file_name,
                    hours(diff),
                    minutes(diff),
                    seconds(diff)
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

    // clean up, that never happens, but good practice
    // TODO: make queue to free at the end
    free(file_name);
    free(el_file_name);
    free(csv_file_name);
    free(py_file_name);
    fclose(fp);
    free(file_name);
    return 0;
}

