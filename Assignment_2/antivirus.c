#include "antivirus.h"

struct node_1 *head_1;
struct node_2 *head_2;
struct node_3 *head_3;

const char* reports[] = {
    "REPORTED_MD5_HASH",
    "REPORTED_SH256_HASH",
    "REPORTED_VIRUS",
    "REPORTED_BITCOIN"
};

const char* result[] ={
    "Safe",
    "Malware"
};

const char* actions[] ={
    "OPENED",
    "CREATED",
    "MODIFIED",
    "DELETED"
};

void add_node_list_1(const char *path, enum report reason){
    struct node_1 *new_node = (struct node_1 *)malloc(sizeof(struct node_1));
    if(!new_node) exit(0);
    new_node->path = (char *)malloc(strlen(path) + 1);
    new_node->reason = reason;
    strcpy(new_node->path, path);
    new_node->next = head_1;
    head_1 = new_node;
}

void add_node_list_2(const char *path, const char *file, const char *domain, int is_exec, enum result result){
    struct node_2 *new_node = (struct node_2 *)malloc(sizeof(struct node_2));
    if(!new_node) exit(0);
    new_node->path = (char *)malloc(strlen(path) + 1);
    new_node->file = (char *)malloc(strlen(file) + 1);
    new_node->domain = (char *)malloc(strlen(domain) + 1);
    strcpy(new_node->path, path);
    strcpy(new_node->file, file);
    strcpy(new_node->domain, domain);
    new_node->is_exec = is_exec;
    new_node->result = result;
    new_node->next = head_2;
    head_2 = new_node;
}

void add_node_list_3(const char *filename, enum actions action){
    struct node_3 *new_node = (struct node_3 *)malloc(sizeof(struct node_3));
    if(!new_node) exit(0);
    new_node->filename = (char *)malloc(strlen(filename) + 1);
    new_node->next = NULL;
    strcpy(new_node->filename, filename);
    new_node->action = action;
    struct node_3 *cur;
    if(head_3 == NULL){
        head_3 = new_node;
        return;
    }
    cur = head_3;

    while(cur->next != NULL){
        cur = cur->next;
    }

    cur->next = new_node;
}

void print_list_1(){
    struct node_1 *current = head_1;
    while(current){
        printf("%s: %s\n", current->path, reports[current->reason]);
        current = current->next;
    }
}   

void print_list_2(){
    struct node_2 *current = head_2;
    while(current){
        if(current->is_exec == 1)
            printf("| %s | %s | %s | TRUE | %s |\n", current->file, current->path, current->domain, result[current->result]);
        else
            printf("| %s | %s | %s | FALSE | %s |\n", current->file, current->path, current->domain, result[current->result]);
        current = current->next;
    }
}

void print_list_3(){
    struct node_3 *current = head_3;
    while(current){
        printf("File: %s | Action: %s \n",current->filename, actions[current->action]);
        current = current->next;
    }
}


size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    strcat(userdata, ptr);
    return size * nmemb;
}

void remove_substring(char *str, const char *sub) {
    char *match = strstr(str, sub);
    if (match) {
        size_t sub_len = strlen(sub);
        memmove(match, match + sub_len, strlen(match + sub_len) + 1); // Include the null terminator
    }
}

int check_for_pattern(char *name){
    struct node_3 *current = head_3;
    int pattern = 0;
    int opened_flag = 0, created_flag = 0, modified_flag = 0, deleted_flag = 0;
    char *locked_name = malloc(strlen(name) + 8);    //7 for .locked + 1 for \0
    strcpy(locked_name,name);
    strcat(locked_name,".locked");
    while(current){
        if(current->action == OPENED && !strcmp(current->filename, name)){
            opened_flag = 1;
            created_flag = 0;
            modified_flag = 0;
            deleted_flag = 0;
        }
        else if(current->action == CREATED && opened_flag == 1 && !strcmp(current->filename, locked_name)){
            opened_flag = 0;
            created_flag = 1;
            modified_flag = 0;
            deleted_flag = 0;
        }
        else if(current->action == MODIFIED && created_flag == 1 && !strcmp(current->filename, locked_name)){
            opened_flag = 0;
            created_flag = 0;
            modified_flag = 1;
            deleted_flag = 0;
        }
        else if(current->action == DELETED && modified_flag == 1 && !strcmp(current->filename, name)){
            opened_flag = 0;
            created_flag = 0;
            modified_flag = 0;
            deleted_flag = 1;
        }

        if(deleted_flag == 1)
            return 1;

        current = current->next;
    }
    return 0;
}

#define BUF_LEN 1000 * (sizeof(struct inotify_event) + NAME_MAX + 1)
void monitor_dir(const char *dir_name){
    int inotify_fd, watch_fd, poll_result;
    struct inotify_event *event;
    char buffer[BUF_LEN];

    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    watch_fd = inotify_add_watch(inotify_fd, dir_name, IN_OPEN | IN_CREATE | IN_MODIFY | IN_DELETE );
    if (watch_fd == -1) {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    struct pollfd poll_fd = {inotify_fd, POLLIN, 0};

    while (1) {
        poll_result = poll(&poll_fd, 1, -1);
        if (poll_result == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        if (poll_result > 0) {
            if (poll_fd.revents & POLLIN) {
                ssize_t  num_bytes = read(inotify_fd, buffer, BUF_LEN);
                if (num_bytes == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }

                for (char *ptr = buffer; ptr < buffer + num_bytes; ptr += sizeof(struct inotify_event) + event->len) {
                    event = (struct inotify_event *) ptr;
                    if(event->mask & IN_OPEN){
                        add_node_list_3(event->name, OPENED);
                        printf("File \'%s\' was opened\n", event->name);
                    }
                    if(event->mask & IN_CREATE){
                        add_node_list_3(event->name, CREATED);
                        printf("File \'%s\' was created\n", event->name);
                    }
                    if(event->mask & IN_MODIFY){
                        add_node_list_3(event->name, MODIFIED);
                        printf("File \'%s\' was modified\n", event->name);
                    }
                    if(event->mask & IN_DELETE){
                        add_node_list_3(event->name, DELETED);
                        printf("File \'%s\' was deleted\n", event->name);
                        if(check_for_pattern(event->name) == 1) printf("[WARN] Ransomware attack detected on file %s\n",event->name);
                    }
                    if(event->mask & IN_CLOSE_WRITE){
                        printf("File \'%s\' that was opened for writing was closed\n", event->name);
                    }
                    if(event->mask & IN_CLOSE_NOWRITE){
                        printf("File \'%s\' that was not opened for writing was closed\n", event->name);
                    }
                    if(event->mask & IN_ACCESS){
                        printf("File \'%s\' was accessed\n", event->name);
                    }
                }
            }
        }
    }
    
    // Clean up
    inotify_rm_watch(inotify_fd,watch_fd);
    close(inotify_fd);
}


void check_for_malicious_domain(const char *path, char *domain){
    CURL *curl;
    CURLcode curl_result;
    long http_code = 0;
    char *url;
    char *file = strrchr(path, '/');
    int is_exec = 0;
    char response[4096] = "";

    if(file != NULL) {
        file++;
    }
    else file = (char *)path;

    char *end = strrchr(file, '.') ;
    if(end != NULL) {
        end++;  
        if(!strcmp(end, "out") || !strcmp(end, "exe"))
            is_exec = 1;
    } 
    else end = file + strlen(file);

    remove_substring(domain, "https://");
    remove_substring(domain, "http://");
    remove_substring(domain, "www.");


    url = malloc(strlen("https://family.cloudflare-dns.com/dns-query?name=") + strlen(domain) + 1);
    strcpy(url, "https://family.cloudflare-dns.com/dns-query?name=");
    strcat(url, domain);


    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "accept: application/dns-json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);


        curl_result = curl_easy_perform(curl);

        if (curl_result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(curl_result));
        }
        else{
            const char *comment_start = strstr(response, "\"Comment\":");
            if (!comment_start) {
                curl_easy_cleanup(curl);
                add_node_list_2(path, file, domain, is_exec, Safe);
                return;
            }
            
            // Find the start position of the "Censored" string within the "Comment" array
            const char *censored_start = strstr(comment_start, "Censored");
            if (!censored_start) {
                add_node_list_2(path, file, domain, is_exec, Safe);
            } else {
                add_node_list_2(path, file, domain, is_exec, Malware);
            }
        }

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize libcurl\n");
    }
    
    free(url);
}

int is_readable_char(char c) {
    // Check if the character is printable and not a space or control character
    return (c >= 32 && c <= 126);
}

int find_web_addr(const char *path){
    regex_t regex;
    regmatch_t match[2];
    int res;
    char msgbuf[100];
    char domain[512];
    // Compile the regular expression
    res = regcomp(&regex, "((https?:\\/\\/(www.)?|www.)(\\w|\\d|\\.\\w|\\-|\\/|\\?)+)", REG_EXTENDED);
    if (res) {
        fprintf(stderr, "Could not compile regex\n");
        return -1;
    }

    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        return -1;
    }

    char buffer[10000 + 1];
    int len = 0;

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (is_readable_char(c)) {
            if (len < 10000) {
                buffer[len++] = c;
            }
        } else {
            if (len > 5) {
                buffer[len] = '\0';
                // Execute the regular expression
                res = regexec(&regex, buffer, 2, match, 0);
                if (!res) {
                    snprintf(domain, sizeof(domain), "%.*s", (match[0].rm_eo - match[0].rm_so), buffer + match[0].rm_so);
                    check_for_malicious_domain(path, domain);
                } else if (res != REG_NOMATCH) {
                    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
                    regerror(res, &regex, msgbuf, sizeof(msgbuf));
                    return -1;
                }
            }
            len = 0; // Reset the length for the next string
        }
    }

    // Free the compiled regular expression
    regfree(&regex);
    fclose(file);
    return 0;
}

void inspect_dir(const char *dir_name){
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if (!(dir = opendir(dir_name))) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {

        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        size_t dir_len = strlen(dir_name);
        size_t name_len = strlen(entry->d_name);
        char *path = malloc(dir_len + 1 + name_len + 1); // 1 for '/' and 1 for null terminator
        if (!path) {
            perror("malloc");
            closedir(dir);
        }
        strcpy(path, dir_name);
        path[dir_len] = '/';
        strcpy(path + dir_len + 1, entry->d_name);

        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            inspect_dir(path);
        } else {
            find_web_addr(path);
            files_scanned++;
        }
    }
}


int check_for_MD5_hash(const char *path) {
    char *malicious_hash = "85578cd4404c6d586cd0ae1b36c98aca";

    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    int bytes_read;
    EVP_MD_CTX *md5_ctx = EVP_MD_CTX_new();
    if (!md5_ctx) {
        perror("EVP_MD_CTX_new");
        fclose(file);
        return EXIT_FAILURE;
    }

    EVP_DigestInit(md5_ctx, EVP_md5());

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        EVP_DigestUpdate(md5_ctx, buffer, bytes_read);
    }

    if (bytes_read < 0) {
        perror("fread");
        fclose(file);
        return -1;
    }

    fclose(file);

    unsigned int md5_hash_length;
    unsigned char md5_hash[EVP_MAX_MD_SIZE];

    EVP_DigestFinal(md5_ctx, md5_hash, &md5_hash_length);

    EVP_MD_CTX_free(md5_ctx);


    // Convert the MD5 hash string to binary
    unsigned char u_md5_hash[md5_hash_length];
    for(int i = 0 ; i < md5_hash_length ; i++){
        u_md5_hash[i] = 0;
    }
    assert(malicious_hash != NULL && md5_hash_length != 0) ;
    for (int i = 0; i < md5_hash_length; i++) {
        sscanf(malicious_hash + 2 * i, "%2hhx", &u_md5_hash[i]);
    }


    for (int i = 0; i < md5_hash_length; i++) {
        if (u_md5_hash[i] != md5_hash[i]) {
            return 0;
        }
    }
    return 1;

}

int check_for_SHA256_hash(const char *path) {
    char *malicious_hash = "d56d67f2c43411d966525b3250bfaa1a85db34bf371468df1b6a9882fee78849";

    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        return -1;
    }

    unsigned char buffer[BUFFER_SIZE];
    int bytes_read;
    EVP_MD_CTX *sha256_ctx = EVP_MD_CTX_new();
    if (!sha256_ctx) {
        perror("EVP_MD_CTX_new");
        fclose(file);
        return EXIT_FAILURE;
    }

    EVP_DigestInit(sha256_ctx, EVP_sha256());;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        EVP_DigestUpdate(sha256_ctx, buffer, bytes_read);
    }

    if (bytes_read < 0) {
        perror("fread");
        fclose(file);
        return -1;
    }

    fclose(file);
    unsigned int sha256_hash_length;
    unsigned char sha256_hash[EVP_MAX_MD_SIZE];

    EVP_DigestFinal(sha256_ctx, sha256_hash, &sha256_hash_length);
    EVP_MD_CTX_free(sha256_ctx);

    
    // Convert the MD5 hash string to binary
    unsigned char u_sha256_hash[sha256_hash_length];
    for(int i = 0 ; i < sha256_hash_length ; i++){
        u_sha256_hash[i] = 0;
    }
    assert(malicious_hash != NULL && sha256_hash_length != 0) ;
    for (int i = 0; i < sha256_hash_length; i++) {
        sscanf(malicious_hash + 2 * i, "%2hhx", &u_sha256_hash[i]);
    }

    for (int i = 0; i < sha256_hash_length; i++) {
        if (u_sha256_hash[i] != sha256_hash[i]) {
            return 0;
        }
    }

    return 1;
}

int check_for_virus_signature(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        return EXIT_FAILURE;
    }

   uint8_t virus_signature[16] = {
        0x98, 0x1d, 0x00, 0x00, 0xec, 0x33, 0xff, 0xff,
        0xfb, 0x06, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10
    };

    // Read the file byte by byte and search for the virus_signature
    int byte;
    int found = 0;
    long position = 0;
    while ((byte = fgetc(file)) != EOF) {
        position++;
        if (byte == virus_signature[0]) {  // Check if the first byte of the virus_signature matches
            // Read the next bytes and compare with the virus_signature
            int match = 1;
            for (int i = 1; i < 16; i++) {
                byte = fgetc(file);
                position++;
                if (byte != virus_signature[i]) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        return 0;
    }
    return 1;
}

int check_for_bitcoin_address(const char *path){
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("fopen");
        return -1;
    }

    char buffer[1000 + 1];
    int found = 0;
    int len = 0;

    int c;
    while ((c = fgetc(file)) != EOF) {
        if (is_readable_char(c)) {
            if (len < 1000) {
                buffer[len++] = c;
            }
        } else {
            // Check if the current string exceeds the minimum length
            if (len > 4) {
                buffer[len] = '\0'; // Null-terminate the string
                if (strstr(buffer, "bc1qa5wkgaew2dkv56kfvj49j0av5nml45x9ek9hz6") != NULL) {
                    found = 1;
                    break;
                }
            }
            len = 0; // Reset the length for the next string
        }
    }

    fclose(file);
    return found;
}

void scan_dir(const char *dir_name) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if (!(dir = opendir(dir_name))) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {

        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        size_t dir_len = strlen(dir_name);
        size_t name_len = strlen(entry->d_name);
        char *path = malloc(dir_len + 1 + name_len + 1); // 1 for '/' and 1 for null terminator
        if (!path) {
            perror("malloc");
            closedir(dir);
        }
        strcpy(path, dir_name);
        path[dir_len] = '/';
        strcpy(path + dir_len + 1, entry->d_name);

        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            scan_dir(path);
        } else {
            files_scanned++;
            int answer = check_for_MD5_hash(path);
            if(answer == -1) printf("Error in MD5 hash\n");
            else if(answer == 1) {
                add_node_list_1(path, REPORTED_MD5_HASH);
                malicious_files_count++;
            }
            answer = check_for_SHA256_hash(path);
            if(answer == -1) printf("Error in SHA256 hash\n");
             else if(answer == 1) {
                add_node_list_1(path, REPORTED_SHA256_HASH);
                malicious_files_count++;
            }
            answer = check_for_virus_signature(path);
            if(answer == -1) printf("Error in virus virus_signature\n");
            else if(answer == 1) {
                add_node_list_1(path, REPORTED_VIRUS);
                malicious_files_count++;
            }
            answer = check_for_bitcoin_address(path);
            if(answer == -1) printf("Error in bitcoin address\n");
            else if(answer == 1) {
                add_node_list_1(path, REPORTED_BITCOIN);
                malicious_files_count++;
            }
        }
    }
    
    closedir(dir);
}

int main(int argc, char **argv){
    pid_t pid = getpid();
    time_t now;
    struct tm *local_time;
    char time_str[20];

    time(&now);
    local_time = localtime(&now);

    // Format the time string
    strftime(time_str, sizeof(time_str), "[%d-%b-%y %H:%M:%S]", local_time);
    
    if(!strcmp(argv[1],"scan")){
        printf("[INFO] [%d] [%s] Application Started\n",pid ,time_str);
        printf("[INFO] [%d] [%s] Scanning Directory %s\n",pid , time_str, argv[2]);    
        printf("[INFO] [%d] [%s] Searching...\n",pid ,time_str);    
        
        scan_dir(argv[2]);

        time(&now);
        local_time = localtime(&now);

        strftime(time_str, sizeof(time_str), "[%d-%b-%y %H:%M:%S]", local_time);

        printf("[INFO] [%d] [%s] Operation Finished\n",pid ,time_str);    
        printf("[INFO] [%d] [%s] Processed %d files. Found %d infected\n\n\n",pid ,time_str, files_scanned,malicious_files_count);    
        print_list_1();
        
    }
    else if(!strcmp(argv[1],"inspect")){
        printf("[INFO] [%d] [%s] Application Started\n",pid ,time_str);
        printf("[INFO] [%d] [%s] Inspecting Directory %s\n",pid , time_str, argv[2]);    
        printf("[INFO] [%d] [%s] Searching...\n",pid ,time_str);

        inspect_dir(argv[2]);

        time(&now);
        local_time = localtime(&now);

        strftime(time_str, sizeof(time_str), "[%d-%b-%y %H:%M:%S]", local_time);

        printf("[INFO] [%d] [%s] Operation Finished\n",pid ,time_str);    
        printf("[INFO] [%d] [%s] Processed %d files.\n\n\n",pid ,time_str, files_scanned);
        printf("|\tFILE\t|\t\tPATH\t\t|\tDOMAIN\t|\tEXECUTABLE\t|\tRESULT\t|\n==================================================================================================================\n");
        print_list_2();   
        
    }
    else if(!strcmp(argv[1],"monitor")){
        printf("[INFO] [%d] [%s] Application Started\n",pid ,time_str);
        printf("[INFO] [%d] [%s] Monitoring Directory %s\n",pid , time_str, argv[2]);    
        printf("[INFO] [%d] [%s] Waiting for events...\n",pid ,time_str);

        monitor_dir(argv[2]);
    }
    else if(!strcmp(argv[1],"slice")){}
    else if(!strcmp(argv[1],"unlock")){}

    return 0;
}