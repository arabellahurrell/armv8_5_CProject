#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <conio.h>
#include <unistd.h>
#include <math.h>

/*
Constants
*/

#define ROWS 15
#define COLS 15
#define CLEAR_SCREEN "\033[1;1H\033[2J"
#define SNAKE_BODY 'X'
#define SNAKE_HEAD 'O'
#define WALL '+'
#define EMPTY ' '
#define APPLE 'A'
#define UP_ARROW 72
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define DOWN_ARROW 80
#define INITIAL_SLEEP_DURATION 150000
#define SPEED_MULTIPLIER 0.99

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
Global Variables
*/

struct Snake snake;
char board[ROWS][COLS];
enum Direction direction;
struct Position applePos;
bool alive;
int sleepDuration;
bool firstGame = true;
int highscore = 0;

/*
Helper Functions
*/

void setBoardChar(struct Position pos, char ch) {
    board[pos.row][pos.col] = ch;
}

char getBoardChar(struct Position pos) {
    return board[pos.row][pos.col];
}

bool isWall(struct Position pos) {
    return pos.row == 0 || pos.row == ROWS - 1 || pos.col == 0 ||
           pos.col == COLS - 1;
}

/*
Game Functions
*/

// Initialise an empty board with a wall border
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

    // Place the apple
    setBoardChar(applePos, APPLE);
}

void resetGame() {
    // Reset global values
    alive = true;
    direction = RIGHT;
    sleepDuration = INITIAL_SLEEP_DURATION;
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
    printf(CLEAR_SCREEN);
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            printf(" %c ", board[row][col]);
        }
        printf("\n");
    }
}

void displayScore() {
    // Display score
    if (snake.score > 0) {
        printf("You scored %i!\n", snake.score);
    }
    // Display the high score
    if (snake.score > highscore) {
        highscore = snake.score;
        printf("New highscore: %i\n", highscore);
    } else {
        printf("Highscore: %i\n", highscore);
    }
}

// Function to take the input
void detectInput() {
    int key = 0;
    while (kbhit()) {
        key = getch();
    }
    switch (key) {
        case UP_ARROW:
            if (direction != DOWN) {
                direction = UP;
            }
            break;
        case LEFT_ARROW:
            if (direction != RIGHT) {
                direction = LEFT;
            }
            break;
        case RIGHT_ARROW:
            if (direction != LEFT) {
                direction = RIGHT;
            }
            break;
        case DOWN_ARROW:
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
        sleepDuration = ceil(sleepDuration * SPEED_MULTIPLIER);
    } else {
        if (isWall(newHead->pos) || newHeadChar == SNAKE_BODY) { // Died
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
    for (;;) {
        char input;
        printf("Press Enter to play%s ",
               firstGame ? "" : " again or x to exit");
        firstGame = false;
        scanf("%c", &input);
        if (input == 'x') {
            return EXIT_SUCCESS;
        }

        resetGame();
        displayBoard();

        while (alive) {
            detectInput();
            nextState();
            displayBoard();
            usleep(sleepDuration);
        }

        displayScore();
    }
}