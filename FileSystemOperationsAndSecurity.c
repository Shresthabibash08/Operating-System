#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define UNAME "admin"
#define PASS  "admin123"
#define MAXP  256

/* ---------- Authentication ---------- */
int authenticate(void) {
    char user[50], pass[50];
    int tries = 3;
    while (tries--) {
        printf("Username: ");
        scanf("%49s", user);
        printf("Password: ");
        scanf("%49s", pass);
        if (strcmp(user, UNAME) == 0 && strcmp(pass, PASS) == 0) {
            printf("\nLogin successful! Welcome, %s.\n\n", user);
            return 1;
        }
        printf("Invalid credentials. %d attempt(s) left.\n\n", tries);
    }
    return 0;
}

/* ---------- File Operations ---------- */
void createFile(void) {
    char fname[MAXP];
    printf("Enter filename to create: ");
    scanf("%255s", fname);
    FILE *fp = fopen(fname, "w");
    if (!fp) { perror("Error creating file"); return; }
    fclose(fp);
    chmod(fname, 0644); /* default permission: 644 */
    printf("File '%s' created successfully with default permission 644.\n", fname);
}

void readFile(void) {
    char fname[MAXP], ch;
    printf("Enter filename to read: ");
    scanf("%255s", fname);
    FILE *fp = fopen(fname, "r");
    if (!fp) { perror("Error opening file (check permissions)"); return; }
    printf("\n--- File Content ---\n");
    while ((ch = fgetc(fp)) != EOF) putchar(ch);
    printf("\n--- End of File ---\n");
    fclose(fp);
}

void writeFile(void) {
    char fname[MAXP], data[1000];
    printf("Enter filename to write/append: ");
    scanf("%255s", fname);
    printf("Append (a) or Overwrite (w)? ");
    char mode[2]; scanf("%1s", mode);
    FILE *fp = fopen(fname, mode[0] == 'a' ? "a" : "w");
    if (!fp) { perror("Error opening file (check permissions)"); return; }
    printf("Enter text (single line): ");
    getchar();
    fgets(data, sizeof(data), stdin);
    fputs(data, fp);
    fclose(fp);
    printf("Data written to '%s' successfully.\n", fname);
}

void deleteFile(void) {
    char fname[MAXP];
    printf("Enter filename to delete: ");
    scanf("%255s", fname);
    if (remove(fname) == 0)
        printf("File '%s' deleted successfully.\n", fname);
    else
        perror("Error deleting file");
}

void setPermissions(void) {
    char fname[MAXP];
    int owner, group, others;
    printf("Enter filename: ");
    scanf("%255s", fname);
    printf("Enter permission digits (0-7) for Owner Group Others\n");
    printf("(e.g. 6 4 4 => file permission 644): ");
    scanf("%d %d %d", &owner, &group, &others);
    if (owner < 0 || owner > 7 || group < 0 || group > 7 || others < 0 || others > 7) {
        printf("Invalid permission values.\n"); return;
    }
    mode_t mode = (owner << 6) | (group << 3) | others;
    if (chmod(fname, mode) == 0)
        printf("Permissions of '%s' set to %d%d%d successfully.\n", fname, owner, group, others);
    else
        perror("Error setting permissions");
}

/* ---------- Menu ---------- */
void showMenu(void) {
    printf("\n===== Secure File Manager =====\n");
    printf("1. Create File\n");
    printf("2. Read File\n");
    printf("3. Write/Append File\n");
    printf("4. Delete File\n");
    printf("5. Set Permissions\n");
    printf("6. Exit\n");
    printf("Choose an option: ");
}

int main(void) {
    if (!authenticate()) {
        printf("Authentication failed. Exiting.\n");
        return 1;
    }

    int choice;
    do {
        showMenu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: createFile(); break;
            case 2: readFile(); break;
            case 3: writeFile(); break;
            case 4: deleteFile(); break;
            case 5: setPermissions(); break;
            case 6: printf("Exiting. Goodbye!\n"); break;
            default: printf("Invalid choice, try again.\n");
        }
    } while (choice != 6);

    return 0;
}
