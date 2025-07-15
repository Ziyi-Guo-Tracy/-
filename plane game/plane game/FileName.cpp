#include <graphics.h>
#include <conio.h>
#include <vector>
#include <ctime>
#include <cstdlib>

// ��Ϸ��������
const int WIDTH = 480;          // ��Ϸ���ڿ��
const int HEIGHT = 700;         // ��Ϸ���ڸ߶�
const int PLAYER_SPEED = 5;     // ����ƶ��ٶ�
const int ENEMY_SPEED = 2;      // �л��ƶ��ٶ�
const int BULLET_SPEED = 8;     // �ӵ��ƶ��ٶ�
const int ENEMY_CREATE_FREQ = 30; // �л�����Ƶ��

// ��Ϸ����ṹ��
struct GameObject {
    int x, y;           // λ��
    int width, height;   // �ߴ�
    int speed;           // �ٶ�
    bool active;         // �Ƿ��Ծ
    IMAGE* img;          // ͼ��ָ��
};

// ��Ϸ״̬
struct GameState {
    GameObject player;           // ��ҷɻ�
    std::vector<GameObject> enemies;  // �л��б�
    std::vector<GameObject> bullets;  // �ӵ��б�
    int score;                   // �÷�
    int lives;                   // ����ֵ
    bool gameOver;               // ��Ϸ�Ƿ����
    bool showAwesome; // �ﵽ1000��ʱ��ʾ���������
    DWORD awesomeTime; // ��¼��ʾ����������Ŀ�ʼʱ��
    bool win; // �ﵽ5000��ʱ��ʾ����Ӯ�ˡ�
};

// ͼ�����
IMAGE playerImg, enemyImg;

// ��ʼ����Ϸ
void InitGame(GameState& game) {
    static bool imgLoaded = false;
    if (!imgLoaded) {
        loadimage(&playerImg, _T("C:\\Users\\xm\\source\\repos\\plane game\\x64\\Debug\\playerImg.bmp"));
        loadimage(&enemyImg, _T("C:\\Users\\xm\\source\\repos\\plane game\\x64\\Debug\\enemyImg.bmp"));

        if (playerImg.getwidth() == 0 || playerImg.getheight() == 0) {
            MessageBox(NULL, _T("playerImg.bmp ����ʧ�ܣ�"), _T("����"), MB_OK);
        }
        if (enemyImg.getwidth() == 0 || enemyImg.getheight() == 0) {
            MessageBox(NULL, _T("enemyImg.bmp ����ʧ�ܣ�"), _T("����"), MB_OK);
        }

        imgLoaded = true;
    }

    // ��ʼ�����
    game.player.x = WIDTH / 2;
    game.player.y = HEIGHT - 100;
    game.player.width = 50;
    game.player.height = 50;
    game.player.speed = PLAYER_SPEED;
    game.player.active = true;
    game.player.img = &playerImg; // ��ֵͼƬָ��

    // ��ʼ����Ϸ״̬
    game.score = 0;
    game.lives = 3;
    game.gameOver = false;
    game.showAwesome = false; // �ﵽ1000��ʱ��ʾ���������
    game.awesomeTime = 0; // ��¼��ʾ����������Ŀ�ʼʱ��
    game.win = false; // �ﵽ5000��ʱ��ʾ����Ӯ�ˡ�

    // ��յл����ӵ�
    game.enemies.clear();
    game.bullets.clear();
}

// �����л�
void CreateEnemy(GameState& game) {
    if (rand() % ENEMY_CREATE_FREQ != 0) return;

    GameObject enemy;
    enemy.x = rand() % (WIDTH - 50);
    enemy.y = -50;
    enemy.width = 50;
    enemy.height = 50;
    enemy.speed = ENEMY_SPEED + rand() % 3;
    enemy.active = true;
    enemy.img = &enemyImg; // ��ֵͼƬָ��

    game.enemies.push_back(enemy);
}

// ��ҷ����ӵ�
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

// �������λ��
void UpdatePlayer(GameState& game) {
    // ���̿���
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
            shootDelay = 10; // �����ȴ
        }
        else {
            shootDelay--;
        }
    }
}

// ���µл�λ��
void UpdateEnemies(GameState& game) {
    for (auto& enemy : game.enemies) {
        if (!enemy.active) continue;

        enemy.y += enemy.speed;

        // �л��ɳ���Ļ
        if (enemy.y > HEIGHT) {
            enemy.active = false;
        }
    }

    // �Ƴ�����Ծ�ĵл�
    game.enemies.erase(
        std::remove_if(game.enemies.begin(), game.enemies.end(),
            [](const GameObject& obj) { return !obj.active; }),
        game.enemies.end());
}

// �����ӵ�λ��
void UpdateBullets(GameState& game) {
    for (auto& bullet : game.bullets) {
        if (!bullet.active) continue;

        bullet.y -= bullet.speed;

        // �ӵ��ɳ���Ļ
        if (bullet.y < 0) {
            bullet.active = false;
        }
    }

    // �Ƴ�����Ծ���ӵ�
    game.bullets.erase(
        std::remove_if(game.bullets.begin(), game.bullets.end(),
            [](const GameObject& obj) { return !obj.active; }),
        game.bullets.end());
}

// ��ײ���
void CheckCollisions(GameState& game) {
    // �ӵ���л���ײ
    for (auto& bullet : game.bullets) {
        if (!bullet.active) continue;

        for (auto& enemy : game.enemies) {
            if (!enemy.active) continue;

            // �򵥵ľ�����ײ���
            if (bullet.x < enemy.x + enemy.width &&
                bullet.x + bullet.width > enemy.x &&
                bullet.y < enemy.y + enemy.height &&
                bullet.y + bullet.height > enemy.y) {

                // ��ײ����
                bullet.active = false;
                enemy.active = false;
                game.score += 10;
                break;
            }
        }
    }

    // �����л���ײ
    for (auto& enemy : game.enemies) {
        if (!enemy.active) continue;

        if (game.player.x < enemy.x + enemy.width &&
            game.player.x + game.player.width > enemy.x &&
            game.player.y < enemy.y + enemy.height &&
            game.player.y + game.player.height > enemy.y) {

            // ��ײ����
            enemy.active = false;
            game.lives--;

            if (game.lives <= 0) {
                game.gameOver = true;
            }
        }
    }
}

// ������������ʤ��
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

// ������Ϸ
void DrawGame(GameState& game) {
    // �����Ļ
    cleardevice();

    // ���Ʊ���
    setbkcolor(RGB(0, 0, 64));
    clearrectangle(0, 0, WIDTH, HEIGHT);

    // �������
    if (game.player.img)
        putimage(game.player.x, game.player.y, game.player.img, SRCCOPY); // ��ΪSRCCOPY
    else {
        setfillcolor(GREEN);
        fillrectangle(game.player.x, game.player.y,
            game.player.x + game.player.width,
            game.player.y + game.player.height);
    }

    // ���Ƶл�
    for (auto& enemy : game.enemies) {
        if (enemy.active) {
            if (enemy.img)
                putimage(enemy.x, enemy.y, enemy.img, SRCCOPY); // ��ΪSRCCOPY
            else {
                setfillcolor(RED);
                fillrectangle(enemy.x, enemy.y,
                    enemy.x + enemy.width,
                    enemy.y + enemy.height);
            }
        }
    }

    // �����ӵ�
    setfillcolor(YELLOW);
    for (auto& bullet : game.bullets) {
        if (bullet.active) {
            fillrectangle(bullet.x, bullet.y,
                bullet.x + bullet.width,
                bullet.y + bullet.height);
        }
    }

    // ����UI
    settextcolor(WHITE);
    settextstyle(20, 0, _T("����"));
    TCHAR scoreStr[32];
    _stprintf_s(scoreStr, _T("�÷�: %d"), game.score);
    outtextxy(10, 10, scoreStr);

    TCHAR livesStr[32];
    _stprintf_s(livesStr, _T("����: %d"), game.lives);
    outtextxy(10, 40, livesStr);

    // ��������������ʾ
    if (game.showAwesome && !game.win) {
        DWORD now = GetTickCount();
        if (now - game.awesomeTime < 3000) {
            settextcolor(RGB(255, 215, 0));
            settextstyle(40, 0, _T("����"));
            outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 80, _T("�������"));
        } else {
            game.showAwesome = false;
        }
    }
    // ��Ϸʤ����ʾ
    if (game.win) {
        settextcolor(RGB(0, 255, 0));
        settextstyle(40, 0, _T("����"));
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, _T("��Ӯ�ˣ�"));
        settextstyle(20, 0, _T("����"));
        TCHAR finalScoreStr[32];
        _stprintf_s(finalScoreStr, _T("���յ÷�: %d"), game.score);
        outtextxy(WIDTH / 2 - 70, HEIGHT / 2 + 30, finalScoreStr);
    }

    // ��Ϸ������ʾ
    if (game.gameOver) {
        settextcolor(RED);
        settextstyle(40, 0, _T("����"));
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, _T("��Ϸ����"));

        settextstyle(20, 0, _T("����"));
        TCHAR finalScoreStr[32];
        _stprintf_s(finalScoreStr, _T("���յ÷�: %d"), game.score);
        outtextxy(WIDTH / 2 - 70, HEIGHT / 2 + 30, finalScoreStr);

        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 + 70, _T("��R�����¿�ʼ"));
    }
}

// ������
int main() {
    // ��ʼ��ͼ�δ���
    initgraph(WIDTH, HEIGHT);
    srand((unsigned)time(NULL));

    // ��Ϸ״̬
    GameState game;
    InitGame(game);

    // ��Ϸ��ѭ��
    while (true) {
        // ��������
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'r' || ch == 'R') {
                if (game.gameOver) {
                    InitGame(game);
                }
            }
        }

        // ��Ϸ�߼�����
        if (!game.gameOver) {
            UpdatePlayer(game);
            CreateEnemy(game);
            UpdateEnemies(game);
            UpdateBullets(game);
            CheckCollisions(game);
            CheckScore(game); // �����������
        }

        // ����
        DrawGame(game);

        // ����֡��
        Sleep(16); // Լ60FPS
    }

    // �ر�ͼ�δ���
    closegraph();
    return 0;
    
}