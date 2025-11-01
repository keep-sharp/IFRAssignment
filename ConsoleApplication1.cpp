#include "header.h"
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <limits>
#include <string>

using namespace std;
using namespace snake_game;

// GameBase类实现
GameBase::GameBase() : score(0), sleeptime(300), add(10), isRunning(false), isPaused(false) {}

void GameBase::setCursorPosition(int x, int y) const {
    COORD cursorPosition;
    cursorPosition.X = static_cast<SHORT>(x);
    cursorPosition.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void GameBase::clearScreen() {
    system("cls"); // 清屏
}

int GameBase::getLevel() const {
    if (score >= 0 && score <= 50)
        return 1;
    else if (score > 50 && score <= 100)
        return 2;
    else if (score > 100 && score <= 200)
        return 3;
    else
        return 4;
}

void GameBase::pauseGame() {
    isPaused = true;
    while (isPaused) {
        Sleep(100);
        if (GetAsyncKeyState(VK_SPACE)) {
            isPaused = false;
            break;
        }
    }
}

// SnakeGame类实现
SnakeGame::SnakeGame() : currentDir(RIGHT), endGameStatus(0) {
    srand(static_cast<unsigned>(time(NULL)));
}

void SnakeGame::init() {
    // 初始化蛇身（用map存储x和y，key=0为x，key=1为y）
    snakeBody.clear();
    snakeBody.push_back({ {0, 28}, {1, 10} });  // 头部
    snakeBody.push_back({ {0, 26}, {1, 10} });
    snakeBody.push_back({ {0, 24}, {1, 10} });  // 尾部

    // 初始化游戏状态
    score = 0;
    sleeptime = 300;
    add = 10;
    isRunning = true;
    currentDir = RIGHT;
    endGameStatus = 0;

    // 生成食物
    generateFood();
}

void SnakeGame::createMap() const {
    // 绘制上下边框
    for (int i = 0; i < 58; i += 2) {
        setCursorPosition(i, 0);
        cout << "■";
        setCursorPosition(i, 26);
        cout << "■";
    }

    // 绘制左右边框
    for (int i = 1; i < 26; i++) {
        setCursorPosition(0, i);
        cout << "■";
        setCursorPosition(56, i);
        cout << "■";
    }

    drawScoreAndTips();
}

void SnakeGame::drawScoreAndTips() const {
    setCursorPosition(74, 2);
    cout << "当前分数: " << score;
    setCursorPosition(74, 4);
    cout << "当前等级: " << getLevel();

    setCursorPosition(74, 8);
    cout << "每吃食物分数为: " << add << "分";
    setCursorPosition(74, 10);
    cout << "不要撞墙，也不要咬到自己";
    setCursorPosition(74, 12);
    cout << "使用方向键移动，按住shift加速";
    setCursorPosition(74, 14);
    cout << "可以通过space暂停游戏";
}

void SnakeGame::generateFood() {
    // 确保食物在偶数x位置（与蛇身对齐）
    do {
        food[0] = rand() % 52 + 2;  // x坐标（key=0）
    } while (food[0] % 2 != 0);

    food[1] = rand() % 24 + 1;  // y坐标（key=1）

    // 检查是否与蛇身重叠
    for (const auto& segment : snakeBody) {
        if (segment.at(0) == food[0] && segment.at(1) == food[1]) {
            generateFood();
            return;
        }
    }
}

bool SnakeGame::checkCollision() {
    // 获取蛇头位置
    auto head = snakeBody.front();

    // 检查撞墙
    if (head.at(0) <= 0 || head.at(0) >= 56 || head.at(1) <= 0 || head.at(1) >= 26) {
        endGameStatus = 1;
        return true;
    }

    // 检查咬到自己
    for (size_t i = 1; i < snakeBody.size(); ++i) {
        if (snakeBody[i].at(0) == head.at(0) && snakeBody[i].at(1) == head.at(1)) {
            endGameStatus = 2;
            return true;
        }
    }

    return false;
}

void SnakeGame::moveSnake() {
    // 获取当前头部位置
    auto head = snakeBody.front();
    map<int, int> newHead = head;  // 用map创建新头部

    // 根据方向计算新头部位置
    switch (currentDir) {
    case UP:
        newHead[1]--;  // y坐标（key=1）减1
        break;
    case DOWN:
        newHead[1]++;  // y坐标加1
        break;
    case LEFT:
        newHead[0] -= 2;  // x坐标（key=0）减2
        break;
    case RIGHT:
        newHead[0] += 2;  // x坐标加2
        break;
    }

    // 添加新头部
    snakeBody.insert(snakeBody.begin(), newHead);

    // 检查是否吃到食物
    if (newHead[0] == food[0] && newHead[1] == food[1]) {
        score += add;
        speedUp();
        generateFood();
        drawFood();
    }
    else {
        // 移除尾部
        clearTail();
        snakeBody.pop_back();
    }

    // 绘制蛇
    drawSnake();
}

void SnakeGame::handleInput() {
    // 键盘控制（逻辑不变）
    if (GetAsyncKeyState(VK_UP) && currentDir != DOWN) {
        currentDir = UP;
    }
    else if (GetAsyncKeyState(VK_DOWN) && currentDir != UP) {
        currentDir = DOWN;
    }
    else if (GetAsyncKeyState(VK_LEFT) && currentDir != RIGHT) {
        currentDir = LEFT;
    }
    else if (GetAsyncKeyState(VK_RIGHT) && currentDir != LEFT) {
        currentDir = RIGHT;
    }

    if (GetAsyncKeyState(VK_SPACE)) {
        pauseGame();
    }
    else if (GetAsyncKeyState(VK_SHIFT)) {
        speedUp();
    }
    else if (GetAsyncKeyState(VK_F1)) {
        speedDown();
    }
}

void SnakeGame::drawSnake() const {
    // 绘制头部
    setCursorPosition(snakeBody.front().at(0), snakeBody.front().at(1));
    cout << "*";
}

void SnakeGame::drawFood() const {
    // 将 food[0] 改为 food.at(0)，food[1] 改为 food.at(1)
    setCursorPosition(food.at(0), food.at(1));
    cout << "★";
}

void SnakeGame::clearTail() {
    auto tail = snakeBody.back();
    setCursorPosition(tail.at(0), tail.at(1));
    cout << "  ";  // 用空格覆盖
}

void SnakeGame::speedUp() {
    if (sleeptime >= 50) {
        sleeptime -= 10;
        add += 2;
    }
}

void SnakeGame::speedDown() {
    if (sleeptime < 350) {
        sleeptime += 30;
        add -= 2;
    }
}

void SnakeGame::showWelcomeScreen() {
    clearScreen();
    setCursorPosition(48, 8);
    cout << "******************" << endl;
    setCursorPosition(49, 9);
    cout << " 贪 吃 蛇 游 戏 " << endl;
    setCursorPosition(48, 10);
    cout << "******************";

    setCursorPosition(40, 12);
    cout << "请通过输入1或2来开始游戏或退出程序";

    setCursorPosition(40, 15);
    cout << "开始游戏";
    setCursorPosition(67, 15);
    cout << "退出游戏" << endl;

    setCursorPosition(55, 18);
    cout << "_____";
    setCursorPosition(55, 17);
    cout << ">";
    setCursorPosition(59, 17);
    cout << "<";

    string m;
    setCursorPosition(57, 17);
    cin >> m;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (m.length() > 1) {
        m = "3";
    }

    char n = m[0];
    switch (n) {
    case '1':
        clearScreen();
        init();
        createMap();
        // 初始绘制蛇
        for (const auto& segment : snakeBody) {
            setCursorPosition(segment.at(0), segment.at(1));
            cout << "*";
        }
        drawFood();
        break;
    case '2':
        isRunning = false;
        break;
    default:
        setCursorPosition(45, 19);
        cout << "输入范围错误，请重新输入！";
        _getch();
        showWelcomeScreen();
    }
}

void SnakeGame::showGameOverScreen() {
    clearScreen();

    setCursorPosition(46, 2);
    cout << "\\\\\\|///";
    setCursorPosition(43, 3);
    cout << "\\\\";
    setCursorPosition(47, 3);
    cout << ".-.-";
    setCursorPosition(54, 3);
    cout << "//";

    setCursorPosition(44, 4);
    cout << "(";
    setCursorPosition(47, 4);
    cout << ".@.@";
    setCursorPosition(54, 4);
    cout << ")";

    setCursorPosition(17, 5);
    cout << "+------------------------";
    setCursorPosition(35, 5);
    cout << "o00o";
    setCursorPosition(39, 5);
    cout << "---------";
    setCursorPosition(48, 5);
    cout << "(_)";
    setCursorPosition(51, 5);
    cout << "---------";
    setCursorPosition(61, 5);
    cout << "o00o";
    setCursorPosition(65, 5);
    cout << "-----------------+";

    for (int i = 6; i <= 19; i++) {
        setCursorPosition(17, i);
        cout << "|";
        setCursorPosition(82, i);
        cout << "|";
    }

    setCursorPosition(17, 20);
    cout << "+----------------------------------------------------------------+";

    if (endGameStatus == 1) {
        setCursorPosition(40, 10);
        cout << "你撞进了后室，游戏结束";
    }
    else if (endGameStatus == 2) {
        setCursorPosition(40, 10);
        cout << "你咬到自己了，游戏结束";
    }

    setCursorPosition(44, 13);
    cout << "你的分数: " << score;
    setCursorPosition(44, 15);
    cout << "你的等级: " << getLevel();
}

bool SnakeGame::chooseAction() {
    string m;
    setCursorPosition(31, 24);
    cout << "重新开始";
    setCursorPosition(60, 24);
    cout << "退出游戏";

    setCursorPosition(37, 28);
    cout << "请输入1或2选择重新开始或退出游戏";

    cin >> m;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (m.length() > 1) {
        m = "3";
    }

    char n = m[0];
    switch (n) {
    case '1':
        clearScreen();
        return true;
    case '2':
        return false;
    default:
        setCursorPosition(35, 27);
        cout << "输入错误，请重新输入！";
        _getch();
        clearScreen();
        showGameOverScreen();
        return chooseAction();
    }
}

void SnakeGame::run() {
    showWelcomeScreen();

    while (isRunning) {
        drawScoreAndTips();
        handleInput();
        Sleep(sleeptime);

        if (checkCollision()) {
            end();
            if (!chooseAction()) {
                isRunning = false;
            }
            else {
                init();
                createMap();
                // 初始绘制蛇
                for (const auto& segment : snakeBody) {
                    setCursorPosition(segment.at(0), segment.at(1));
                    cout << "*";
                }
                drawFood();
            }
        }
        else {
            moveSnake();
        }
    }
}

void SnakeGame::end() {
    showGameOverScreen();
}

int main() {
    snake_game::SnakeGame game;
    game.run();
    return 0;
}