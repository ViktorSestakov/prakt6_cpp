#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

const int Width = 40;
const int Height = 20;

const char Head = '0';
const char Body = 'o';
const char Food = '*';
const char Empty = '.';

struct Cords {
	int x;
	int y;
};

struct Segment {
	Cords pos;
	Segment* next;

	Segment(int x, int y) : pos({ x, y }), next(nullptr) {}
};

class Snake {
private:
	int lenght;
	int x;
	int y;

public:
	Segment* head;

	Snake(int stX, int stY) {
		head = new Segment(stX, stY);
		lenght = 1;
		x = 1;
		y = 0;
	}

	~Snake() {
		Segment* current = head;
		while (current != nullptr) {
			Segment* next = current->next;
			delete current;
			current = next;
		}
		head = nullptr;
	}

	Cords HeadPos() {
		return head->pos;
	}

	void setVector(int newX, int newY) {
		if (x + newX != 0 || y + newY != 0) {
			x = newX;
			y = newY;
		}
	}

	void move() {
		Cords newHeadPos = { head->pos.x + x, head->pos.y + y };

		Segment* newHead = new Segment(newHeadPos.x, newHeadPos.y);
		newHead->next = head;
		head = newHead;
		lenght++;

		// удаление
		Segment* current = head;
		Segment* prev = nullptr;

		while (current->next != nullptr) {
			prev = current;
			current = current->next;
		}

		delete current;

		if (prev != nullptr) {
			prev->next = nullptr;
		}
		else { // если состоит из 1 сегмента то как двинеться исчезнет
			delete head;
			head == nullptr;
		}
	}

	void movengrow() {
		Cords newHeadPos = { head->pos.x + x, head->pos.y + y };
		Segment* newHead = new Segment(newHeadPos.x, newHeadPos.y);
		newHead->next = head;
		head = newHead;
		lenght++;
	}

	bool cheakEatYourself() const {
		if (lenght < 4) return false;

		Cords headPos = head->pos;
		Segment* current = head->next;

		while (current != nullptr) {
			if (current->pos.x == headPos.x && current->pos.y == headPos.y) {
				return true;
			}

			current = current->next;
		}

		return false;
	}

	void draw(char board[Height][Width]) {
		Segment* current = head;
		int i = 0;

		while (current != nullptr) {
			if (i == 0) {
				board[current->pos.y][current->pos.x] = Head;
			}
			else {
				board[current->pos.y][current->pos.x] = Body;
			}
			current = current->next;
			i++;
		}
	}

	int getLenght() {
		return lenght;
	}
};

struct Phood {
	Cords pos;
	bool eat;

	void generate(const Snake snake) {
		bool placed = false;
		while (!placed) {
			pos.x = 1 + rand() % (Width - 2);
			pos.y = 1 + rand() % (Height - 2);

			bool onSnake = false;
			Segment* current = snake.head;

			while (current != nullptr) {
				if (current != nullptr) {
					onSnake = true;
					break;
				}
				current = current->next;
			}

			if (!onSnake) {
				eat = true;
				placed = true;
			}
		}
	}

	Phood() : eat(false) {}
};

void setup(Snake snake, Phood food, int score) {
	srand(time(0));
	score = 0;
	snake = Snake(Width / 4, Height / 2);
	food.generate(snake);
}

void drawBoard(const char board[Height][Width], int score, int lenght) {
	system("cls");

	cout << "#";
	for (int i = 0; i < Width - 2; ++i) {
		cout << "-";
	}
	cout << "#";

	for (int y = 1; y < Height - 1; ++y) {
		cout << "#";
		for (int x = 1; x < Width - 1; ++x) {
			cout << board[y][x];
		}
		cout << "#";
	}

	cout << "#";
	for (int i = 0; i < Width - 2; ++i) {
		cout << "-";
	}
	cout << "#";

	cout << "Score: " << score << " | Lenght: " << lenght << endl;
}

void Input(Snake snake) {
	if (_kbhit()) {
		int key = _getch();

		switch (key) {
		case 'w': case 'W':
			snake.setVector(0, -1);
			break;
		case 'a': case 'A':
			snake.setVector(-1, 0);
			break;
		case 's': case 'S':
			snake.setVector(0, 1);
			break;
		case 'd': case 'D':
			snake.setVector(1, 0);
			break;
		}
	}
}

bool Update(Snake snake, Phood food, int score) {
	Cords head = snake.HeadPos();

	if (head.x <= 0 || head.x >= Width - 1 || head.y <= 0 || head.y >= Height - 1) {
		return false;
	}

	if (snake.cheakEatYourself()) {
		return false;
	}

	if (food.eat && head.x == food.pos.x && head.y == food.pos.y) {
		score += 10;
		food.eat = false;
		snake.movengrow();
		food.generate(snake);
	}
	else {
		snake.move();
	}

	if (snake.HeadPos().x == snake.HeadPos().y && snake.HeadPos().x == 0) {
		return false;
	}

	return true;
}

void Game() {
	Snake snake(0, 0);
	Phood food;
	int score = 0;
	bool run = true;

	setup(snake, food, score);

	const int speed = 150;

	while (run) {
		char board[Height][Width];
		for (int y = 0; y < Height; ++y) {
			for (int x = 0; x < Width; ++x) {
				board[y][x] = Empty;
			}
		}


	}
}

int main()
{
	setlocale(LC_ALL, "Russian");

	Game();
}