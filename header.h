#define NOMINMAX
#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <iostream>
#include <vector>
#include <map>  // 替换utility为map
#include <Windows.h>

namespace snake_game {
    // 游戏基类
    class GameBase {
    protected:
        int score;           // 分数
        int sleeptime;       // 休眠时间(控制速度)
        int add;             // 每吃一次食物增加的分数
        bool isRunning;      // 游戏是否运行
        bool isPaused;       // 游戏是否暂停

    public:
        // 方向枚举
        enum Direction {
            UP = 1,
            DOWN = 2,
            LEFT = 3,
            RIGHT = 4
        };

        GameBase();
        virtual ~GameBase() = default;

        // 纯虚函数，子类必须实现
        virtual void init() = 0;
        virtual void run() = 0;
        virtual void end() = 0;

        void setCursorPosition(int x, int y) const;
        void clearScreen();
        int getLevel() const;
        void pauseGame();
    };

    // 贪吃蛇游戏类，继承游戏基类
    class SnakeGame : public GameBase {
    private:
        Direction currentDir;               // 当前方向
        std::vector<std::map<int, int>> snakeBody;  // 用map存储坐标（key=0表示x，key=1表示y）
        std::map<int, int> food;            // 食物坐标（map替代pair）
        int endGameStatus;                  // 游戏结束状态

        // 私有成员函数
        void createMap() const;
        void drawScoreAndTips() const;
        void generateFood();
        bool checkCollision();
        void moveSnake();
        void handleInput();
        void drawSnake() const;
        void drawFood() const;
        void clearTail();
        void speedUp();
        void speedDown();
        void showWelcomeScreen();
        void showGameOverScreen();
        bool chooseAction();

    public:
        SnakeGame();
        ~SnakeGame() override = default;

        // 重写基类函数
        void init() override;
        void run() override;
        void end() override;
    };
}

#endif // SNAKE_GAME_H