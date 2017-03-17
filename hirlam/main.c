#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#ifdef _WIN32
    #define WGRIB_COMMAND "wgrib -s"
    #define SEPARATOR "\\"
#else
    #define WGRIB_COMMAND "./wgrib -s"
    #define SEPARATOR "/"
#endif
#define TMP_DIR "txt"
#define FULL_FILENAME_SIZE 256
#define FILENAME_SIZE 40
#define COMMAND_SIZE 128
#define BUFFER_SIZE 256
#define DATE_SIZE 16
#define NUM_FILES 300000
#define UGRD_VALUE "UGRD:10 m above gnd"
#define VGRD_VALUE "VGRD:10 m above gnd"
#define PRES_VALUE "PRES:0 m above MSL"

int convert_to_txt(char *input_dirname, char *output_dirname) {
    struct dirent *pDirent;
    DIR *pDir;
    char *relative_file_name;
    char input_filename[FULL_FILENAME_SIZE];
    char output_filename[FULL_FILENAME_SIZE];
    char command[COMMAND_SIZE];

    pDir = opendir(input_dirname);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", input_dirname);
        return -1;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        relative_file_name = pDirent->d_name;
        if (!strcmp (relative_file_name, "."))
            continue;
        if (!strcmp (relative_file_name, ".."))
            continue;
        if (!strcmp (relative_file_name, TMP_DIR))
            continue;

        sprintf(input_filename, "%s%s%s", input_dirname, SEPARATOR, relative_file_name);

        sprintf(output_filename, "%s%s%s.txt", output_dirname, SEPARATOR, relative_file_name);

        sprintf(command, "%s %s > %s", WGRIB_COMMAND, input_filename,output_filename);

        system(command);
    }
    closedir(pDir);
    return 0;

}

char current_date[DATE_SIZE] = "";
int flags[61] = {0};

int check_forecasts() {
    int counter;
    for (counter = 0; counter < 61; counter++) {
        if (flags[counter] < 1)
            printf("Missing file #%d with date %s\n", counter, current_date);
        if (flags[counter] > 1)
            printf("Several files with #%d with date %s\n", counter, current_date);
    }
    return 0;
}

int check_date(char *date, char *filename_fcst_pointer) {
    int num = atoi(filename_fcst_pointer);

    if (strcmp(current_date, "") == 0)
        strcpy(current_date, date);
    if (strcmp(current_date, date) != 0) {
        check_forecasts();
        memset(flags, 0, sizeof(flags));
        strcpy(current_date, date);
    }
    if (num <= 60)
        flags[num]++;
    else
        printf("Forecast value is out of range [0,60]. Value = %s \n", filename_fcst_pointer);

    return 0;
}


int check_grib(char *filename) {
    char buffer[BUFFER_SIZE] = {0};
    FILE *fp;
    int i;
    int pressure_records;
    int ugrd_records;
    int vgrd_records;
    char *hirlam;
    char filename_date_substr[DATE_SIZE] = {0};
    char filename_fcst_substr[DATE_SIZE] = {0};
    char filename_fcst[DATE_SIZE] = {0};
    char *filename_fcst_pointer;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf ("Cannot open file '%s'\n", filename);
        return -1;
    }

    hirlam = strstr(filename, "HIRLAM_C11_");
    strncpy(filename_date_substr, hirlam + 13, 8);
    strncpy(filename_fcst, hirlam + 25, 2);
    filename_fcst_pointer = filename_fcst;
    if (filename_fcst_pointer[0] == '0')
        filename_fcst_pointer++;
    sprintf(filename_fcst_substr, "%shr fcst", filename_fcst_pointer);
    check_date(filename_date_substr, filename_fcst_pointer);

    i = pressure_records = ugrd_records = vgrd_records = 0;
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        i++;
        if (strstr(buffer, UGRD_VALUE) != NULL)
            ugrd_records++;
        if (strstr(buffer, VGRD_VALUE) != NULL)
            vgrd_records++;
        if (strstr(buffer, PRES_VALUE) != NULL)
            pressure_records++;
        if (strstr(buffer, filename_date_substr) == NULL)
            printf("Wrong date in file %s (Record #%d)\n", filename, i);
        if (!strcmp (filename_fcst_pointer, "0")) {
            if (strstr(buffer, "anl") == NULL)
                printf("Wrong forecast value in file %s (Record #%d)\n", filename, i);
        } else {
            if (strstr(buffer, filename_fcst_substr) == NULL)
                printf("Wrong forecast value in file %s (Record #%d)\n", filename, i);
        }
    }

    if (i != 3)
        printf("GRIB file %s should contain 3 records. Number of records : %d\n", filename, i);
    if (ugrd_records != 1)
        printf("UGRD record not found in file %s\n", filename);
    if (vgrd_records != 1)
        printf("VGRD record not found in file %s\n", filename);
    if (pressure_records != 1)
        printf("PRES record not found in file %s\n", filename);

    fclose(fp);
    return 0;
}

int compare_filenames (const void * a, const void * b) {
   char *f1 = *(char **)a;
   char *f2 = *(char **)b;
   return strcmp(f1, f2);
}

int check_grib_files(char *output_dirname) {
    struct dirent *pDirent;
    DIR *pDir;
    char *relative_file_name;
    char input_filename[FULL_FILENAME_SIZE];
    char **filenames;
    int i;
    int filenames_size;
    int t;

    pDir = opendir(output_dirname);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", output_dirname);
        return -1;
    }

    filenames = (char **) malloc(NUM_FILES * sizeof(char *));
    for (i = 0; i < NUM_FILES; i++) {
        filenames[i] = (char *) malloc(FILENAME_SIZE * sizeof(char));
    }

    i = 0;
    while ((pDirent = readdir(pDir)) != NULL) {
        relative_file_name = pDirent->d_name;
        if (!strcmp (relative_file_name, "."))
            continue;
        if (!strcmp (relative_file_name, ".."))
            continue;
        strcpy(filenames[i++], relative_file_name);
    }
    filenames_size = i;

    qsort(filenames, filenames_size, sizeof(char *), compare_filenames);

    for (t = 0; t < filenames_size; t++) {
        sprintf(input_filename, "%s%s%s", output_dirname, SEPARATOR, filenames[t]);
        check_grib(input_filename);
    }
    check_forecasts(current_date);

    closedir(pDir);
    for (i = 0; i < NUM_FILES; i++) {
        free(filenames[i]);
    }
    free(filenames);
    return 0;
}

int delete_txt_files(char *output_dirname) {
    struct dirent *pDirent;
    DIR *pDir;
    char *relative_file_name;
    char input_filename[FULL_FILENAME_SIZE];
    pDir = opendir(output_dirname);
    if (pDir == NULL) {
        printf ("Cannot open directory '%s'\n", output_dirname);
        return -1;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
        relative_file_name = pDirent->d_name;
        if (!strcmp (relative_file_name, "."))
            continue;
        if (!strcmp (relative_file_name, ".."))
            continue;
        sprintf(input_filename, "%s%s%s", output_dirname, SEPARATOR, relative_file_name);
        unlink(input_filename);
    }
    closedir(pDir);
    rmdir(output_dirname);
    return 0;
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf ("Usage: grib_checks <dirname>\n");
        return -1;
    }
    char output_dirname[FULL_FILENAME_SIZE];

    sprintf(output_dirname, "%s%s%s", argv[1], SEPARATOR, TMP_DIR);
    mkdir(output_dirname);
    convert_to_txt(argv[1], output_dirname);
    check_grib_files(output_dirname);
    delete_txt_files(output_dirname);

    return 0;
}

