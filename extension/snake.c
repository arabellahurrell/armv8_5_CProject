#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#include <unistd.h>

/*
Constants
*/

#define ROWS 12
#define COLS 32

char SNAKE_BODY = 'X';
char SNAKE_HEAD = 'O';
char WALL = '+';
char EMPTY = ' ';
char APPLE = 'A';

/*
Structures
*/

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

struct Position {
    int row, col;
};

// A Single unit of the snake
struct SnakeUnit {
    struct Position pos;
    struct SnakeUnit *next;
};

// Encapsulates the state of the snake
struct Snake {
    struct SnakeUnit *head;
    struct SnakeUnit *tail;
    int score;
};

/*
Global Varibles
*/

struct Snake snake;
char board[ROWS][COLS];
enum Direction direction;
struct Position applePos;
bool alive;
bool firstGame = 1;
bool newGame = 1;
int highscore = 0;

/*
Helper Functions
*/

void setBoardChar(struct Position pos, char character) {
    board[pos.row][pos.col] = character;
}

char getBoardChar(struct Position pos) {
    return board[pos.row][pos.col];
}

bool isWall(struct Position pos) {
    return pos.row == 0 || pos.row == ROWS - 1 || pos.col == 0 || pos.col == COLS - 1;
}

/*
Game Functions
*/

// Intialise an empty board with a wall border
void initialiseBoard() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            struct Position pos = {row, col};
            if (isWall(pos)) {
                setBoardChar(pos, WALL);
            } else {
                setBoardChar(pos, EMPTY);
            }
        }
    }
}

void placeApple() {
    // Generate random position
    do {
        int row = (rand() % (ROWS - 2)) + 1;
        // HAS THE SAME SEQUENCE OF NUMBERS EVERY TIME!!!!
        int col = (rand() % (COLS - 2)) + 1;
        applePos.row = row;
        applePos.col = col;
    } while (getBoardChar(applePos) != ' ');

    // Place apple
    setBoardChar(applePos, APPLE);
}


void resetGame() {
    // Reset global values
    alive = true;
    direction = RIGHT;
    initialiseBoard();
    placeApple();

    // Setup snake
    snake.score = 0;
    struct SnakeUnit *head = malloc(sizeof(struct SnakeUnit));
    head->pos.row = ROWS / 2;
    head->pos.col = 1;
    snake.head = head;
    snake.tail = head;
    setBoardChar(head->pos, SNAKE_HEAD);
}

void displayBoard() {
    printf("\e[1;1H\e[2J");
    for (int i = 0; i < ROWS; i++) {
        char row[COLS + 1];
        row[COLS] = '\0';
        strncpy(row, board[i], COLS);
        printf("%s\n", row);
    }
}

void displayScore() {
    printf("You scored %i!\n", snake.score);
}

void highScore() {
    if (snake.score > highscore) {
        highscore = snake.score;
    }
    printf("Highscore: %i\n", highscore);
}

void setUpGame() {
    char ignoreInput[1000];
    if (firstGame) {
        displayBoard();
        printf("Press enter to start.");
        while (getch() != 13) {
            usleep(10000);
        }
        firstGame = 0;
    } else {
        printf("Press enter to start replay.");
        while (getch() != 13) {
            usleep(10000);
        }
    }
}

// Function to take the input
void detectInput() {
    int key = 0;
    while (kbhit()) {
        key = getch();
    }
    switch (key) {
        case 72:
            if (direction != DOWN) {
                direction = UP;
            }
            break;
        case 75:
            if (direction != RIGHT) {
                direction = LEFT;
            }
            break;
        case 77:
            if (direction != LEFT) {
                direction = RIGHT;
            }
            break;
        case 80:
            if (direction != UP) {
                direction = DOWN;
            }
            break;
        default:
            break;
    }
}

void nextState() {
    struct SnakeUnit *newHead = malloc(sizeof(struct SnakeUnit));
    struct Position pos = snake.head->pos;

    // Determine next head position
    switch (direction) {
        case RIGHT:
            newHead->pos.col = pos.col + 1;
            newHead->pos.row = pos.row;
            break;
        case LEFT:
            newHead->pos.col = pos.col - 1;
            newHead->pos.row = pos.row;
            break;
        case UP:
            newHead->pos.col = pos.col;
            newHead->pos.row = pos.row - 1;
            break;
        case DOWN:
            newHead->pos.col = pos.col;
            newHead->pos.row = pos.row + 1;
            break;
    }

    char newHeadChar = getBoardChar(newHead->pos);
    snake.head->next = newHead;
    snake.head = newHead;
    setBoardChar(snake.head->pos, SNAKE_HEAD);
    setBoardChar(pos, SNAKE_BODY);

    if (newHeadChar == APPLE) { // Hit an Apple
        snake.score++;
        placeApple();
    } else {
        if (isWall(newHead->pos) || newHeadChar == SNAKE_BODY) { // Dead
            alive = false;
        }
        // Update tail
        struct SnakeUnit *newTail = snake.tail->next;
        setBoardChar(snake.tail->pos, EMPTY);
        free(snake.tail);
        snake.tail = newTail;
    }
}

int main() {

    while (newGame) {
        setUpGame();

        resetGame();
        displayBoard();

        while (alive) {
            detectInput();
            nextState();
            displayBoard();
            usleep(100000);
        }

        displayScore();
        highScore();
    }
    return EXIT_SUCCESS;
}