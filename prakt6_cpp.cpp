#include <iostream>
#include <conio.h> // Для _kbhit() и _getch()
#include <windows.h> // Для Sleep() и system("cls")
#include <ctime>   // Для time() и srand()
#include <locale.h> // Для setlocale()

using namespace std;

const int Width = 40;
const int Height = 20;

const char Head = '0';
const char Body = 'o';
const char Food = '*';
const char Empty = '.';

// Структура для хранения координат
struct Cords {
	int x;
	int y;
};

// Структура для сегмента змейки (используется как узел связного списка)
struct Segment {
	Cords pos;
	Segment* next;

	Segment(int x, int y) : pos({ x, y }), next(nullptr) {}
};

// Класс Змейки
class Snake {
private:
	int lenght; // Длина змейки
	int dirX;  // Направление движения по X
	int dirY;  // Направление движения по Y

public:
	Segment* head; // Голова змейки (начало связного списка)

	// Конструктор змейки
	Snake(int stX, int stY) {
		head = new Segment(stX, stY);
		lenght = 1;
		dirX = 1; // Начальное движение вправо
		dirY = 0;
	}

	// Деструктор змейки (освобождение памяти)
	~Snake() {
		Segment* current = head;
		while (current != nullptr) {
			Segment* next = current->next;
			delete current;
			current = next;
		}
		head = nullptr;
	}

	// Получить позицию головы
	Cords HeadPos() const { // Добавлен const, так как метод не меняет состояние объекта
		return head->pos;
	}

	// Задать новое направление движения
	void setVector(int newDirX, int newDirY) {
		// Запрещаем движение в противоположном направлении
		if (dirX + newDirX != 0 || dirY + newDirY != 0) {
			dirX = newDirX;
			dirY = newDirY;
		}
	}

	// Метод для обычного движения (без роста)
	void move() {
		// 1. Создаем новую голову
		Cords newHeadPos = { head->pos.x + dirX, head->pos.y + dirY };
		Segment* newHead = new Segment(newHeadPos.x, newHeadPos.y);
		newHead->next = head;
		head = newHead;

		// 2. Находим хвост и удаляем его
		Segment* tail = head;
		Segment* prevOfTail = nullptr;
		while (tail->next != nullptr) {
			prevOfTail = tail;
			tail = tail->next;
		}

		// tail теперь указывает на хвост
		delete tail; // Удаляем хвост

		if (prevOfTail != nullptr) {
			prevOfTail->next = nullptr; // Отсоединяем хвост
		} else {
			// Если prevOfTail == nullptr, значит, хвост был единственным сегментом (tail == head).
			// Мы добавили новую голову, и она стала единственной.
			// Старый head (который был единственным сегментом) удален.
			// `lenght` остается 1.
		}
		// `lenght` не меняется в `move()`, потому что один сегмент добавляется, один удаляется.
	}

	// Метод для движения с ростом (съедение еды)
	void movengrow() {
		Cords newHeadPos = { head->pos.x + dirX, head->pos.y + dirY };
		Segment* newHead = new Segment(newHeadPos.x, newHeadPos.y);
		newHead->next = head;
		head = newHead;
		lenght++; // <-- Увеличиваем длину
		// Хвост НЕ удаляется
	}

	// Проверка на поедание самого себя
	bool cheakEatYourself() const { // Добавлен const
		if (lenght < 4) return false; // Змейка не может съесть себя, пока не вырастет до определенной длины

		Cords headPos = head->pos;
		Segment* current = head->next; // Начинаем проверку со второго сегмента

		while (current != nullptr) {
			if (current->pos.x == headPos.x && current->pos.y == headPos.y) {
				return true; // Голова наехала на тело
			}
			current = current->next;
		}

		return false;
	}

	// Отрисовка змейки на игровом поле
	// board передается по ссылке на массив, чтобы изменения были видны вызывающей стороне
	void draw(char (&board)[Height][Width]) const { // Добавлен const
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

	// Получить длину змейки
	int getLenght() const { // Добавлен const
		return lenght;
	}
};

// Структура для еды
struct Phood {
	Cords pos;
	bool isEaten; // Переименовано для ясности

	// Генерация новой позиции еды (должна быть свободной)
	void generate(const Snake& snake) { // snake передается по const ссылке
		bool placed = false;
		while (!placed) {
			// Генерируем координаты внутри игрового поля (без учета стен)
			pos.x = 1 + rand() % (Width - 2);
			pos.y = 1 + rand() % (Height - 2);

			bool onSnake = false;
			Segment* current = snake.head;

			// Проверяем, не находится ли еда на теле змейки
			while (current != nullptr) {
				if (current->pos.x == pos.x && current->pos.y == pos.y) {
					onSnake = true;
					break;
				}
				current = current->next;
			}

			if (!onSnake) {
				isEaten = false; // Еда готова к поеданию
				placed = true;
			}
		}
	}

	// Конструктор
	Phood() : isEaten(true) {} // Изначально еда "съедена", чтобы при первом вызове generate она появилась
};

// --- Функции игры ---

// Инициализация игры
// snake и food передаются по ссылке, чтобы изменения были видны
void setup(Snake& snake, Phood& food, int& score) { // score также по ссылке
	srand(static_cast<unsigned int>(time(0))); // Более безопасный способ инициализации srand
	score = 0;
	// Создаем змейку в центре левой части игрового поля
	snake = Snake(Width / 4, Height / 2);
	food.generate(snake); // Генерируем первую еду
}

// Отрисовка игрового поля и информации
// board передается по const ссылке, так как функция только читает данные
void drawBoard(const char board[Height][Width], int score, int lenght) {
	system("cls"); // Очистка консоли

	// Верхняя граница
	cout << "#";
	for (int i = 0; i < Width - 2; ++i) {
		cout << "-";
	}
	cout << "#";
	cout << endl; // Переход на новую строку после верхней границы

	// Игровое поле
	for (int y = 1; y < Height - 1; ++y) {
		cout << "#"; // Левая граница
		for (int x = 1; x < Width - 1; ++x) {
			cout << board[y][x];
		}
		cout << "#"; // Правая граница
		cout << endl; // Переход на новую строку
	}

	// Нижняя граница
	cout << "#";
	for (int i = 0; i < Width - 2; ++i) {
		cout << "-";
	}
	cout << "#";
	cout << endl; // Переход на новую строку

	// Информация об игроке
	cout << "Score: " << score << " | Lenght: " << lenght << endl;
}

// Обработка ввода с клавиатуры
// snake передается по ссылке, чтобы изменения направления были видны
void Input(Snake& snake) {
	if (_kbhit()) { // Проверяем, была ли нажата клавиша
		int key = _getch(); // Получаем код нажатой клавиши

		switch (key) {
		case 'w': case 'W':
			snake.setVector(0, -1); // Движение вверх
			break;
		case 'a': case 'A':
			snake.setVector(-1, 0); // Движение влево
			break;
		case 's': case 'S':
			snake.setVector(0, 1);  // Движение вниз
			break;
		case 'd': case 'D':
			snake.setVector(1, 0);  // Движение вправо
			break;
		// Можно добавить выход из игры по другой кнопке, например ESC
		case 27: // Код клавиши ESC
			exit(0); // Завершить программу
			break;
		}
	}
}

// Основной цикл обновления состояния игры
// Возвращает true, если игра продолжается, false - если конец игры
// snake и food передаются по ссылке, score также по ссылке
bool Update(Snake& snake, Phood& food, int& score) {
	Cords head = snake.HeadPos();

	// 1. Проверка столкновения со стенами
	if (head.x <= 0 || head.x >= Width - 1 || head.y <= 0 || head.y >= Height - 1) {
		return false; // Конец игры
	}

	// 2. Проверка столкновения с самим собой
	if (snake.cheakEatYourself()) {
		return false; // Конец игры
	}

	// 3. Проверка поедания еды
	if (!food.isEaten && head.x == food.pos.x && head.y == food.pos.y) {
		score += 10; // Увеличиваем счет
		food.isEaten = true; // Еда съедена
		snake.movengrow(); // Змейка растет
		food.generate(snake); // Генерируем новую еду
	}
	else {
		// Если еда не была съедена, змейка просто движется
		snake.move();
	}

	// 4. Дополнительная проверка (похоже, лишняя, так как столкновение со стенами уже обработано)
	// if (snake.HeadPos().x == snake.HeadPos().y && snake.HeadPos().x == 0) {
	// 	return false;
	// }

	return true; // Игра продолжается
}

// Основная игровая функция
void Game() {
	Snake snake(0, 0); // Начальные параметры snake здесь не имеют значения, т.к. setup их переопределит
	Phood food;
	int score = 0;
	bool run = true; // Флаг для основного цикла

	setup(snake, food, score); // Инициализация

	const int speed = 150; // Скорость игры (чем меньше, тем быстрее)

	while (run) {
		// Игровое поле (на каждом шаге очищаем и отрисовываем заново)
		char board[Height][Width];
		for (int y = 0; y < Height; ++y) {
			for (int x = 0; x < Width; ++x) {
				board[y][x] = Empty; // Заполняем поле пустыми символами
			}
		}

		// Отрисовка змейки и еды на поле
		// Важно: еду нужно отрисовать после змейки, чтобы она была видна, если оказалась на голове
		if (!food.isEaten) {
			board[food.pos.y][food.pos.x] = Food;
		}
		snake.draw(board);

		drawBoard(board, score, snake.getLenght()); // Отрисовка всего интерфейса

		Input(snake); // Обработка ввода

		run = Update(snake, food, score); // Обновление состояния игры

		Sleep(speed); // Задержка для контроля скорости
	}

	// Если цикл завершился (run == false)
	cout << "Game Over!" << endl;
	cout << "Final Score: " << score << endl;
	// Пауза перед закрытием, чтобы пользователь увидел результат
	system("pause");
}

int main()
{
	setlocale(LC_ALL, "Russian"); // Для корректного отображения русских символов (хотя в данном коде они не используются)

	Game(); // Запуск игры

	return 0;
}
