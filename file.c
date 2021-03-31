//
// Created by cuong on 26/03/2021.
//

#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct file_data *file_load(char *filename){
    char *buffer, *p;
    //const char delim[2] = "/";
    struct stat buf;
    int bytes_read, bytes_remaining, total_bytes = 0;

//    filename = strtok(file, "\0");
//    filename = strtok(NULL, "/");

    printf("%s\n", filename);
    //Get the file size
    if (stat(filename, &buf) == -1){
        return NULL;
    }

    //Make sure it's a regular file
    if (!(buf.st_mode & S_IFREG)){
        return NULL;
    }

    //Open the file for reading
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL){
        return NULL;
    }
    printf("pass fp = NULL\n");
    //Allocate that many bytes
    bytes_remaining = buf.st_size;
    p = buffer = malloc(bytes_remaining);

    if (buffer == NULL){
        return NULL;
    }

    //Read in the entire file
    while (bytes_read = fread(p, 1, bytes_remaining, fp),
            bytes_read != 0 && bytes_remaining > 0){
        if (bytes_read == -1){
            free(buffer);
            return NULL;
        }
        bytes_remaining -=bytes_read;
        p += bytes_read;
        total_bytes +=bytes_read;
    }

    printf("pass read = NULL\n");
    //Allocate the file data struct
    struct file_data *filedata = malloc(sizeof *filedata);

    if (filedata == NULL){
        free(buffer);
        return NULL;
    }

    filedata->data = buffer;
    filedata->size = total_bytes;

    return filedata;
}

//Free memory allocated by file_load()
void file_free(struct file_data *filedata){
    free(filedata->data);
    free(filedata);
}