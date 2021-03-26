//
// Created by cuong on 25/03/2021.
//

#ifndef WEB_SERVER_HASHTABLE_H
#define WEB_SERVER_HASHTABLE_H

struct hashtable{
    int size;
    int num_entries;
    float load;
    struct llist **bucket;
    int (*hashf)(void *data, int data_size, int bucket_count);
};

extern void hashtable_destroy(struct hashtable *ht);
extern void *hashtable_put(struct hashtable *ht, char *key, void *data);
extern void *hashtable_put_bin(struct hashtable *ht, void *key, int key_size,
                               void *data);
extern void *hashtable_get(struct hashtable *ht, char *key);
extern void *hashtable_get_bin(struct hashtable *ht, void *key, int key_size);
extern void *hashtable_delete(struct hashtable *ht, char *key);
extern void *hashtable_delete_bin(struct hashtable *ht, void *key,
                                  int key_size);
extern void hashtable_foreach(struct hashtable *ht, void (*f)(void *, void *),
                              void *arg);
#endif //WEB_SERVER_HASHTABLE_H
