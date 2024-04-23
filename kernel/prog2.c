#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory> <date>\n", argv[0]);
        return 1;
    }
    
    // Open directory
    DIR *dir = opendir(argv[1]);
    if (!dir) {
        fprintf(stderr, "Could not open directory '%s'\n", argv[1]);
        return 1;
    }
    
    // Parse date argument
    struct tm date;
    if (strptime(argv[2], "%Y-%m-%d", &date) == NULL) {
        fprintf(stderr, "Invalid date format (should be 'YYYY-MM-DD')\n");
        return 1;
    }
    time_t target_time = mktime(&date);
    
    // Traverse directory
    struct dirent *entry;
    struct stat st;
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        
        // Construct full path
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/%s", argv[1], entry->d_name);
        
        // Get file stats
        if (stat(path, &st) == -1) {
            fprintf(stderr, "Could not stat file '%s'\n", path);
            continue;
        }
        
        // Check if file was created after target date
        if (st.st_mtime > target_time) {
            // Truncate file to half its original size
            off_t new_size = st.st_size / 2;
            if (truncate(path, new_size) == -1) {
                fprintf(stderr, "Could not truncate file '%s'\n", path);
                continue;
            }
            printf("Truncated file '%s' to %lld bytes\n", path, (long long)new_size);
        }
    }
    
    // Close directory
    closedir(dir);
    
    return 0;
}