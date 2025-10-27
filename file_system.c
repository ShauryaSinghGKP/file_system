#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50

// ---------------------- STRUCTURE ----------------------
typedef struct Node {
    char name[NAME_LEN];
    int isFile;                // 0 = directory, 1 = file
    struct Node *parent;
    struct Node *child;
    struct Node *sibling;
} Node;

Node *root, *currentDir;

// ---------------------- CORE FUNCTIONS ----------------------
Node* createNode(const char *name, int isFile) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    strcpy(newNode->name, name);
    newNode->isFile = isFile;
    newNode->parent = newNode->child = newNode->sibling = NULL;
    return newNode;
}

void addChild(Node *parent, Node *child) {
    if (!parent->child) parent->child = child;
    else {
        Node *temp = parent->child;
        while (temp->sibling) temp = temp->sibling;
        temp->sibling = child;
    }
    child->parent = parent;
}

Node* findChild(Node *parent, const char *name) {
    Node *temp = parent->child;
    while (temp) {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->sibling;
    }
    return NULL;
}

void listDir(Node *dir) {
    Node *temp = dir->child;
    if (!temp) {
        printf("[Empty Directory]\n");
        return;
    }
    while (temp) {
        printf("%s%s\n", temp->name, temp->isFile ? "" : "/");
        temp = temp->sibling;
    }
}

void printPath(Node *dir) {
    if (dir->parent == NULL) {
        printf("/");
        return;
    }
    printPath(dir->parent);
    printf("%s/", dir->name);
}

void deleteNode(Node *target) {
    if (!target) return;
    Node *child = target->child;
    while (child) {
        Node *next = child->sibling;
        deleteNode(child);
        child = next;
    }
    free(target);
}

// ---------------------- COMMANDS ----------------------
void mkdir_cmd(const char *name) {
    if (findChild(currentDir, name)) {
        printf("Directory already exists!\n");
        return;
    }
    addChild(currentDir, createNode(name, 0));
}

void touch_cmd(const char *name) {
    if (findChild(currentDir, name)) {
        printf("File already exists!\n");
        return;
    }
    addChild(currentDir, createNode(name, 1));
}

void ls_cmd() {
    listDir(currentDir);
}

void cd_cmd(const char *name) {
    if (strcmp(name, "..") == 0) {
        if (currentDir->parent) currentDir = currentDir->parent;
        return;
    }
    Node *next = findChild(currentDir, name);
    if (next && !next->isFile)
        currentDir = next;
    else
        printf("No such directory!\n");
}

void pwd_cmd() {
    printPath(currentDir);
    printf("\n");
}

void rm_cmd(const char *name) {
    Node *prev = NULL, *temp = currentDir->child;
    while (temp && strcmp(temp->name, name) != 0) {
        prev = temp;
        temp = temp->sibling;
    }
    if (!temp) {
        printf("Not found!\n");
        return;
    }
    if (prev) prev->sibling = temp->sibling;
    else currentDir->child = temp->sibling;
    deleteNode(temp);
}

// ---------------------- SEARCH & TEMP ----------------------
void search(Node *root, const char *name) {
    if (!root) return;
    if (strcmp(root->name, name) == 0) {
        printPath(root);
        printf("\n");
    }
    search(root->child, name);
    search(root->sibling, name);
}

void search_cmd(const char *name) {
    search(currentDir, name);
}

int hasTmpExtension(const char *filename) {
    int len = strlen(filename);
    return (len >= 4 && strcmp(filename + len - 4, ".tmp") == 0);
}

void removeTempFiles(Node *dir) {
    if (!dir) return;
    Node *prev = NULL;
    Node *curr = dir->child;
    while (curr) {
        Node *next = curr->sibling;
        if (curr->isFile && hasTmpExtension(curr->name)) {
            printf("Deleting temp file: %s\n", curr->name);
            if (prev) prev->sibling = next;
            else dir->child = next;
            deleteNode(curr);
        } else {
            removeTempFiles(curr);
            prev = curr;
        }
        curr = next;
    }
}

void rmtmp_cmd() {
    removeTempFiles(root);
    printf("All temporary (.tmp) files removed successfully.\n");
}

// ---------------------- MAIN PROGRAM ----------------------
int main() {
    root = createNode("", 0);
    currentDir = root;

    char cmd[100], arg[50];

    printf("\n=== FILE SYSTEM SIMULATOR (C) ===\n");
    printf("Commands:\n");
    printf(" mkdir <dir>\n touch <file>\n ls\n cd <dir>\n cd ..\n pwd\n rm <name>\n search <name>\n rmtmp\n exit\n\n");

    while (1) {
        printPath(currentDir);
        printf("> ");
        scanf("%s", cmd);

        if (strcmp(cmd, "exit") == 0) break;
        else if (strcmp(cmd, "mkdir") == 0) { scanf("%s", arg); mkdir_cmd(arg); }
        else if (strcmp(cmd, "touch") == 0) { scanf("%s", arg); touch_cmd(arg); }
        else if (strcmp(cmd, "ls") == 0) ls_cmd();
        else if (strcmp(cmd, "cd") == 0) { scanf("%s", arg); cd_cmd(arg); }
        else if (strcmp(cmd, "pwd") == 0) pwd_cmd();
        else if (strcmp(cmd, "rm") == 0) { scanf("%s", arg); rm_cmd(arg); }
        else if (strcmp(cmd, "search") == 0) { scanf("%s", arg); search_cmd(arg); }
        else if (strcmp(cmd, "rmtmp") == 0) rmtmp_cmd();
        else printf("Invalid command!\n");
    }

    deleteNode(root);
    printf("Exiting simulator...\n");
    return 0;
}
