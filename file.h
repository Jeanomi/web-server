//
// Created by cuong on 26/03/2021.
//

#ifndef WEB_SERVER_FILE_H
#define WEB_SERVER_FILE_H

struct file_data{
    int size;
    void *data;
};

extern struct file_data *file_load(char *filename);
extern void file_free(struct file_data *filedata);

#endif //WEB_SERVER_FILE_H
