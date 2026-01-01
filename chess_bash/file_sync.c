#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_PATH 1024
#define MAX_FILES 100

// Function to check if a file exists
int file_exists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

// Function to copy a file using fork and exec
void copy(char *src, char *dest) {
    pid_t pid = fork();
    if (pid == 0 ) {
        execl("/bin/cp", "cp", src, dest, NULL);
        perror("execl");
        exit(1); 
    }
    wait(NULL);
    printf("Copied: %s -> %s\n", src, dest);
}

// Function to check if two files are different using diff
int diff_check(const char *src, const char *dest) {
    pid_t pid = fork();
    if (pid == 0) {
        int null_fd = open("/dev/null", O_WRONLY);
        dup2(null_fd, STDOUT_FILENO);
        close(null_fd);
        execl("/usr/bin/diff", "diff", "-q", src, dest, NULL);
        perror("execl diff");
        exit(1);
    }
    // This function returns 0 if files are identical, 1 if they are different
    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int main(int argc, char *argv[]) {
    //Print the current working directory
    char cwd[MAX_PATH];
    getcwd(cwd, sizeof(cwd));
    printf("Current working directory: %s\n", cwd);

    // Check if the correct number of arguments is provided
    if (argc != 3) {
        printf("Usage: file_sync <source_directory> <destination_directory>\n");
        return 1;
    }

    // Extract source and destination directories from command line arguments
    char *source = argv[1];
    char *dest = argv[2]; 

    char src_path[MAX_PATH], dest_path[MAX_PATH];
    realpath(source, src_path);
    realpath(dest, dest_path);

    // Check if source directory valid
    struct stat st;
    if (stat(source, &st) != 0) {
        printf("Error: Source directory '%s' does not exist.", source);
        return 1;
    }

    // Check if destination directory valid
    if (stat(dest, &st) != 0) {
        if (fork() == 0) {
            // Create destination directory if it doesn't exist
            execl("/bin/mkdir", "mkdir", "-p", dest, NULL);
            perror("mkdir");
            exit(1);
        }
        wait(NULL);
        printf("Created destination directory '%s'.\n", dest);
        realpath(dest, dest_path);
    }

    // Open the source directory
    DIR *src_dir = opendir(source);
    if (src_dir == NULL) {
        perror("opendir");
        return 1;
    }

    // Read the contents of the source directory
    struct dirent *entry;
    char *files[MAX_FILES];
    int file_count = 0;

    // Loop through the directory entries
    while ((entry = readdir(src_dir)) != NULL) {
        // Skip the current and parent directory entries
        if (entry->d_name[0] != '.') {
            char full_path[MAX_PATH];
            snprintf(full_path, sizeof(full_path), "%s/%s", source, entry->d_name);
            // Check if the entry is a regular file
            if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
                files[file_count] = strdup(entry->d_name);
                file_count++;
            }
        }
    }
    closedir(src_dir);

    // Sort files alphabetically
    for (int i = 0; i < file_count - 1; i++) {
        for (int j = i + 1; j < file_count; j++) {
            if (strcmp(files[i], files[j]) > 0) {
                char *tmp = files[i];
                files[i] = files[j];
                files[j] = tmp;
            }
        }
    }

    printf("Synchronizing from %s to %s\n", src_path, dest_path);

    // Loop through the files and check for differences
    for (int i = 0; i < file_count; i++) {
        char full_src[MAX_PATH], full_dest[MAX_PATH];
        snprintf(full_src, sizeof(full_src), "%s/%s", src_path, files[i]);
        snprintf(full_dest, sizeof(full_dest), "%s/%s", dest_path, files[i]);

        // Check if the file does not exist in the destination directory
        if (!file_exists(full_dest)) {
            printf("New file found: %s\n", files[i]);
            // Copy the file to the destination directory
            copy(full_src, full_dest);
        } 
        else {
        // Compare the files using diff
            int difference = diff_check(full_src, full_dest);
            if (difference) {
                struct stat src_stat, dest_stat;
                stat(full_src, &src_stat);
                stat(full_dest, &dest_stat);
                // Check if the source file is newer than the destination file, if so, copy it
                if (src_stat.st_mtime > dest_stat.st_mtime) {
                    printf("File %s is newer in source. Updating...\n",  files[i]);
                    copy(full_src, full_dest);
                } else {
                    // If the destination file is newer, skip it
                    printf("File %s is newer in destination. Skipping...\n",  files[i]);
                }
            } else {
                // If the files are identical, skip it
                printf("File %s is identical. Skipping...\n",  files[i]);
            }
        }

        // Free the allocated memory for the file name
        free(files[i]);
    }

    printf("Synchronization complete.\n");
    return 0;
}
