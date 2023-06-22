#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <conio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

/*
 * Constants
 */

// Customisable
#define ROWS 15
#define COLS 15
#define INITIAL_SLEEP_DURATION 150000
#define SPEED_MULTIPLIER 0.98
#define SNAKE_BODY 'X'
#define SNAKE_HEAD 'O'
#define WALL '+'
#define EMPTY ' '
#define APPLE 'A'

// Constant
#define UP_ARROW 72
#define LEFT_ARROW 75
#define RIGHT_ARROW 77
#define DOWN_ARROW 80
#define CLEAR_SCREEN "\033[1;1H\033[2J"

/*
 * Structures
 */

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

// A position of {0, 0} indicates the top left hand corner of the board
struct Position {
    int row, col;
};

// A single unit of the snake represented as a linked list node
struct SnakeNode {
    struct Position pos;
    struct SnakeNode *next;
};

// Encapsulates the state of the snake
// Inspired by linked list implementation
struct SnakeList {
    struct SnakeNode *head;
    struct SnakeNode *tail;
    int score;
};

/*
 * Global Variables
 */

// Game variables
struct SnakeList snake;
char board[ROWS][COLS];
enum Direction direction;
struct Position applePos;
int sleepDuration;
bool alive;
int loops;
int minTotalMoves = 0;

// Summary variables
bool firstGame = true;
int highScore = 0;
int highSkillLevel = 0;

/*
 * Helper Functions
 */

// Sets a character of the game board
void setBoardChar(struct Position pos, char ch) {
    board[pos.row][pos.col] = ch;
}

// Returns a character of the game board
char getBoardChar(struct Position pos) {
    return board[pos.row][pos.col];
}

// Returns true if the current position of the snake is at a wall
bool isWall(struct Position pos) {
    return pos.row == 0 || pos.row == ROWS - 1 ||
           pos.col == 0 || pos.col == COLS - 1;
}

/*
 * Game Functions
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

// Calculates the shortest path from the snake head to the apple.
int minimumMovesToApple(struct Position pos) {
    int moves = 0;

    moves += abs(pos.col - applePos.col);
    moves += abs(pos.row - applePos.row);

    // Readjusts values if snake is heading inn opposite direction to apple
    if ((direction == UP && pos.row < applePos.row &&
         pos.col == applePos.col) ||
        (direction == DOWN && pos.row > applePos.row &&
         pos.col == applePos.col) ||
        (direction == RIGHT && pos.col > applePos.col &&
         pos.row == applePos.row) ||
        (direction == LEFT && pos.col < applePos.col &&
         pos.row == applePos.row)) {
        moves += 2;
    }

    return moves;
}

void placeApple() {
    // Generate random position
    do {
        int row = (rand() % (ROWS - 2)) + 1;
        int col = (rand() % (COLS - 2)) + 1;
        applePos.row = row;
        applePos.col = col;
    } while (getBoardChar(applePos) != ' ');

    // Place the apple
    setBoardChar(applePos, APPLE);

    // Update min moves
    minTotalMoves += minimumMovesToApple(snake.head->pos);
}

void resetGame() {
    // Reset global values
    alive = true;
    loops = 0;
    minTotalMoves = 0;
    direction = RIGHT;
    sleepDuration = INITIAL_SLEEP_DURATION;
    initialiseBoard();

    // Setup snake
    snake.score = 0;
    struct SnakeNode *head = malloc(sizeof(struct SnakeNode));
    head->pos.row = ROWS / 2;
    head->pos.col = 1;
    snake.head = head;
    snake.tail = head;
    setBoardChar(head->pos, SNAKE_HEAD);

    placeApple();
}

void displayBoard() {
    // Clears console of previous output
    printf(CLEAR_SCREEN);

    // Prints board border
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            printf(" %c ", board[row][col]);
        }
        printf("\n");
    }
}

// Function to take the input from the arrow keys
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
    loops++;

    struct SnakeNode *newHead = malloc(sizeof(struct SnakeNode));
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

    // Update head
    char newHeadChar = getBoardChar(newHead->pos);
    snake.head->next = newHead;
    snake.head = newHead;
    setBoardChar(snake.head->pos, SNAKE_HEAD);
    setBoardChar(pos, SNAKE_BODY);

    if (newHeadChar == APPLE) { // Hit an Apple
        snake.score++;
        placeApple();
        displayBoard();
        sleepDuration = ceil(sleepDuration * SPEED_MULTIPLIER);
    } else {
        if (isWall(newHead->pos) || newHeadChar == SNAKE_BODY) { // Died
            alive = false;
            loops += minimumMovesToApple(newHead->pos);
        }
        // Update tail
        struct SnakeNode *newTail = snake.tail->next;
        setBoardChar(snake.tail->pos, EMPTY);
        free(snake.tail);
        snake.tail = (struct SnakeNode *) newTail;
    }
}

/*
 * UPDATE SKILL LEVEL FUNCTION
 *
 * Calculate minimum moves to get to next apple from head of snake
 * Calculate a 100*(minimum moves)/(actual moves - minimum moves) or something
 * Add this to store
 * Divide by number of apples when dead.
 *
 * note: consider direction travelling (as in snake cannot go back on itself)
 *
 */

void displayResults() {
    printf("\n");

    // Skill level indicates how efficiently you navigate to apples
    int skillLevel = (int) round((double) 100 * minTotalMoves / loops);

    // Score
    if (snake.score > 0) {
        printf("You scored %i!\n", snake.score);
        printf("Your skill level was %i!\n", skillLevel);
    } else {
        printf("You scored 0. Oh dear.\n");
    }

    // High score
    if (snake.score > highScore) {
        highScore = snake.score;
        printf("New high score!\n");
    } else {
        printf("High score: %i\n", highScore);
    }

    if (skillLevel > highSkillLevel) {
        highSkillLevel = skillLevel;
        printf("New high skill level!\n");
    } else {
        printf("High skill level: %i\n", skillLevel);
    }
    printf("\n");
}

// Return true iff the user wants to play
bool detectPlay() {
    int input, temp;
    printf("Press Enter to play%s > ",
           firstGame ? "" : " again or x to exit");
    firstGame = false;
    while ((temp = getchar()) != '\n') {
        input = temp;
    }
    return input != 'x';
}

/*
 * Main play loop
 */

int main() {
    // Set random seed
    srand(time(NULL));

    for (;;) {
        if (!detectPlay()) {
            return EXIT_SUCCESS;
        }

        resetGame();

        while (alive) {
            detectInput();
            nextState();
            displayBoard();
            usleep(sleepDuration);//sleepDuration);
        }

        displayResults();
    }
}