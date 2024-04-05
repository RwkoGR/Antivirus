#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <assert.h>
#include <unistd.h>
#include <regex.h>
#include <curl/curl.h>
#include <sys/inotify.h>
#include <poll.h>
#include <errno.h>
#define BUFFER_SIZE 4096

int files_scanned;
int malicious_files_count;

enum report{
    REPORTED_MD5_HASH,
    REPORTED_SHA256_HASH,
    REPORTED_VIRUS,
    REPORTED_BITCOIN
};

enum result{
    Safe,
    Malware
};

enum actions{
    OPENED,
    CREATED,
    MODIFIED,
    DELETED
};


struct node_1{
    char *path;
    enum report reason;
    struct node_1 *next;
};

struct node_2{
    char *path;
    char *file;
    char *domain;
    int is_exec;
    enum result result;
    struct node_2 *next;
};

struct node_3{
    char *filename;
    enum actions action;
    struct node_3 *next;
};

void add_node_list_1(const char *path, enum report reason);
void add_node_list_2(const char *path, const char *file, const char *domain, int is_exec, enum result result);
void add_node_list_3(const char *filename, enum actions action);
void print_list_1();
void print_list_2();
void print_list_3();