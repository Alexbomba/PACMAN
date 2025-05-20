#pragma once

#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

#pragma comment(lib, "winmm.lib") // Для звука

// ------------------- Структури -------------------

struct Position {
    int x, y;
};

struct Enemy {
    Position pos;
};

struct Game {
    int height, width;
    int** map;
    Position heroPos;
    int coinsCollected;
    int totalCoins;
    Enemy enemies[10];
    int enemyCount;
};

// ------------------- Перечислення -------------------

enum GameObject : short { HALL, WALL, COIN, ENEMY };

enum Color : short {
    BLACK, DARKBLUE, DARKGREEN, TURQUOISE, DARKRED,
    PURPLE, DARKYELLOW, GREY, DARKGREY, BLUE, GREEN,
    CYAN, RED, PINK, YELLOW, WHITE
};

enum Key : short {
    LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80,
    ENTER = 13, SPACE = 32, ESCAPE = 27, BACKSPACE = 8
};

// ------------------- Прототипи функцій -------------------

DWORD WINAPI MusicThread(void* param);
void HideCursor(HANDLE* h);
void GenerateMap(Game* game, const char** layout);
void DrawMap(HANDLE* h, Game* game);
void DrawHero(HANDLE* h, Position* hero);
void ShowCoinCount(HANDLE* h, int* count, int* width);
void MoveEnemies(Game* game, HANDLE* h);

// ------------------- Реалізація -------------------

DWORD WINAPI MusicThread(void* param) {
    PlaySoundA("C:\\Users\\arg50\\Downloads\\IT STEP\\LESSONS\\C++\\PACMAN\\Pac-Man-intro-music", NULL, SND_NODEFAULT | SND_LOOP);
    return 0;
}

void HideCursor(HANDLE* h) {
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = false;
    cursor.dwSize = 100;
    SetConsoleCursorInfo(*h, &cursor);
}

void GenerateMap(Game* game, const char** layout) {
    game->totalCoins = 0;
    game->enemyCount = 0;
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            char ch = layout[y][x];
            switch (ch) {
            case '#': game->map[y][x] = WALL; break;
            case '.': game->map[y][x] = COIN; game->totalCoins++; break;
            case 'E':
                game->map[y][x] = ENEMY;
                if (game->enemyCount < 10) {
                    game->enemies[game->enemyCount++].pos = { x, y };
                }
                break;
            default: game->map[y][x] = HALL;
            }
        }
    }
}

void DrawMap(HANDLE* h, Game* game) {
    for (int y = 0; y < game->height; y++) {
        for (int x = 0; x < game->width; x++) {
            switch (game->map[y][x]) {
            case HALL: SetConsoleTextAttribute(*h, BLACK); cout << " "; break;
            case WALL: SetConsoleTextAttribute(*h, BLUE); cout << (char)178; break;
            case COIN: SetConsoleTextAttribute(*h, YELLOW); cout << "."; break;
            case ENEMY: SetConsoleTextAttribute(*h, RED); cout << "O"; break;
            }
        }
        cout << "\n";
    }
}

void DrawHero(HANDLE* h, Position* hero) {
    COORD pos = { (SHORT)hero->x, (SHORT)hero->y };
    SetConsoleCursorPosition(*h, pos);
    SetConsoleTextAttribute(*h, YELLOW);
    cout << "C";
}

void ShowCoinCount(HANDLE* h, int* count, int* width) {
    COORD info = { (SHORT)(*width + 2), 0 };
    SetConsoleCursorPosition(*h, info);
    SetConsoleTextAttribute(*h, DARKYELLOW);
    cout << "COINS: ";
    SetConsoleTextAttribute(*h, YELLOW);
    cout << *count;
}

void MoveEnemies(Game* game, HANDLE* h) {
    for (int i = 0; i < game->enemyCount; i++) {
        Position& e = game->enemies[i].pos;

        COORD old = { (SHORT)e.x, (SHORT)e.y };
        SetConsoleCursorPosition(*h, old);
        SetConsoleTextAttribute(*h, BLACK);
        cout << " ";
        game->map[e.y][e.x] = HALL;

        int dx = (game->heroPos.x > e.x) ? 1 : (game->heroPos.x < e.x) ? -1 : 0;
        int dy = (game->heroPos.y > e.y) ? 1 : (game->heroPos.y < e.y) ? -1 : 0;

        if (dx != 0 && game->map[e.y][e.x + dx] != WALL && game->map[e.y][e.x + dx] != ENEMY) e.x += dx;
        else if (dy != 0 && game->map[e.y + dy][e.x] != WALL && game->map[e.y + dy][e.x] != ENEMY) e.y += dy;

        if (e.x == game->heroPos.x && e.y == game->heroPos.y) {
            PlaySoundA("C:\\Users\\arg50\\Downloads\\IT STEP\\LESSONS\\C++\\PACMAN\\PacMan-Death-Sound-Effect-HD", NULL, SND_NODEFAULT | SND_LOOP);
            COORD msgPos = { 0, (SHORT)(game->height + 1) };
            SetConsoleCursorPosition(*h, msgPos);
            SetConsoleTextAttribute(*h, RED);
            cout << "YOU WERE CAUGHT BY AN ENEMY! GAME OVER!\n";
            exit(0);
        }

        COORD newPos = { (SHORT)e.x, (SHORT)e.y };
        SetConsoleCursorPosition(*h, newPos);
        SetConsoleTextAttribute(*h, RED);
        cout << "O";
        game->map[e.y][e.x] = ENEMY;
    }
}