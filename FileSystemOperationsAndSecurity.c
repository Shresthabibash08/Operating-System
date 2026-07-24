#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Default login credentials
#define USER "admin"
#define PASS "hello123"

// Maximum filename length
#define MAX 256

// Records every file operation into audit.log
void logAction(const char *action, const char *file) {
    FILE *log = fopen("audit.log", "a");
    if (log) {
        fprintf(log, "%s : %s\n", action, file);
        fclose(log);
    }
}

// Reads filename from the user
void getFilename(char *buffer) {
    printf("Enter filename: ");
    scanf("%255s", buffer);
}

// User authentication (3 attempts)
int authenticate() {
    char user[30], pass[30];

    for (int i = 3; i > 0; i--) {

        printf("Username: ");
        scanf("%29s", user);

        printf("Password: ");
        scanf("%29s", pass);

        if (strcmp(user, USER) == 0 && strcmp(pass, PASS) == 0) {
            printf("\nLogin Successful!\n");
            return 1;
        }

        printf("Invalid Login! Attempts Left: %d\n", i - 1);
    }

    return 0;
}

// Creates a new file
void createFile() {

    char file[MAX];

    getFilename(file);

    FILE *fp = fopen(file, "w");

    if (!fp) {
        perror("Error");
        return;
    }

    fclose(fp);

    // Default permission
    chmod(file, 0644);

    logAction("Created", file);

    printf("File created successfully.\n");
}

// Reads file contents
void readFile() {

    char file[MAX];
    int ch;

    getFilename(file);

    FILE *fp = fopen(file, "r");

    if (!fp) {
        perror("Error");
        return;
    }

    printf("\n----- File Content -----\n");

    while ((ch = fgetc(fp)) != EOF)
        putchar(ch);

    printf("\n------------------------\n");

    fclose(fp);

    logAction("Read", file);
}

// Appends data into an existing file
void writeFile() {

    char file[MAX], text[500];

    getFilename(file);

    while (getchar() != '\n');

    FILE *fp = fopen(file, "a");

    if (!fp) {
        perror("Error");
        return;
    }

    printf("Enter text: ");

    if (fgets(text, sizeof(text), stdin))
        fputs(text, fp);

    fclose(fp);

    logAction("Written", file);

    printf("Data written successfully.\n");
}

// Deletes a file
void deleteFile() {

    char file[MAX];

    getFilename(file);

    if (remove(file) == 0) {

        logAction("Deleted", file);

        printf("File deleted successfully.\n");

    } else

        perror("Error");
}

// Changes Linux file permission using chmod()
void setPermission() {

    char file[MAX];
    mode_t perm;
    int choice;

    getFilename(file);

    printf("\n1. 777 (All)\n");
    printf("2. 755 (Owner Full Access)\n");
    printf("3. 644 (Owner Read/Write)\n");
    printf("4. 600 (Owner Only)\n");
    printf("5. Custom Permission\n");

    printf("Choice: ");

    if (scanf("%d", &choice) != 1)
        return;

    switch (choice) {

        case 1:
            perm = 0777;
            break;

        case 2:
            perm = 0755;
            break;

        case 3:
            perm = 0644;
            break;

        case 4:
            perm = 0600;
            break;

        case 5:
            printf("Enter permission (Example:755): ");
            scanf("%o", &perm);
            break;

        default:
            printf("Invalid Choice!\n");
            return;
    }

    if (chmod(file, perm) == 0) {

        logAction("Permission Changed", file);

        printf("Permission updated successfully.\n");

    } else

        perror("Error");
}

// XOR encryption/decryption
// Running twice restores the original file
void xorFile() {

    char file[MAX];
    int ch;

    getFilename(file);

    FILE *fp = fopen(file, "rb+");

    if (!fp) {
        perror("Error");
        return;
    }

    while ((ch = fgetc(fp)) != EOF) {

        fseek(fp, -1, SEEK_CUR);

        fputc(ch ^ 'K', fp);

        fflush(fp);
    }

    fclose(fp);

    logAction("Encrypted/Decrypted", file);

    printf("Operation completed successfully.\n");
}

// Main program
int main() {

    int choice;

    if (!authenticate()) {

        printf("Authentication Failed!\n");

        return 0;
    }

    do {

        printf("\n========== Secure File Manager ==========\n");
        printf("1. Create File\n");
        printf("2. Read File\n");
        printf("3. Write File\n");
        printf("4. Delete File\n");
        printf("5. Set Permission\n");
        printf("6. Encrypt File\n");
        printf("7. Decrypt File\n");
        printf("8. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {

            while (getchar() != '\n');

            continue;
        }

        switch (choice) {

            case 1:
                createFile();
                break;

            case 2:
                readFile();
                break;

            case 3:
                writeFile();
                break;

            case 4:
                deleteFile();
                break;

            case 5:
                setPermission();
                break;

            case 6:
            case 7:
                xorFile();
                break;

            case 8:
                printf("Thank You!\n");
                break;

            default:
                printf("Invalid Choice!\n");
        }

    } while (choice != 8);

    return 0;
}
