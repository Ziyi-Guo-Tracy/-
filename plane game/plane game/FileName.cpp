#include <graphics.h>
#include <conio.h>
#include <vector>
#include <ctime>
#include <cstdlib>

// 游戏常量定义
const int WIDTH = 480;          // 游戏窗口宽度
const int HEIGHT = 700;         // 游戏窗口高度
const int PLAYER_SPEED = 5;     // 玩家移动速度
const int ENEMY_SPEED = 2;      // 敌机移动速度
const int BULLET_SPEED = 8;     // 子弹移动速度
const int ENEMY_CREATE_FREQ = 30; // 敌机生成频率

// 游戏对象结构体
struct GameObject {
    int x, y;           // 位置
    int width, height;   // 尺寸
    int speed;           // 速度
    bool active;         // 是否活跃
    IMAGE* img;          // 图像指针
};

// 游戏状态
struct GameState {
    GameObject player;           // 玩家飞机
    std::vector<GameObject> enemies;  // 敌机列表
    std::vector<GameObject> bullets;  // 子弹列表
    int score;                   // 得分
    int lives;                   // 生命值
    bool gameOver;               // 游戏是否结束
    bool showAwesome; // 达到1000分时显示“你真棒”
    DWORD awesomeTime; // 记录显示“你真棒”的开始时间
    bool win; // 达到5000分时显示“你赢了”
};

// 图像对象
IMAGE playerImg, enemyImg;

// 初始化游戏
void InitGame(GameState& game) {
    static bool imgLoaded = false;
    if (!imgLoaded) {
        loadimage(&playerImg, _T("C:\\Users\\xm\\source\\repos\\plane game\\x64\\Debug\\playerImg.bmp"));
        loadimage(&enemyImg, _T("C:\\Users\\xm\\source\\repos\\plane game\\x64\\Debug\\enemyImg.bmp"));

        if (playerImg.getwidth() == 0 || playerImg.getheight() == 0) {
            MessageBox(NULL, _T("playerImg.bmp 加载失败！"), _T("错误"), MB_OK);
        }
        if (enemyImg.getwidth() == 0 || enemyImg.getheight() == 0) {
            MessageBox(NULL, _T("enemyImg.bmp 加载失败！"), _T("错误"), MB_OK);
        }

        imgLoaded = true;
    }

    // 初始化玩家
    game.player.x = WIDTH / 2;
    game.player.y = HEIGHT - 100;
    game.player.width = 50;
    game.player.height = 50;
    game.player.speed = PLAYER_SPEED;
    game.player.active = true;
    game.player.img = &playerImg; // 赋值图片指针

    // 初始化游戏状态
    game.score = 0;
    game.lives = 3;
    game.gameOver = false;
    game.showAwesome = false; // 达到1000分时显示“你真棒”
    game.awesomeTime = 0; // 记录显示“你真棒”的开始时间
    game.win = false; // 达到5000分时显示“你赢了”

    // 清空敌机和子弹
    game.enemies.clear();
    game.bullets.clear();
}

// 创建敌机
void CreateEnemy(GameState& game) {
    if (rand() % ENEMY_CREATE_FREQ != 0) return;

    GameObject enemy;
    enemy.x = rand() % (WIDTH - 50);
    enemy.y = -50;
    enemy.width = 50;
    enemy.height = 50;
    enemy.speed = ENEMY_SPEED + rand() % 3;
    enemy.active = true;
    enemy.img = &enemyImg; // 赋值图片指针

    game.enemies.push_back(enemy);
}

// 玩家发射子弹
void PlayerShoot(GameState& game) {
    GameObject bullet;
    bullet.x = game.player.x + game.player.width / 2 - 5;
    bullet.y = game.player.y;
    bullet.width = 10;
    bullet.height = 20;
    bullet.speed = BULLET_SPEED;
    bullet.active = true;

    game.bullets.push_back(bullet);
}

// 更新玩家位置
void UpdatePlayer(GameState& game) {
    // 键盘控制
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        game.player.x -= game.player.speed;
        if (game.player.x < 0) game.player.x = 0;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        game.player.x += game.player.speed;
        if (game.player.x > WIDTH - game.player.width) game.player.x = WIDTH - game.player.width;
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        game.player.y -= game.player.speed;
        if (game.player.y < 0) game.player.y = 0;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        game.player.y += game.player.speed;
        if (game.player.y > HEIGHT - game.player.height) game.player.y = HEIGHT - game.player.height;
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        static int shootDelay = 0;
        if (shootDelay <= 0) {
            PlayerShoot(game);
            shootDelay = 10; // 射击冷却
        }
        else {
            shootDelay--;
        }
    }
}

// 更新敌机位置
void UpdateEnemies(GameState& game) {
    for (auto& enemy : game.enemies) {
        if (!enemy.active) continue;

        enemy.y += enemy.speed;

        // 敌机飞出屏幕
        if (enemy.y > HEIGHT) {
            enemy.active = false;
        }
    }

    // 移除不活跃的敌机
    game.enemies.erase(
        std::remove_if(game.enemies.begin(), game.enemies.end(),
            [](const GameObject& obj) { return !obj.active; }),
        game.enemies.end());
}

// 更新子弹位置
void UpdateBullets(GameState& game) {
    for (auto& bullet : game.bullets) {
        if (!bullet.active) continue;

        bullet.y -= bullet.speed;

        // 子弹飞出屏幕
        if (bullet.y < 0) {
            bullet.active = false;
        }
    }

    // 移除不活跃的子弹
    game.bullets.erase(
        std::remove_if(game.bullets.begin(), game.bullets.end(),
            [](const GameObject& obj) { return !obj.active; }),
        game.bullets.end());
}

// 碰撞检测
void CheckCollisions(GameState& game) {
    // 子弹与敌机碰撞
    for (auto& bullet : game.bullets) {
        if (!bullet.active) continue;

        for (auto& enemy : game.enemies) {
            if (!enemy.active) continue;

            // 简单的矩形碰撞检测
            if (bullet.x < enemy.x + enemy.width &&
                bullet.x + bullet.width > enemy.x &&
                bullet.y < enemy.y + enemy.height &&
                bullet.y + bullet.height > enemy.y) {

                // 碰撞发生
                bullet.active = false;
                enemy.active = false;
                game.score += 10;
                break;
            }
        }
    }

    // 玩家与敌机碰撞
    for (auto& enemy : game.enemies) {
        if (!enemy.active) continue;

        if (game.player.x < enemy.x + enemy.width &&
            game.player.x + game.player.width > enemy.x &&
            game.player.y < enemy.y + enemy.height &&
            game.player.y + game.player.height > enemy.y) {

            // 碰撞发生
            enemy.active = false;
            game.lives--;

            if (game.lives <= 0) {
                game.gameOver = true;
            }
        }
    }
}

// 检测分数奖励和胜利
void CheckScore(GameState& game) {
    if (!game.showAwesome && game.score >= 1000 && game.score < 5000) {
        game.showAwesome = true;
        game.awesomeTime = GetTickCount();
    }
    if (!game.win && game.score >= 5000) {
        game.win = true;
        game.gameOver = true;
    }
}

// 绘制游戏
void DrawGame(GameState& game) {
    // 清空屏幕
    cleardevice();

    // 绘制背景
    setbkcolor(RGB(0, 0, 64));
    clearrectangle(0, 0, WIDTH, HEIGHT);

    // 绘制玩家
    if (game.player.img)
        putimage(game.player.x, game.player.y, game.player.img, SRCCOPY); // 改为SRCCOPY
    else {
        setfillcolor(GREEN);
        fillrectangle(game.player.x, game.player.y,
            game.player.x + game.player.width,
            game.player.y + game.player.height);
    }

    // 绘制敌机
    for (auto& enemy : game.enemies) {
        if (enemy.active) {
            if (enemy.img)
                putimage(enemy.x, enemy.y, enemy.img, SRCCOPY); // 改为SRCCOPY
            else {
                setfillcolor(RED);
                fillrectangle(enemy.x, enemy.y,
                    enemy.x + enemy.width,
                    enemy.y + enemy.height);
            }
        }
    }

    // 绘制子弹
    setfillcolor(YELLOW);
    for (auto& bullet : game.bullets) {
        if (bullet.active) {
            fillrectangle(bullet.x, bullet.y,
                bullet.x + bullet.width,
                bullet.y + bullet.height);
        }
    }

    // 绘制UI
    settextcolor(WHITE);
    settextstyle(20, 0, _T("宋体"));
    TCHAR scoreStr[32];
    _stprintf_s(scoreStr, _T("得分: %d"), game.score);
    outtextxy(10, 10, scoreStr);

    TCHAR livesStr[32];
    _stprintf_s(livesStr, _T("生命: %d"), game.lives);
    outtextxy(10, 40, livesStr);

    // 新增分数奖励显示
    if (game.showAwesome && !game.win) {
        DWORD now = GetTickCount();
        if (now - game.awesomeTime < 3000) {
            settextcolor(RGB(255, 215, 0));
            settextstyle(40, 0, _T("宋体"));
            outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 80, _T("你真棒！"));
        } else {
            game.showAwesome = false;
        }
    }
    // 游戏胜利显示
    if (game.win) {
        settextcolor(RGB(0, 255, 0));
        settextstyle(40, 0, _T("宋体"));
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, _T("你赢了！"));
        settextstyle(20, 0, _T("宋体"));
        TCHAR finalScoreStr[32];
        _stprintf_s(finalScoreStr, _T("最终得分: %d"), game.score);
        outtextxy(WIDTH / 2 - 70, HEIGHT / 2 + 30, finalScoreStr);
    }

    // 游戏结束显示
    if (game.gameOver) {
        settextcolor(RED);
        settextstyle(40, 0, _T("宋体"));
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, _T("游戏结束"));

        settextstyle(20, 0, _T("宋体"));
        TCHAR finalScoreStr[32];
        _stprintf_s(finalScoreStr, _T("最终得分: %d"), game.score);
        outtextxy(WIDTH / 2 - 70, HEIGHT / 2 + 30, finalScoreStr);

        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 + 70, _T("按R键重新开始"));
    }
}

// 主函数
int main() {
    // 初始化图形窗口
    initgraph(WIDTH, HEIGHT);
    srand((unsigned)time(NULL));

    // 游戏状态
    GameState game;
    InitGame(game);

    // 游戏主循环
    while (true) {
        // 处理输入
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'r' || ch == 'R') {
                if (game.gameOver) {
                    InitGame(game);
                }
            }
        }

        // 游戏逻辑更新
        if (!game.gameOver) {
            UpdatePlayer(game);
            CreateEnemy(game);
            UpdateEnemies(game);
            UpdateBullets(game);
            CheckCollisions(game);
            CheckScore(game); // 新增分数检测
        }

        // 绘制
        DrawGame(game);

        // 控制帧率
        Sleep(16); // 约60FPS
    }

    // 关闭图形窗口
    closegraph();
    return 0;
    
}