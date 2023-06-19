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
#define START_LENGTH 1
#define DIRECTION_INPUT 0xE0
#define FRAME_DURATION 0.5

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
    struct SnakeUnit *prev;
};

// Encapsulates the state of the snake
struct Snake {
    struct SnakeUnit *head;
    struct SnakeUnit *tail;
    int length;
};

/*
Global Varibles
*/

struct Snake snake;
char board[ROWS][COLS];
enum Direction direction;
struct Position applePos;
bool alive;

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
            setBoardChar(pos, EMPTY);
            if (isWall(pos)) {
                setBoardChar(pos, WALL);
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

void setupGame() {
    // Reset global values
    alive = true;
    initialiseBoard();
    placeApple();

    // Setup snake
    snake.length = START_LENGTH;
    struct SnakeUnit *head = malloc(sizeof(struct SnakeUnit));
    head->pos.row = ROWS / 2;
    head->pos.col = 1;
    snake.head = head;
    snake.tail = head;
    setBoardChar(head->pos, SNAKE_HEAD);
}

void displayBoard() {
    system("cls");
    for (int i = 0; i < ROWS; i++) {
        char row[COLS + 1];
        row[COLS] = '\0';
        strncpy(row, board[i], COLS);
        printf("%s\n", row);
    }
}

void displayScore() {
    printf("You scored %i!\n", snake.length);
}

// Function to take the input
void detectInput() {
    if (kbhit()) {
        int key = getch();
        if (key == DIRECTION_INPUT) {
            do {
                key = getch();
            } while (key == DIRECTION_INPUT);

            switch (key) {
                case 72:
                    direction = UP;
                    break;
                case 75:
                    direction = LEFT;
                    break;
                case 77:
                    direction = RIGHT;
                    break;
                case 80:
                    direction = DOWN;
                    break;
                case 'x':
                    alive = false;
                    break;
            }
        }
    }
}

void nextState() {
    struct SnakeUnit *newHead = malloc(sizeof(struct SnakeUnit));
    struct Position pos = snake.head->pos;
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
            newHead->pos.row = pos.row + 1;
            break;
        case DOWN:
            newHead->pos.col = pos.col;
            newHead->pos.row = pos.row - 1;
            break;
    }
    snake.head->next = newHead;
    setBoardChar(snake.head->pos, SNAKE_BODY);
    setBoardChar(newHead->pos, SNAKE_HEAD);
    snake.head = newHead;

    if (getBoardChar(snake.head->pos) == APPLE) {
        placeApple();
        snake.length++;
    } else {
        struct SnakeUnit *newTail = snake.tail->next;
        setBoardChar(snake.tail->pos, EMPTY);
        free(snake.tail);
        snake.tail = newTail;
    }
}

void detectDeath() {
    if (isWall(snake.head->pos) || getBoardChar(snake.head->pos) != ' ') {
        alive = false;
    }
}

int main() {
    setupGame();
    displayBoard();

    while (alive) {
        detectInput();
        nextState();
        detectDeath();
        displayBoard();
        usleep(500000);
    }

    displayScore();
    return EXIT_SUCCESS;
}