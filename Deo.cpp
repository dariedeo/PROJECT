#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"
#define MAX_BOOKS 100
#define MAX_TITLE 100
#define MAX_AUTHOR 100
#define MAX_NAME 50

typedef struct {
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int isBorrowed;
    char borrower[MAX_NAME];
} Book;

Book library[MAX_BOOKS];
int bookCount = 0;

void logTransaction(const char *action, const Book *book) {
    FILE *log = fopen("transactions.txt", "a");
    if (log != NULL) {
        fprintf(log, "%s: \"%s\" by %s (Borrower: %s)\n",
                action, book->title, book->author,
                book->isBorrowed ? book->borrower : "N/A");
        fclose(log);
    }
}

void saveToCSV() {
    FILE *file = fopen("library_books.csv", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(file, "Title,Author,Status,Borrower\n");
    for (int i = 0; i < bookCount; i++) {
        fprintf(file, "\"%s\",\"%s\",%s,\"%s\"\n",
                library[i].title,
                library[i].author,
                library[i].isBorrowed ? "Borrowed" : "Available",
                library[i].isBorrowed ? library[i].borrower : "N/A");
    }

    fclose(file);
}

void loadFromCSV() {
    FILE *file = fopen("library_books.csv", "r");
    if (file == NULL) return;

    char line[300];
    fgets(line, sizeof(line), file); // skip header

    while (fgets(line, sizeof(line), file)) {
        Book b;
        char status[20], borrower[MAX_NAME];

        sscanf(line, "\"%[^\"]\",\"%[^\"]\",%[^,],\"%[^\"]\"",
               b.title, b.author, status, borrower);

        b.isBorrowed = (strcmp(status, "Borrowed") == 0) ? 1 : 0;
        if (b.isBorrowed)
            strcpy(b.borrower, borrower);
        else
            strcpy(b.borrower, "");

        library[bookCount++] = b;
    }

    fclose(file);
}

void addBook() {
    if (bookCount >= MAX_BOOKS) {
        printf("Library is full!\n");
        return;
    }

    Book newBook;
    printf("Enter book title: ");
    fgets(newBook.title, MAX_TITLE, stdin);
    newBook.title[strcspn(newBook.title, "\n")] = '\0';

    printf("Enter author name: ");
    fgets(newBook.author, MAX_AUTHOR, stdin);
    newBook.author[strcspn(newBook.author, "\n")] = '\0';

    newBook.isBorrowed = 0;
    strcpy(newBook.borrower, "");

    library[bookCount++] = newBook;
    printf("Book added successfully!\n");

    saveToCSV();
}

void viewAllBooks() {
    printf("\n===== ALL BOOKS =====\n");

    if (bookCount == 0) {
        printf("No books in the library.\n");
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        printf("\nTitle: %s\n", library[i].title);
        printf("Author: %s\n", library[i].author);

        if (library[i].isBorrowed) {
            printf("Status: " COLOR_RED "Borrowed by %s" COLOR_RESET "\n", library[i].borrower);
        } else {
            printf("Status: " COLOR_GREEN "Available" COLOR_RESET "\n");
        }
    }
}

void borrowBook(const char *name) {
    int availableIndexes[MAX_BOOKS];
    int availableCount = 0;

    printf("\nAvailable Books to Borrow:\n");
    for (int i = 0; i < bookCount; i++) {
        if (!library[i].isBorrowed) {
            availableIndexes[availableCount++] = i;
            printf("[%d] %s by %s\n", availableCount, library[i].title, library[i].author);
        }
    }

    if (availableCount == 0) {
        printf("No books available to borrow.\n");
        return;
    }

    printf("\nEnter the numbers of the books you want to borrow (put space to borrow multiple books, 0 to stop borrowing books):\n");
    int selection;
    while (1) {
        scanf("%d", &selection);
        if (selection == 0) break;

        if (selection < 1 || selection > availableCount) {
            printf("Invalid selection: %d. Skipping...\n", selection);
            continue;
        }

        int bookIndex = availableIndexes[selection - 1];

        library[bookIndex].isBorrowed = 1;
        strcpy(library[bookIndex].borrower, name);
        printf("You have borrowed \"%s\".\n", library[bookIndex].title);
        logTransaction("Borrowed", &library[bookIndex]);
    }

    getchar(); 
    saveToCSV();
}
void returnBook(const char *name) {
    int borrowedIndexes[MAX_BOOKS];
    int borrowedCount = 0;

    printf("\nBooks You Have Borrowed:\n");
    for (int i = 0; i < bookCount; i++) {
        if (library[i].isBorrowed && strcmp(library[i].borrower, name) == 0) {
            borrowedIndexes[borrowedCount++] = i;
            printf("[%d] %s by %s\n", borrowedCount, library[i].title, library[i].author);
        }
    }

    if (borrowedCount == 0) {
        printf("You have no books to return.\n");
        return;
    }

    printf("\nEnter the numbers of the books you want to return (separated by spaces, end with 0):\n");
    int selection;
    while (1) {
        scanf("%d", &selection);
        if (selection == 0) break;

        if (selection < 1 || selection > borrowedCount) {
            printf("Invalid selection: %d. Skipping...\n", selection);
            continue;
        }

        int bookIndex = borrowedIndexes[selection - 1];

        library[bookIndex].isBorrowed = 0;
        strcpy(library[bookIndex].borrower, "");
        printf("You have returned \"%s\".\n", library[bookIndex].title);
        logTransaction("Returned", &library[bookIndex]);
    }

    getchar(); 
    saveToCSV();
}
void addPredefinedBooks() {
    struct {
        char title[MAX_TITLE];
        char author[MAX_AUTHOR];
    } predefined[] = {
        {"Pride and Prejudice", "Jane Austen"},
        {"The Notebook", "Nicholas Sparks"},
        {"Romeo and Juliet", "William Shakespeare"},
        {"The Kite Runner", "Khaled Hosseini"},
        {"Death of a Salesman", "Arthur Miller"},
        {"Hamlet", "William Shakespeare"},
        {"A Raisin in the Sun", "Lorraine Hansberry"},
        {"My Sister's Keeper", "Jodi Picoult"},
        {"Solaris", "Stanislaw Lem"},
        {"Kindred", "Octavia E. Butler"},
        {"Ender's Game", "Orson Scott Card"},
        {"Conclave", "Robert Harris"},
        {"The Shadow of the Wind", "Carlos Ruiz Zafon"},
        {"Jurassic Park", "Michael Crichton"},
        {"The Hobbit", "JRR Tolkien"},
        {"Treasure Island", "Robert Louis Stevenson"},
        {"To Kill a Mockingbird", "Harper Lee"},
        {"The Great Gatsby", "F. Scott Fitzgerald"},
        {"Dune", "Frank Herbert"},
        {"1984", "George Orwell"},
        {"The Martian", "Andy Weir"},
        {"Gone Girl", "Gillian Flynn"},
        {"The Girl with the Dragon Tattoo", "Stieg Larsson"},
        {"The Da Vinci Code", "Dan Brown"},
        {"The Hunger Games", "Suzanne Collins"},
        {"Harry Potter and the Sorcerers Stone", "J.K. Rowling"},
        {"The Maze Runner", "James Dashner"}
    };

    int count = sizeof(predefined) / sizeof(predefined[0]);

    for (int i = 0; i < count && bookCount < MAX_BOOKS; i++) {
        strcpy(library[bookCount].title, predefined[i].title);
        strcpy(library[bookCount].author, predefined[i].author);
        library[bookCount].isBorrowed = 0;
        strcpy(library[bookCount].borrower, "");
        bookCount++;
    }

    saveToCSV();
}

int isBookInGenre(const char *title, int genreId) {
    
    if (genreId == 1) {
        return (strcmp(title, "Pride and Prejudice") == 0 || strcmp(title, "The Notebook") == 0 || strcmp(title, "Romeo and Juliet") == 0 || strcmp(title, "The Kite Runner") == 0 || strcmp(title, "Death of a Salesman") == 0);
    } else if (genreId == 2) {
        return (strcmp(title, "To Kill a Mockingbird") == 0 || strcmp(title, "The Great Gatsby") == 0 || strcmp(title, "Hamlet") == 0 || strcmp(title, "A Raisin in the Sun") == 0 || strcmp(title, "My Sister's Keeper") == 0);
    } else if (genreId == 3) {
        return (strcmp(title, "Dune") == 0 || strcmp(title, "1984") == 0 || strcmp(title, "The Martian") == 0 || strcmp(title, "Solaris") == 0 || strcmp(title, "Kindred") == 0 || strcmp(title, "Ender's Game") == 0);
    } else if (genreId == 4) {
        return (strcmp(title, "Gone Girl") == 0 || strcmp(title, "The Girl with the Dragon Tattoo") == 0 || strcmp(title, "The Da Vinci Code") == 0 || strcmp(title, "Conclave") == 0 || strcmp(title, "The Shadow of the Wind") == 0);
    } else if (genreId == 5) {
        return (strcmp(title, "The Hunger Games") == 0 || strcmp(title, "Harry Potter and the Sorcerers Stone") == 0 || strcmp(title, "The Maze Runner") == 0 || strcmp(title, "Jurassic Park") == 0 || strcmp(title, "The Hobbit") == 0 || strcmp(title, "Treasure Island") == 0);
    }
    return 0;
}

int main() {
    int choice;
    char name[50];
    char department[50];
    char exitChoice;
    int genreCount[5] = {0};
    int shouldExit = 0;

    loadFromCSV();
if (bookCount == 0) {
    addPredefinedBooks(); 
}

    printf("============================================\n");
    printf("||                                        ||\n");
    printf("||           SAN PEDRO LIBRARY              ||\n");
    printf("||                                        ||\n");
    printf("============================================\n\n");
    printf("WELCOME TO SAN PEDRO DIGITAL LIBRARY!\n\n");

    printf("Enter your name: ");
    scanf("%s", name);
    printf("Enter your department: ");
    scanf("%s", department);
    getchar();
    
    do {
        printf("\nWELCOME USER!\n");
        printf("User: %s\n", name);
        printf("\n============================================\n");
        printf("USER: %s\t\tDEPARTMENT: %s\n", name, department);
        printf("============================================\n");
        printf("||             MAIN MENU                   ||\n");
        printf("============================================\n");
        printf("|| [1] ROMANCE                            ||\n");
        printf("|| [2] DRAMA                              ||\n");
        printf("|| [3] SCI-FI                             ||\n");
        printf("|| [4] THRILLER                           ||\n");
        printf("|| [5] ACTION                             ||\n");
        printf("|| [6] ADD A BOOK                         ||\n");
        printf("|| [7] VIEW ALL BOOKS                     ||\n");
        printf("|| [8] BORROW A BOOK                      ||\n");
        printf("|| [9] RETURN A BOOK                      ||\n");
        printf("|| [0] EXIT                               ||\n");
        printf("============================================\n");
        printf("SELECT (0-8): ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                genreCount[0]++;
                printf("You've chosen Romance genre!\n");
                printf("Books in Romance genre:\n");
                for (int i = 0; i < bookCount; i++) {
                    if (isBookInGenre(library[i].title, 1)) {
                        printf("- %s by %s", library[i].title, library[i].author);
                        if (library[i].isBorrowed) {
                            printf(" (Borrowed by %s)\n", library[i].borrower);
                        } else {
                            printf(" (Available)\n");
                        }
                    }
                }
                break;
            case 2:
                genreCount[1]++;
                printf("You've chosen Drama genre!\n");
                printf("Books in Drama genre:\n");
                for (int i = 0; i < bookCount; i++) {
                    if (isBookInGenre(library[i].title, 2)) {
                        printf("- %s by %s", library[i].title, library[i].author);
                        if (library[i].isBorrowed) {
                            printf(" (Borrowed by %s)\n", library[i].borrower);
                        } else {
                            printf(" (Available)\n");
                        }
                    }
                }
                break;
            case 3:
                genreCount[2]++;
                printf("You've chosen SCI-FI genre!\n");
                printf("Books in SCI-FI genre:\n");
                for (int i = 0; i < bookCount; i++) {
                    if (isBookInGenre(library[i].title, 3)) {
                        printf("- %s by %s", library[i].title, library[i].author);
                        if (library[i].isBorrowed) {
                            printf(" (Borrowed by %s)\n", library[i].borrower);
                        } else {
                            printf(" (Available)\n");
                        }
                    }
                }
                break;
            case 4:
                genreCount[3]++;
                printf("You've chosen Thriller genre!\n");
                printf("Books in Thriller genre:\n");
                for (int i = 0; i < bookCount; i++) {
                    if (isBookInGenre(library[i].title, 4)) {
                        printf("- %s by %s", library[i].title, library[i].author);
                        if (library[i].isBorrowed) {
                            printf(" (Borrowed by %s)\n", library[i].borrower);
                        } else {
                            printf(" (Available)\n");
                        }
                    }
                }
                break;
            case 5:
                genreCount[4]++;
                printf("You've chosen Action genre!\n");
                printf("Books in Action genre:\n");
                for (int i = 0; i < bookCount; i++) {
                    if (isBookInGenre(library[i].title, 5)) {
                        printf("- %s by %s", library[i].title, library[i].author);
                        if (library[i].isBorrowed) {
                            printf(" (Borrowed by %s)\n", library[i].borrower);
                        } else {
                            printf(" (Available)\n");
                        }
                    }
                }
                break;
            case 6:
                addBook();
                break;
            case 7:
                viewAllBooks();
                break;
            case 8:
                borrowBook(name);
                break;
            case 9:
                returnBook(name);
                break;
            case 0:
                shouldExit = 1;
                break;
            default:
                printf("Invalid choice. Please select again.\n");
        }

        if (choice != 0) {
            printf("\nDo you want to go back to genre selection? (y/n): ");
            scanf(" %c", &exitChoice);
            getchar();
            exitChoice = toupper(exitChoice);
            
            if (exitChoice == 'N') {
                shouldExit = 1;
            }
        }
    } while (!shouldExit);

      printf("\n============================================\n");
    printf("GENRE VISIT SUMMARY:\n");
    printf("Romance: %d visit(s)\n", genreCount[0]);
    printf("Drama: %d visit(s)\n", genreCount[1]);
    printf("SCI-FI: %d visit(s)\n", genreCount[2]);
    printf("Thriller: %d visit(s)\n", genreCount[3]);
    printf("Action: %d visit(s)\n", genreCount[4]);

    int maxCount = 0, mostVisitedGenre = -1;
    for (int i = 0; i < 5; i++) {
        if (genreCount[i] > maxCount) {
            maxCount = genreCount[i];
            mostVisitedGenre = i;
        }
    }

    if (mostVisitedGenre != -1) {
        printf("\nMost Visited Genre: ");
        switch (mostVisitedGenre) {
            case 0: printf("Romance\n"); break;
            case 1: printf("Drama\n"); break;
            case 2: printf("SCI-FI\n"); break;
            case 3: printf("Thriller\n"); break;
            case 4: printf("Action\n"); break;
        }
    } else {
        printf("\nNo genres were visited.\n");
    }

    printf("============================================\n");
    printf("Thank you for using the Digital Library!\n");
    return 0;
}
