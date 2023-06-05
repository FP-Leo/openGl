#include"glew.h"
#include"freeglut.h"
#include<iostream>
#include <vector>

using namespace std;

//Function prototypes for the Snake Game
void resetBoard();
void editSnakeBodyCell(int, int, int);
void growSnake();
int detectCollision(int, int);
void display();
void drawLevelOne();
void drawSnake();
void changeDirection(int, int, int);
void refreshSnakeAnimation(int);
void projectLevelOne();
void generateFood();
void drawFood();
void deleteFood();
void startGame();

//To destroy the game window after the level is completed
int windowId;
int gameEnded = 0;

//For dynamic coding, everything is based on the board's size
const int BOARD_SIZE = 400;
//Dynamically defining snake's and borders' lengths
const int pointSize = BOARD_SIZE / 40;
const int lineWidth = BOARD_SIZE / 20;


//Snake's food related
//To keep track of how many food circles are generated and how many consumed
const int totalFood = 6;
const int smallFood = 0;
const int bigFood = 1;
int generatedFood = 1;
int foodType = smallFood;
//To know if a food is generated at that time, values: 0 = no, 1 = yes
int currentlyGenerated = 0;
//To keep track of the generated food and its type
int foodX = -1;
int foodY = -1;
int foodRadius = -1;
//Food score
const int smallFoodScore = 500;
const int maxFoodLifeTime = 10000;
//To keep track of how long has the current food been generated
int foodGeneratedTimer = 0;
//P value to generate circles/food
const double mathPi = 3.14159265358979323846;
//How many points to use to generate circles/food
const int circlePointNumber = 100;

//To keep track of the game timer
int gameTimer = 0;
const int refreshAnimationTimer = 200;

//To keep track of walls, player's and food position/s
int board[BOARD_SIZE][BOARD_SIZE] = { {0} };
//To keep track of the current Level
int level = 1;
//To keep track if the current level is projected onto the board, to only project the level once.values : 0 = no, 1 = yes
int levelProjected = 0;
//Default values for projecting objects
const int emptySpace = 0;
const int wallProjection = 1;
const int bodyProjetion = 2;
const int foodProjection = 3;
const int outOfBounds = 4;

//Setting the default starting point for snake and keeping track of its head
int snakeHeadX = pointSize + pointSize / 2, snakeHeadY = BOARD_SIZE / 2;
//To keep track of the snake's coordinates
vector<int> snakeX = {snakeHeadX, snakeHeadX-pointSize};
vector<int> snakeY = {snakeHeadY, snakeHeadY};
//To keep track of which direction is the snake moving, value: 1 = right, 2 = down, 3 = left, 4 = up
int direction = 1;
//To keep track of the score
int score = 0;

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitWindowPosition(500, 200);

	//To get what the user wants to do
	int choice = -1;
	cout << "Welcome!\n" << endl;
	cout << "1.Snake Game" << endl;
	cout << "2.Quit\n" << endl;
	cout << "Enter your choice: ";
	//Checking for bad input
	while (!(cin >> choice) || (choice != 1 && choice != 2)) {
		cin.clear(); //clear bad input flag
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); //discard input
		system("cls");
		cout << "Bad input! Try again!\n" << endl;
		Sleep(500);
		cout << "1.Snake Game" << endl;
		cout << "2.Quit\n" << endl;
		cout << "Enter your choice: ";
	}
	system("cls");
	
	if (choice == 1) {
		startGame();
	}
	else {
		return 0;
	}

	//Starting Window Loop
	glutMainLoop();

	return 0;
}

//Function to start the snake game
void startGame() {
	//to randomize where the food appears based on the current time seed
	srand(time(0));
	//explaining the game 
	for (int i = 3; i >= 1; i--) {
		cout << "\tExplanation:\n\nHello and welcome to the snake game.\n" <<
			"The game is simple, all you have to do is survive a certain amount of time to pass the level.\n" <<
			"Eat the food to gain points.\n" <<
			"Small food increases your size for 1 while big food increases it by 3.\n" <<
			"But be careful because they are only available for a certain time!\n" <<
			"If you survive long enough at the end your score will be displayed on the console.\n\n" <<
			"\tUse arrow keys to change the direction the snake!\n\n" <<
			"Press any of the F1 to F12, home, end, inset, page  up/down keys to quit in the middle of the game.\n" << 
			"If you touch a wall or your own body, you lose!\n" <<
			"Try to collect as much points as possible\n" <<
			"Good luck and have fun!\n"
			<< endl;

		cout << "\tThe game starts in " << i << " seconds" << endl;
		Sleep(1000);
		system("cls");
	}
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	//declaring window's size
	glutInitWindowSize(BOARD_SIZE, BOARD_SIZE);
	//declaring window's title
	windowId = glutCreateWindow("Snake Game ");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	gluOrtho2D(0.0, BOARD_SIZE, 0.0, BOARD_SIZE);

	//sets Point Size
	glPointSize(pointSize);
	//sets Line Width
	glLineWidth(lineWidth);

	//setting functions
	glutDisplayFunc(display);
	glutSpecialFunc(changeDirection);
	glutTimerFunc(refreshAnimationTimer, refreshSnakeAnimation, refreshAnimationTimer);

	//project the snake at the start on the board
	editSnakeBodyCell(snakeX[0], snakeY[0], 2);
	editSnakeBodyCell(snakeX[1], snakeY[1], 2);
}
//Function to reset the whole board to 0 and the global variables to their proper states
void resetBoard() {
	levelProjected = 0;
	snakeHeadX = pointSize + pointSize / 2, snakeHeadY = BOARD_SIZE / 2;

	snakeX.resize(2);
	snakeY.resize(2);
	snakeX[0] = snakeHeadX;
	snakeX[1] = snakeHeadX - pointSize;
	snakeY[0] = snakeHeadY;
	snakeY[1] = snakeHeadY;

	direction = 1;

	score = 0;

	foodX = -1;
	foodY = -1;
	foodRadius = -1;

	gameTimer = 0;

	generatedFood = 1;
	currentlyGenerated = 0;
	foodGeneratedTimer = 0;

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++)
			board[i][j] = emptySpace;
	}
}
//Function that projects level one onto the board
void projectLevelOne() {
	//projecting the window onto a 2D matrix only once
	int lineRadius = lineWidth / 2;
	int lineLength = BOARD_SIZE / 4 - 1;
	int bs = BOARD_SIZE - 1;

	//drawing line borders
	for (int i = 0; i < lineLength; i++) {
		board[lineRadius][i] = wallProjection;
		board[i][lineRadius] = wallProjection;

		board[bs - lineLength + i][lineRadius] = wallProjection;
		board[bs - lineRadius][i] = wallProjection;

		board[bs - lineLength + i][bs - lineRadius] = wallProjection;
		board[bs - lineRadius][bs - i] = wallProjection;

		board[lineRadius][bs - lineLength + i] = wallProjection;
		board[i][bs - lineRadius] = wallProjection;
	}
	//drawing line borders' corners
	for (int i = 0; i <= lineRadius; i++) {
		board[lineLength][i] = wallProjection;
		board[i][lineLength] = wallProjection;

		board[i][bs - lineLength] = wallProjection;
		board[lineLength][bs - i] = wallProjection;

		board[bs - lineLength][i] = wallProjection;
		board[bs - i][lineLength] = wallProjection;

		board[bs - i][bs - lineLength] = wallProjection;
		board[bs - lineLength][bs - i] = wallProjection;
	}
	//drawing the rectangle on the middle
	for (int i = lineLength; i <= lineLength * 3; i++) {
		for (int j = BOARD_SIZE * 1 / 3; j <= BOARD_SIZE * 2 / 3; j++)
			board[i][j] = wallProjection;
	}
	//levelProjected successfully, no need to project it again
	levelProjected = 1;
}
//Function to set or remove one segment of the snake from/to the 2d board
void editSnakeBodyCell(int x, int y, int value) {
	x -= pointSize / 2;
	y -= pointSize / 2;

	for (int i = 0; i < pointSize; i++) {
		for (int j = 0; j < pointSize; j++) {
			if (x + i >= BOARD_SIZE || x + i < 0 || y + i >= BOARD_SIZE || y + i < 0)
				continue;
			board[x + i][y + j] = value;
		}
	}
}
//Function to add a segment to the end of snake's body
void growSnake() {
	int x = snakeX[snakeX.size() - 1];
	int y = snakeY[snakeY.size() - 1];

	switch (direction) {
		case 1: x += pointSize; break;
		case 2: y -= pointSize; break;
		case 3: x -= pointSize; break;
		case 4: y += pointSize; break;
	}

	snakeX.push_back(x);
	snakeY.push_back(y);
	editSnakeBodyCell(x, y, bodyProjetion);
}
//Function to check if the snake's head touched a wall, it's own body, or food
int detectCollision(int x, int y) {
	int count = 0;
	x -= pointSize / 2;
	y -= pointSize / 2;
	for (int i = 0; i < pointSize; i++) {
		for (int j = 0; j < pointSize; j++) {
			if (x + i >= BOARD_SIZE || x + i < 0 || y + i >= BOARD_SIZE || y + i < 0) {
				count++;
				continue;
			}
			if (board[x + i][y + j] != emptySpace)
				return board[x + i][y + j];
		}
	}
	if (count == pointSize * pointSize)
		return outOfBounds;
	return emptySpace;
}
//Function generate proper x,y coordinates for the food
void generateFood() {
	//Don't generate more than needed
	if (generatedFood > totalFood)
		return;
	//Calculate the radius of the food based on its type, for every 2 small foods generate a big one
	int radius;
	if (generatedFood % 3 != 0) {
		foodType = smallFood;
		radius = pointSize * 4 / 3;
	}
	else {
		foodType = bigFood;
		radius = pointSize * 2;
	}
	//Generate Position
	int x = rand() % BOARD_SIZE;
	int y = rand() % BOARD_SIZE;
	// Check if the generated position collides with the player or walls, if so regenerate
	while(board[x][y] != emptySpace || x + radius >= BOARD_SIZE || y + radius >= BOARD_SIZE || x - radius < 0 || y - radius < 0 ||
			board[x + radius][y] != emptySpace || board[x][y + radius] != emptySpace || board[x - radius][y] != emptySpace || board[x][y - radius] != emptySpace){
		x = rand() % BOARD_SIZE;
		y = rand() % BOARD_SIZE;
	}
		
	foodX = x;
	foodY = y;
	foodRadius = radius;

	generatedFood++;
}
//Function to draw the food based on the generated x,y coordinates and project this food onto the board
void drawFood() {
	if (foodX == -1 || foodY == -1 || foodRadius == -1)
		return;
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLE_FAN);
	// Center vertex
	glVertex2i(foodX, foodY); 
	for (float i = 0; i <= circlePointNumber; i++) {
		float theta = 2.0f * mathPi* static_cast<float>(i) / static_cast<float>(circlePointNumber);
		float dx = foodRadius * std::cos(theta);
		float dy = foodRadius * std::sin(theta);
		glVertex2f(foodX + dx, foodY + dy);
		//Projecting it to the board
		board[foodX + (int)dx][foodY + (int)dy] = foodProjection;
	}
	glEnd();
}
//Remove the Food projected at the 2D Array
void deleteFood() {
	for (float i = 0; i <= circlePointNumber; i++) {
		float theta = 2.0f * mathPi * static_cast<float>(i) / static_cast<float>(circlePointNumber);
		float dx = foodRadius * std::cos(theta);
		float dy = foodRadius * std::sin(theta);
		board[foodX + (int)dx][foodY + (int)dy] = emptySpace;
	}

	foodX = -1;
	foodY = -1;
	foodRadius = -1;

	currentlyGenerated = 0;
	foodGeneratedTimer = 0;

	if (generatedFood > totalFood) {
		gameEnded = 1;
		cout << "You won!!" << endl;
		cout << "Total points: " << score << endl;
		resetBoard();
		level++;
	}
}
//Main function to draw the game
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);

	//draw the snake
	drawSnake();

	//draw the level
	switch (level) {
		case 1: drawLevelOne(); break;
	}
	//draw the food when it's time
	drawFood();
	
	glFlush();
	//destroy the window if the game ended
	if (gameEnded) {
		Sleep(500);
		glutDestroyWindow(windowId);
	}	
}
//Function that draws the snake using the coordinate vectors
void drawSnake() {
	glBegin(GL_POINTS);
	for (int i = 0; i < snakeX.size(); i++) {
		glColor3f(1.0, 0.0, 0.0);
		glVertex2i(snakeX[i], snakeY[i]);
	}
	glEnd();
}
//Function that draws the first level and projects it onto the 2d board
void drawLevelOne(void) {
	//project the level onto the board
	if (levelProjected == 0) {
		projectLevelOne();
	}
	
	glColor3i(0, 0, 0);
	//draw the level
	glBegin(GL_LINE_STRIP);
	glVertex2i(0, BOARD_SIZE / 4);
	glVertex2i(0, 0);
	glVertex2i(BOARD_SIZE / 4, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(BOARD_SIZE - BOARD_SIZE / 4, 0);
	glVertex2i(BOARD_SIZE, 0);
	glVertex2i(BOARD_SIZE, BOARD_SIZE / 4);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(BOARD_SIZE / 4, BOARD_SIZE);
	glVertex2i(0, BOARD_SIZE);
	glVertex2i(0, BOARD_SIZE - BOARD_SIZE / 4);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(BOARD_SIZE - BOARD_SIZE / 4, BOARD_SIZE);
	glVertex2i(BOARD_SIZE, BOARD_SIZE);
	glVertex2i(BOARD_SIZE, BOARD_SIZE - BOARD_SIZE / 4);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2i(BOARD_SIZE / 4, BOARD_SIZE / 3);
	glVertex2i(BOARD_SIZE / 4, BOARD_SIZE * 2 / 3);
	glVertex2i(BOARD_SIZE * 3 / 4, BOARD_SIZE * 2 / 3);
	glVertex2i(BOARD_SIZE * 3 / 4, BOARD_SIZE / 3);
	glEnd();
}
//Function that reacts to key pressed and changes direction based on the key pressed
void changeDirection(int key, int x, int y) {
	//switch directions based on pressed keys; Dont allow 180 degree turns;
	switch (key) {
		case GLUT_KEY_UP: {
			if (direction == 2)
				return;
			direction = 4;
		}
		break;
		case GLUT_KEY_DOWN: {
			if (direction == 4)
				return;
			direction = 2;
		}break;
		case GLUT_KEY_LEFT: {
			if (direction == 1)
				return;
			direction = 3;
		} break;
		case GLUT_KEY_RIGHT: {
			if (direction == 3)
				return;
			direction = 1;

		} break;
		default: direction = 5; break;
	}
}
//Constantly moves the snake and redraws the frames
void refreshSnakeAnimation(int value) {
	if (gameTimer % (refreshAnimationTimer*100) == 0) {
		generateFood();
		currentlyGenerated = 1;
	}
	gameTimer += value;

	//remove the last segment from the board
	editSnakeBodyCell(snakeX[snakeX.size() - 1], snakeY[snakeX.size() - 1], emptySpace);
	//increase the appropriate value based on direction
	switch (direction) {
		case 1: snakeHeadX += pointSize; break;
		case 2: snakeHeadY -= pointSize; break;
		case 3: snakeHeadX -= pointSize; break;
		case 4: snakeHeadY += pointSize; break;
		case 5: gameEnded = 1; return;
	}

	int result = detectCollision(snakeHeadX, snakeHeadY);
	//check to see if snake crashed on the wall or with it's body
	if (result == wallProjection || result == bodyProjetion) {
		cout << "You lose!" << endl;
		//add a segment to show the place it crashed to
		editSnakeBodyCell(snakeHeadX, snakeHeadY, bodyProjetion);
		gameEnded = 1;
	}
	else if (result == foodProjection) {//check to see if the snake ate food
		//add a segment at the end
		growSnake();
		score += smallFoodScore;
		//if it's a big meal add two other segment
		if (foodType == bigFood) {
			growSnake();
			growSnake();
			score += 2*smallFoodScore;
		}
		//remove the projected food from the board
		deleteFood();
	}
	else if(result == outOfBounds){
		switch (direction) {
			case 1: snakeHeadX = pointSize / 2;  break;
			case 2: snakeHeadY = BOARD_SIZE - pointSize / 2; break;
			case 3: snakeHeadX = BOARD_SIZE - pointSize / 2; break;
			case 4: snakeHeadY = pointSize / 2; break;
		}
	}

	if (currentlyGenerated) {
		foodGeneratedTimer += value;
		if (foodGeneratedTimer == maxFoodLifeTime) {
			deleteFood();
		}
	}

	//remove the last segment's coordinates
	snakeX.pop_back();
	snakeY.pop_back();
	//add a new segment at the beggining
	snakeX.insert(snakeX.begin(), snakeHeadX);
	snakeY.insert(snakeY.begin(), snakeHeadY);
	//redraw the frames
	glutPostRedisplay();
	//project the added segment at the board
	editSnakeBodyCell(snakeHeadX, snakeHeadY, bodyProjetion);
	//continue moving the snake
	glutTimerFunc(refreshAnimationTimer, refreshSnakeAnimation, refreshAnimationTimer);
}
//Main function for the 3D animation, creates the window and sets the propper functions

