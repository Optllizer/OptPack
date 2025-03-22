/*

Sample HTTP Download Source

Author: teknecal
Created: 15/08/2009

Using some source from ftpii v0.0.5
ftpii Source Code Copyright (C) 2008 Joseph Jordan <joe.ftpii@psychlaw.com.au>

*/
#include <errno.h>
#include <malloc.h>
#include <math.h>
#include <network.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <time.h>
#include <wiiuse/wpad.h>
#include <unistd.h>
#include "common.h"
#include <sys/stat.h>


#define MAX_FILES 400
#define MAX_FILENAME_LENGTH 100

s32 main_server;

void debug(char* in) {
    int x = 0;
    for (; x < strlen(in); x++) {
        //printf("\n", in[x], in[x]);
    }
}

int get_version(char* version_file) {
  FILE* f;

  size_t buff_size = 16;
  char* buff;
  buff = malloc(buff_size);

  f = fopen(version_file, "r");

  if (f == NULL) {
    return 0;
  }

  fgets(buff, buff_size, f);
  //printf("Received line: '%s'\n", buff);
  //printf("\n", buff);

  return atoi(buff);
}

int update_local_version(char* version_file, int new_version) {
//return 0;
  FILE* f;
  f = fopen(version_file,"w+");

  fprintf(f, "%d", new_version);

  fclose(f);

  return 0;
}

int get_latest_version(s32* server) {
  // s32 version_server = server_connect();
  printf("\nGetting latest available version.\n");

  FILE* fp = fopen("remote_version.txt", "wb");

  char http_request2[1000];
  strcpy(http_request2, "GET /optpack/update1/last_version.txt");
  strcat(http_request2, " HTTP/1.0\r\nHost: www.codemii.com\r\nCache-Control: no-cache\r\n\r\n");

  write_http_reply(*server, http_request2);
  int result = request_file(*server, fp);

  fclose(fp);
  net_close(*server);
  if (result) {
    printf("Succesfully obtained latest version.\n");
	printf("\n");
    return 1;
  } else {
    printf("Failed to download latest version. Error:92\n");
	printf("\n");
    return 0;
  }
}

void get_filenames(s32* server, int version) {
    int result = 0;
  
  char test[100]="download_list.txt";
  char* tmp;
  
  tmp = malloc(sizeof(char) * 128);
  snprintf(tmp, sizeof(char) * 128, "%d/%s", version, test);

	//printf("Getting download list from: '%s'\n", tmp);
    //printf("Setting up your OptPack to update.\n");
    //printf("\n");
    char list[100]="sd:/optpack/download_list.txt";
    FILE *f = fopen(list, "wb");

    if (f == NULL) {
        fclose(f);
        die("There was a problem creating/accessing the update. Please contact Optllizer in the OptPack discord. Error:115\n");
    }

    s32 filename_server;
    filename_server = server_connect();

    //printf("Downloading update information from the server...\n");

    char http_request[1000];
    strcpy(http_request, "GET /optpack/update1/");
    strcat(http_request, tmp);
    strcat(http_request, " HTTP/1.0\r\nHost: www.codemii.com\r\nCache-Control: no-cache\r\n\r\n");

    write_http_reply(filename_server, http_request);
    result = request_file(filename_server, f);

    fclose(f);

    if (result) {
        //printf("\nSuccessfully downloaded update information.\n");
    } else {
        printf("\nFailed to setup OptPack to update. Error:136\n");
        printf("\nPlease contact Optllizer in the OptPack discord.\n");
    }

    //printf("Finished setting up OptPack to update.\n");
	printf("\n");
    net_close(filename_server);
    sleep(3);

}

void load_filenames(char** arr, int* cntr, int version) {
    // Load line by line into arr 
    // 400 strings / 100 characters max (see #defines ^)
    
    // Allocate memory for the array
    //arr = (char**) malloc(MAX_FILES * sizeof(char*));
    int y = 0;
    for (y = 0; y < MAX_FILES; y++) {
        arr[y] = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
        memset(arr[y], 0, MAX_FILENAME_LENGTH*sizeof(char));
    }

    char list[100]="sd:/optpack/download_list.txt";
    FILE *f = fopen(list, "r");
    char buffer[MAX_FILENAME_LENGTH];
    int x = 0;
	while(fgets(buffer, MAX_FILENAME_LENGTH, f)) {
        //printf("%s\n", buffer);
        buffer[strcspn(buffer, "\r")] = 0;
        buffer[strcspn(buffer, "\n")] = '\0';
		char* tmp;
		tmp = malloc(sizeof(char) * 128);
		snprintf(tmp, sizeof(char) * 128, "%d/%s", version, buffer);
        strcpy(arr[x], tmp);
        x++;
		*cntr = x;       
		free(tmp);
    }
    //printf("\nDownloading updates please wait...\n");
    //printf("This may take a while.\n");  
}

void copy_file() {
    char *src = "sd:/optpack/Settings.pul";
    char dest_base[] = "sd:/optpack/binaries/Settings";
    char dest[200];  // Buffer to hold the destination file name

    FILE *source, *target;
    char buffer[128];  // Buffer to hold file content

    // Open the source file
    source = fopen(src, "r");
    if (source == NULL) {
        //perror("Error opening source file");
        return;
    }

    // Generate a unique destination file name
    int suffix = 1;
    do {
        snprintf(dest, sizeof(dest), "%s_%d.pul", dest_base, suffix);
        FILE *check = fopen(dest, "r");
        if (check) {
            fclose(check);  // File exists, increment suffix
            suffix++;
        } else {
            break;  // File does not exist, use this name
        }
    } while (1);

    // Open the target file for writing
    target = fopen(dest, "w");
    if (target == NULL) {
        //perror("Error opening destination file");
        fclose(source);
        return;
    }

    // Copy the content from source to target
    size_t n, m;
    while ((n = fread(buffer, 1, sizeof buffer, source)) > 0) {
        m = fwrite(buffer, 1, n, target);
        if (m != n) {
            //perror("Error writing to destination file");
            break;
        }
    }

    if (n != 0) {
        //perror("Error reading from source file");
    }

    fclose(source);
    fclose(target);

    //printf("Successfully copied to: %s\n", dest);
}


void delete_files(s32* server, int version) {
    int result = 0;

    // Path to the delete list
    char delete_list[100] = "delete_list.txt";
    char* tmp;

    // Allocate memory for constructing the full path
    tmp = malloc(sizeof(char) * 128);
    snprintf(tmp, sizeof(char) * 128, "%d/%s", version, delete_list);

    // Define the local path for storing delete_list.txt
    char list[100] = "sd:/optpack/delete_list.txt";
    FILE *f = fopen(list, "wb");

    if (f == NULL) {
        fclose(f);
        die("There was a problem creating/accessing the delete list. Please contact Optllizer in the OptPack discord. Error:115\n");
    }

    s32 filename_server;
    filename_server = server_connect();

    // Construct the HTTP request for delete_list.txt
    char http_request[1000];
    strcpy(http_request, "GET /optpack/update1/");
    strcat(http_request, tmp);
    strcat(http_request, " HTTP/1.0\r\nHost: www.codemii.com\r\nCache-Control: no-cache\r\n\r\n");

    write_http_reply(filename_server, http_request);
    result = request_file(filename_server, f);

    fclose(f);

    if (!result) {
        printf("\nFailed to download delete_list.txt. Error:136\n");
        printf("\nPlease contact Optllizer in the OptPack discord.\n");
        free(tmp);
        return;
    }

    net_close(filename_server);
    free(tmp);

    // Now process the delete list and delete files
    f = fopen(list, "r");
    if (f == NULL) {
        die("Unable to open delete_list.txt for reading.\n");
    }

    char buffer[MAX_FILENAME_LENGTH];
    while (fgets(buffer, MAX_FILENAME_LENGTH, f)) {
        // Strip newline characters
        buffer[strcspn(buffer, "\r")] = 0;
        buffer[strcspn(buffer, "\n")] = '\0';

        // Construct the full path to the file
        char full_path[200] = "sd:/";
        strcat(full_path, buffer);

        // Attempt to delete the file
        if (remove(full_path) == 0) {
            //printf("Deleted file: %s\n", full_path);
        } else {
            //printf("Failed to delete file: %s\n", full_path);
        }
    }

    fclose(f);

    //printf("\nFinished processing delete list.\n");
}

void print_motd(s32* server) {
    char filename[100] = "motd.txt";
    char* tmp;
    tmp = malloc(sizeof(char) * 128);
    snprintf(tmp, sizeof(char) * 128, "%s", filename);

    char list[100] = "sd:/optpack/motd.txt";
    FILE *f = fopen(list, "wb");

    if (f == NULL) {
        fclose(f);
        printf("There was a problem creating/accessing the file. Error:115\n");
        return;
    }

    s32 filename_server;
    filename_server = server_connect();

    char http_request[1000];
    strcpy(http_request, "GET /optpack/update1/");
    strcat(http_request, tmp);
    strcat(http_request, " HTTP/1.0\r\nHost: www.codemii.com\r\nCache-Control: no-cache\r\n\r\n");

    write_http_reply(filename_server, http_request);
    int result = request_file(filename_server, f);

    fclose(f);

    if (result) {
        //printf("\nSuccessfully downloaded MOTD.\n");
    } else {
        printf("\nFailed to download MOTD. Error:136\n");
        return;
    }

    net_close(filename_server);
    sleep(3);

    // Now print the MOTD
    FILE *file = fopen(list, "r");
    if (file == NULL) {
        printf("Cannot open file\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}


static void _mkdir(const char *dir) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
                tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
                if(*p == '/') {
                        *p = 0;
                        mkdir(tmp, S_IRWXU);
                        *p = '/';
                }
        mkdir(tmp, S_IRWXU);
}

double get_file_size(char* filename) {
    struct stat st;
    if(stat(filename, &st) != 0) {
        return -1;  // stat failed
    }
    return (double)st.st_size;
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

typedef struct {
    double size;
    double speed;
} DownloadInfo;


DownloadInfo download(char* filename) {
    // download filename (filename includes path)
    char http_request[1000];
    char file[MAX_FILENAME_LENGTH] = "sd:";
    char directory[MAX_FILENAME_LENGTH];
    memset(directory, '\0', sizeof(directory));
    int i; 
    for(i=0; i < strlen(filename); i++){
        if (filename[i] == '/'){
            break;
        }
    }
    strcat(file, filename+i);
    int j;
    int lastindex=0;
    for(j=0; j < strlen(file); j++){
        if (file[j] == '/'){
            lastindex=j;
        }
    }
    strncpy(directory, file, lastindex);
    _mkdir(directory);
    FILE *f = fopen(file, "wb");

    if (f == NULL) {
        printf("\n Error downloading update: %s \n full file \"%s\"\n", strerror(errno), file);
        printf("Please contact Optllizer in the OptPack discord. Error:227/n");
        fclose(f); 
        die("");  
    }

    s32 dl_server;
    dl_server = server_connect();

    strcpy(http_request, "GET /optpack/update1/");
    strcat(http_request, filename);
    strcat(http_request, " HTTP/1.0\r\nHost: www.codemii.com\r\nCache-Control: no-cache\r\n\r\n");

    write_http_reply(dl_server, http_request);

	double start_time = get_time();
    int result = request_file(dl_server, f);
    double end_time = get_time();

    // Calculate the download speed
    double file_size = get_file_size(file); 
    double file_size_kb = file_size / 1024; // convert to KB
    double download_time = end_time - start_time;
    double download_speed = file_size_kb / download_time; // in KB/s

    fclose(f);
    net_close(dl_server);

    DownloadInfo info;
    info.speed = download_speed;

    if(result) { 
        double file_size = get_file_size(file);  // You need to implement this function
        double file_size_mb = file_size / (1024 * 1024);  // File size in megabytes

        info.size = file_size_mb;
    } else { 
        printf("Update Failed, Please try again. Error:255\n"); 
        info.size = -1.0;
    }
    sleep(3);

    return info;
}

void mainloop() {
    int x = 0;
    char** filename_arr;
    int filename_cntr = 0;

    printf("\nConnecting to the main update server...\n");
    printf("\nIf this process takes longer than 30 seconds, most likely the updater failed\nto connect to the main update server.\n");
    printf("\nPlease close the updater by pressing home and try the backup updater.\n");
    printf("\nIf the updater connects, ignore the previous messages.\n");

    main_server = server_connect();

    printf("\nConnected to the update server.\n");
    //printf("\n");
    // Local version
    int local_v = get_version("sd:/apps/optpack/version.txt");
    int fix_v = local_v - 1;
    // Remote 
    get_latest_version(&main_server);
    int remote_v = get_version("remote_version.txt");

    printf("Your Version: 1.%d | Latest Version: 1.%d \n", fix_v, remote_v);
	printf("\n");
	print_motd(&main_server);
	printf("\n");

    while (local_v <= remote_v) {
        get_filenames(&main_server, local_v);

        filename_arr = (char **) malloc(MAX_FILES * sizeof(char *));
        load_filenames(filename_arr, &filename_cntr, local_v);

        char tmp[128];
        int len;
        printf("Starting download of Version 1.%d files \n", local_v);
        printf("\n");

for (x = 0; x < filename_cntr; x++) {
    DownloadInfo info = download(filename_arr[x]); // store download info
    double download_speed = info.speed;
    double file_size_kb = info.size * 1024; // convert MB to KB
	
    if (file_size_kb < 999) {
        len = sprintf(tmp, "Downloading files: [%d/%d] [%.2f KB] [%.2f KB/s]       ", x, filename_cntr, file_size_kb, download_speed);
    } else {
        len = sprintf(tmp, "Downloading files: [%d/%d] [%.2f MB] [%.2f KB/s]       ", x, filename_cntr, info.size, download_speed);
    }
            printf("%s", tmp);
            fflush(stdout);

            for (int y = 0; y < len + 1; y++) {
                printf("\r");
            }
        }

        printf("\rDownload complete.                                                                                                                               \n");
        update_local_version("sd:/apps/optpack/version.txt", local_v);
        local_v++;
    }
    update_local_version("sd:/apps/optpack/version.txt", local_v);
	
	// Delete files
    printf("\nCleaning up...\n");
    delete_files(&main_server, remote_v);
    net_close(main_server);
    free(filename_arr);
	printf("\nBacking up Settings.pul...\n");
	copy_file();
	printf("Successfully backed up Settings.pul");
    printf("\n\nUpdates Finished.\n\n");
    printf("\nFor changelog, please visit the OptPack wiki page or the OptPack discord.\n");
    printf("\nUpdater will close in 10 seconds.\n\n");
    sleep(10);
}
int main(int argc, char **argv) {
  initialise();
  printf("\x1b[2;0H");
  printf("OptPack Updater\n");
  printf("Version 1.7\n");
  //printf("This backup updater should only be used if the main OptPack Updater is down.\n");
  //printf("If you launched this updater by mistake, please press the home button now.\n");
  if (initialise_reset_button()) {
    printf("To exit, press the home button.\n\n");
    printf("Please do not turn off your system till the process is completed.\n\n");
    printf("By using this updater, you agree that you are liable for any damage done to\n");
    printf("your Wii/Wii U and/or SD CARD.\n\n");
    printf("\nYou have 5 seconds to cancel.\n\n");
    sleep(5);
  } else {
    printf("Unable to exit - hold down the power button to exit. Error:393\n");
  }
  initialise_network();
	
	mainloop();
	//changelog();

	return 0;
}