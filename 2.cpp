#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
using namespace std;

//======================================================================
//                         UI / INTERFACE HELPERS
//======================================================================
const int BOX_WIDTH = 64;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void printLine(char ch = '=', int len = BOX_WIDTH) {
    cout << string(len, ch) << "\n";
}

void printCentered(const string& text, int width = BOX_WIDTH) {
    int pad = (width - (int)text.length()) / 2;
    if (pad < 0) pad = 0;
    cout << string(pad, ' ') << text << "\n";
}

// Full-screen header used for stand-alone screens (Sign Up, Login, etc.)
void printTitle(const string& subtitle) {
    clearScreen();
    printLine('=');
    printCentered("MINI GOOGLE DRIVE SYSTEM");
    printLine('-');
    if (!subtitle.empty()) {
        printCentered(subtitle);
        printLine('=');
    }
}

// Header used inside the logged-in role menus: shows user, role, current path
void printUserBar(const string& username, const string& role, const string& path) {
    clearScreen();
    printLine('=');
    printCentered("MINI GOOGLE DRIVE SYSTEM");
    printLine('-');
    cout << " User : " << username << "      Role : " << role << "\n";
    cout << " Path : " << path << "\n";
    printLine('=');
}

// Validated integer reader -- prevents the whole program from
// crashing/terminating on bad input (e.g. typing letters for a menu choice)
int readInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();

        cout << " Invalid input, please enter a number: ";
    }
    return value;
}

string getCurrentTime() {
    time_t now = time(0);
    string dt = ctime(&now);
    if (!dt.empty()) dt.pop_back(); // remove newline
    return dt;
}

//======================================================================
//                          DATA STRUCTURES
//======================================================================

// --------------------- Stack for Recycle Bin ---------------------
struct StackNode {
    string fileName;
    StackNode* next = nullptr;
};

class RecycleBin {
private:
    StackNode* top;
public:
    RecycleBin() : top(NULL) {}

    void push(string file) {
        StackNode* temp = new StackNode();
        temp->fileName = file;
        temp->next = top;
        top = temp;
    }

    void pop() {
        if (top == NULL) {
            cout << " Recycle bin is empty.\n";
            return;
        }
        StackNode* temp = top;
        cout << " Restored: " << top->fileName << "\n";
        top = top->next;
        delete temp;
    }

    void view() {
        if (top == NULL) {
            cout << " Recycle bin is empty.\n";
            return;
        }
        cout << " (most recently deleted first)\n";
        printLine('-');
        StackNode* temp = top;
        int i = 1;
        while (temp) {
            cout << "  " << i++ << ". " << temp->fileName << "\n";
            temp = temp->next;
        }
    }

    bool isEmpty() { return top == NULL; }
};

// --------------------- Queue for Recent Files ---------------------
struct QueueNode {
    string fileName;
    QueueNode* next = nullptr;
};

class RecentQueue {
private:
    QueueNode* front, * rear;
    int count, capacity;
public:
    RecentQueue(int cap) : front(NULL), rear(NULL), count(0), capacity(cap) {}

    void enqueue(string file) {
        QueueNode* temp = new QueueNode();
        temp->fileName = file;
        temp->next = NULL;

        if (count == capacity) {
            QueueNode* toDel = front;
            front = front->next;
            delete toDel;
            count--;
        }

        if (!rear) front = rear = temp;
        else {
            rear->next = temp;
            rear = temp;
        }
        count++;
    }

    void display() {
        if (!front) {
            cout << " No recent files yet.\n";
            return;
        }
        cout << " (oldest -> newest, capacity " << capacity << ")\n";
        printLine('-');
        QueueNode* temp = front;
        int i = 1;
        while (temp) {
            cout << "  " << i++ << ". " << temp->fileName << "\n";
            temp = temp->next;
        }
    }
};

// --------------------- Doubly Linked List for File Versions ---------------------
struct VersionNode {
    string content;
    VersionNode* next = nullptr, * prev = nullptr;
};

class VersionList {
private:
    VersionNode* head;
public:
    VersionList() : head(NULL) {}

    void addVersion(string data) {
        VersionNode* temp = new VersionNode();
        temp->content = data;
        temp->next = head;
        temp->prev = NULL;
        if (head) head->prev = temp;
        head = temp;
    }

    void viewVersions() {
        if (!head) {
            cout << " No versions recorded yet.\n";
            return;
        }
        cout << " (newest -> oldest)\n";
        printLine('-');
        VersionNode* temp = head;
        int i = 1;
        while (temp) {
            cout << "  v" << i++ << " : " << temp->content << "\n";
            temp = temp->next;
        }
    }
};

// --------------------- Hash Table for File Metadata ---------------------
const int HASH_SIZE = 10;

struct HashNode {
    string fileName, type, owner;
    int size;
    HashNode* next;
};

class HashTable {
private:
    HashNode* table[HASH_SIZE];

    int hash(string key) {
        int h = 0;
        for (char ch : key) h += ch;
        return h % HASH_SIZE;
    }

public:
    HashTable() {
        for (int i = 0; i < HASH_SIZE; i++)
            table[i] = NULL;
    }

    void insert(string name, string type, int size, string owner) {
        int idx = hash(name);
        HashNode* temp = new HashNode{ name, type, owner, size, table[idx] };
        table[idx] = temp;
    }

    bool find(string name) {
        int idx = hash(name);
        HashNode* temp = table[idx];
        while (temp) {
            if (temp->fileName == name) {
                printLine('-');
                cout << " File Name : " << temp->fileName << "\n";
                cout << " Type      : " << temp->type << "\n";
                cout << " Size      : " << temp->size << " KB\n";
                cout << " Owner     : " << temp->owner << "\n";
                printLine('-');
                return true;
            }
            temp = temp->next;
        }
        cout << " File not found.\n";
        return false;
    }
};

// --------------------- Tree for Folder Structure ---------------------
struct TreeNode {
    string name;
    TreeNode* child[10];
    int childCount;
    TreeNode* parent;
};

class FolderTree {
private:
    TreeNode* root;
    TreeNode* current;

    void dfs(TreeNode* node, int depth) {
        if (!node) return;
        cout << string(depth * 3, ' ') << (depth > 0 ? "|- " : "") << node->name << "\n";
        for (int i = 0; i < node->childCount; i++) {
            dfs(node->child[i], depth + 1);
        }
    }

public:
    FolderTree() {
        root = new TreeNode{ "root", {}, 0, NULL };
        current = root;
    }

    // Breadcrumb path from root to the current folder, e.g. root > Docs > 2024
    string getPath() {
        if (current == root) return "root";
        string result = current->name;
        TreeNode* node = current->parent;
        while (node) {
            result = node->name + " > " + result;
            node = node->parent;
        }
        return result;
    }

    void createFolder(string name) {
        if (current->childCount >= 10) {
            cout << " This folder already has the maximum number of sub-folders (10).\n";
            return;
        }
        TreeNode* node = new TreeNode{ name, {}, 0, current };
        current->child[current->childCount++] = node;
        cout << " Folder \"" << name << "\" created inside \"" << current->name << "\".\n";
    }

    void listContents() {
        cout << " Current Folder: " << getPath() << "\n";
        printLine('-');
        if (current->childCount == 0) {
            cout << " (empty -- no sub-folders here)\n";
            return;
        }
        for (int i = 0; i < current->childCount; i++)
            cout << "  [DIR] " << current->child[i]->name << "\n";
    }

    void enterFolder(string name) {
        for (int i = 0; i < current->childCount; i++) {
            if (current->child[i]->name == name) {
                current = current->child[i];
                cout << " Entered \"" << name << "\".\n";
                return;
            }
        }
        cout << " Folder not found in \"" << current->name << "\".\n";
    }

    void back() {
        if (current->parent) {
            current = current->parent;
            cout << " Moved back to \"" << current->name << "\".\n";
        } else {
            cout << " Already at root -- can't go back further.\n";
        }
    }

    void BFS() {
        TreeNode* queue[100];
        int front = 0, rear = 0;
        queue[rear++] = root;
        int level = 0;

        while (front < rear) {
            int levelCount = rear - front;
            cout << " Level " << level << ": ";
            for (int i = 0; i < levelCount; i++) {
                TreeNode* node = queue[front++];
                cout << node->name << "  ";
                for (int j = 0; j < node->childCount; j++) {
                    queue[rear++] = node->child[j];
                }
            }
            cout << "\n";
            level++;
        }
    }

    void DFS() {
        dfs(root, 0);
    }
};

// --------------------- Graph for Users and File Sharing ---------------------
struct User {
    string username;
    string password;
    string role;
    string securityQ;
    User* next;
};

class UserGraph {
public:
    User* head;
    UserGraph() : head(NULL) {}

    bool usernameTaken(string user) {
        User* temp = head;
        while (temp) {
            if (temp->username == user) return true;
            temp = temp->next;
        }
        return false;
    }

    void signUp(string user, string pass, string role, string secQ) {
        User* temp = new User{ user, pass, role, secQ, head };
        head = temp;
        cout << " User \"" << user << "\" registered successfully as " << role << ".\n";
    }

    User* login(string user, string pass) {
        User* temp = head;
        while (temp) {
            if (temp->username == user && temp->password == pass) {
                cout << " Login successful. Welcome, " << user << "!\n";
                return temp;
            }
            temp = temp->next;
        }
        cout << " Invalid username or password.\n";
        return NULL;
    }

    bool forgotPassword(string user, string secA) {
        User* temp = head;
        while (temp) {
            if (temp->username == user && temp->securityQ == secA) {
                cout << " Your password is: " << temp->password << "\n";
                return true;
            }
            temp = temp->next;
        }
        cout << " Incorrect username or security answer.\n";
        return false;
    }
};

//======================================================================
//                              MAIN PROGRAM
//======================================================================
int main() {

    UserGraph users;
    FolderTree folders;
    RecycleBin bin;
    RecentQueue recent(5);
    HashTable fileMeta;
    VersionList fileVersions;

    User* currentUser = NULL;
    int choice;

    try {
        //------------------------- AUTHENTICATION SCREEN -------------------------
        while (true) {
            printTitle("WELCOME");
            cout << "\n  [1] Sign Up\n  [2] Login\n  [3] Forgot Password\n  [4] Exit\n";
            printLine('-');
            cout << " Choice: ";
            choice = readInt();

            if (choice == 1) {
                printTitle("SIGN UP");
                string u, p, r, q;
                cout << " Username: "; cin >> u;

                if (users.usernameTaken(u)) {
                    cout << " That username is already taken.\n";
                    pauseScreen();
                    continue;
                }

                cout << " Password: "; cin >> p;
                cout << "\n Select Role:\n   [1] Admin\n   [2] User\n   [3] Viewer\n Choice: ";
                int roleChoice = readInt();
                if (roleChoice == 1) r = "Admin";
                else if (roleChoice == 2) r = "User";
                else if (roleChoice == 3) r = "Viewer";
                else {
                    cout << "\n Invalid role option. Sign up cancelled.\n";
                    pauseScreen();
                    continue;
                }
                cout << " Security Answer (used for password recovery): "; cin >> q;
                cout << "\n";
                printLine('-');
                users.signUp(u, p, r, q);
                pauseScreen();
            }
            else if (choice == 2) {
                printTitle("LOGIN");
                string u, p;
                cout << " Username: "; cin >> u;
                cout << " Password: "; cin >> p;
                cout << "\n";
                printLine('-');
                currentUser = users.login(u, p);
                if (currentUser) {
                    pauseScreen();
                    break;
                }
                pauseScreen();
            }
            else if (choice == 3) {
                printTitle("FORGOT PASSWORD");
                string u, q;
                cout << " Username: "; cin >> u;
                cout << " Security Answer: "; cin >> q;
                cout << "\n";
                printLine('-');
                users.forgotPassword(u, q);
                pauseScreen();
            }
            else if (choice == 4) {
                cout << "\n Goodbye!\n";
                return 0;
            }
            else {
                cout << " Invalid choice.\n";
                pauseScreen();
            }
        }

        //--------------------------- MAIN ROLE MENUS ---------------------------
        bool loggedIn = true;
        while (loggedIn)
        {
            if (currentUser->role == "Admin" || currentUser->role == "User") {

                printUserBar(currentUser->username, currentUser->role, folders.getPath());
                cout << "  [1]  Create Folder\n";
                cout << "  [2]  Enter Folder\n";
                cout << "  [3]  Go Back\n";
                cout << "  [4]  List Folder Contents\n";
                cout << "  [5]  Create File\n";
                cout << "  [6]  View Recent Files\n";
                cout << "  [7]  Delete File (-> Recycle Bin)\n";
                cout << "  [8]  Recycle Bin / Restore\n";
                cout << "  [9]  Update File (New Version)\n";
                cout << "  [10] View File Versions\n";
                cout << "  [11] Search File\n";
                if (currentUser->role == "Admin") {
                    cout << "  [12] View All Users\n";
                    cout << "  [13] Delete User Account\n";
                }
                cout << "  [14] BFS Folder Traversal\n";
                cout << "  [15] DFS Folder Traversal\n";
                cout << "  [16] Logout\n";
                printLine('-');
                cout << " Choice: ";
                choice = readInt();

                if (choice == 1) {
                    printTitle("CREATE FOLDER");
                    string name;
                    cout << " Folder Name: "; cin >> name;
                    cout << "\n";
                    folders.createFolder(name);
                    pauseScreen();
                }
                else if (choice == 2) {
                    printTitle("ENTER FOLDER");
                    string name;
                    cout << " Current: " << folders.getPath() << "\n";
                    cout << " Enter Folder Name: "; cin >> name;
                    cout << "\n";
                    folders.enterFolder(name);
                    pauseScreen();
                }
                else if (choice == 3) {
                    printTitle("GO BACK");
                    folders.back();
                    pauseScreen();
                }
                else if (choice == 4) {
                    printTitle("FOLDER CONTENTS");
                    folders.listContents();
                    pauseScreen();
                }
                else if (choice == 5) {
                    printTitle("CREATE FILE");
                    string name, type;
                    int size;
                    cout << " File Name: "; cin >> name;
                    cout << " Type: "; cin >> type;
                    cout << " Size (KB): "; size = readInt();
                    fileMeta.insert(name, type, size, currentUser->username);
                    recent.enqueue(name);
                    fileVersions.addVersion("Initial content of " + name);
                    cout << "\n";
                    printLine('-');
                    cout << " File \"" << name << "\" created at " << getCurrentTime() << "\n";
                    pauseScreen();
                }
                else if (choice == 6) {
                    printTitle("RECENT FILES");
                    recent.display();
                    pauseScreen();
                }
                else if (choice == 7) {
                    printTitle("DELETE FILE");
                    string name;
                    cout << " File to Delete: "; cin >> name;
                    bin.push(name);
                    cout << "\n \"" << name << "\" moved to Recycle Bin.\n";
                    pauseScreen();
                }
                else if (choice == 8) {
                    printTitle("RECYCLE BIN");
                    bin.view();
                    if (!bin.isEmpty()) {
                        cout << "\n Restore the most recently deleted file? (y/n): ";
                        char opt; cin >> opt;
                        if (opt == 'y' || opt == 'Y') bin.pop();
                    }
                    pauseScreen();
                }
                else if (choice == 9) {
                    printTitle("UPDATE FILE");
                    string content;
                    cout << " New content: ";
                    getline(cin, content);
                    fileVersions.addVersion(content);
                    cout << "\n New version saved.\n";
                    pauseScreen();
                }
                else if (choice == 10) {
                    printTitle("FILE VERSIONS");
                    fileVersions.viewVersions();
                    pauseScreen();
                }
                else if (choice == 11) {
                    printTitle("SEARCH FILE");
                    string name;
                    cout << " Search File: "; cin >> name;
                    cout << "\n";
                    fileMeta.find(name);
                    pauseScreen();
                }
                else if (choice == 12 && currentUser->role == "Admin") {
                    printTitle("REGISTERED USERS");
                    User* temp = users.head;
                    int i = 1;
                    while (temp) {
                        cout << "  " << i++ << ". " << temp->username << "  (" << temp->role << ")\n";
                        temp = temp->next;
                    }
                    pauseScreen();
                }
                else if (choice == 13 && currentUser->role == "Admin") {
                    printTitle("DELETE USER ACCOUNT");
                    string toDelete;
                    cout << " Enter username to delete: ";
                    cin >> toDelete;
                    User** temp = &users.head;
                    bool found = false;
                    while (*temp) {
                        if ((*temp)->username == toDelete) {
                            User* del = *temp;
                            *temp = del->next;
                            bool deletingSelf = (del == currentUser);
                            delete del;
                            found = true;
                            cout << "\n User \"" << toDelete << "\" deleted.\n";
                            if (deletingSelf) {
                                cout << " You deleted your own account. Logging out...\n";
                                currentUser = NULL;
                                loggedIn = false;
                            }
                            break;
                        }
                        temp = &((*temp)->next);
                    }
                    if (!found) cout << "\n User not found.\n";
                    pauseScreen();
                }
                else if (choice == 14) {
                    printTitle("BFS TRAVERSAL");
                    folders.BFS();
                    pauseScreen();
                }
                else if (choice == 15) {
                    printTitle("DFS TRAVERSAL");
                    folders.DFS();
                    pauseScreen();
                }
                else if (choice == 16) {
                    printTitle("LOGOUT");
                    cout << " Logging out... Goodbye, " << currentUser->username << "!\n";
                    pauseScreen();
                    currentUser = NULL;
                    loggedIn = false;
                }
                else {
                    cout << " Invalid choice.\n";
                    pauseScreen();
                }
            }
            else if (currentUser->role == "Viewer") {

                printUserBar(currentUser->username, currentUser->role, folders.getPath());
                cout << "  [1] Enter Folder\n";
                cout << "  [2] Go Back\n";
                cout << "  [3] List Contents\n";
                cout << "  [4] View Recent Files\n";
                cout << "  [5] Search File\n";
                cout << "  [6] Logout\n";
                printLine('-');
                cout << " Choice: ";
                choice = readInt();

                if (choice == 1) {
                    printTitle("ENTER FOLDER");
                    string name;
                    cout << " Current: " << folders.getPath() << "\n";
                    cout << " Enter Folder Name: "; cin >> name;
                    cout << "\n";
                    folders.enterFolder(name);
                    pauseScreen();
                }
                else if (choice == 2) {
                    printTitle("GO BACK");
                    folders.back();
                    pauseScreen();
                }
                else if (choice == 3) {
                    printTitle("FOLDER CONTENTS");
                    folders.listContents();
                    pauseScreen();
                }
                else if (choice == 4) {
                    printTitle("RECENT FILES");
                    recent.display();
                    pauseScreen();
                }
                else if (choice == 5) {
                    printTitle("SEARCH FILE");
                    string name;
                    cout << " Search File: "; cin >> name;
                    cout << "\n";
                    fileMeta.find(name);
                    pauseScreen();
                }
                else if (choice == 6) {
                    printTitle("LOGOUT");
                    cout << " Logging out... Goodbye, " << currentUser->username << "!\n";
                    pauseScreen();
                    currentUser = NULL;
                    loggedIn = false;
                }
                else {
                    cout << " Invalid choice.\n";
                    pauseScreen();
                }
            }
        }

        // After logout, restart at the authentication screen
        if (!currentUser) {
            printTitle("SESSION ENDED");
            cout << " You have been logged out.\n";
            cout << " Restart the program to log in again. Goodbye!\n";
        }
    }
    catch (exception& e) {
        cout << "\n An error occurred: " << e.what() << "\n";
    }
    catch (...) {
        cout << "\n Unknown error occurred.\n";
    }

    return 0;
}