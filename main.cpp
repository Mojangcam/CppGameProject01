#include <stdio.h>
#include <Windows.h>
#include <malloc.h>
#include <stddef.h>
#include <time.h>
#include <iostream>
#include <random>

#define MAP_SIZE_HEIGHT             40  // 맵의 높이를 정의
#define MAP_SIZE_WIDTH              40  // 맵의 너비를 정의
#define POINT_PRINT_POSITION_X      1   // 점수를 표시하는 X 좌표를 정의
#define POINT_PRINT_POSITION_Y      42  // 점수를 표시하는 Y 좌표를 정의
#define COOLTIME_PRINT_POSITION_X   1   // 쿨타임을 표시하는 X 좌표를 정의
#define COOLTIME_PRINT_POSITION_Y   43  // 쿨타임을 표시하는 Y 좌표를 정의

using namespace std;

int gPosPointX;                         // 점수의 X 좌표를 담는 변수
int gPosPointY;                         // 점수의 Y 좌표를 담는 변수
bool isGetPointJump = false;            // 점수를 얻으려면 점프를 해야 하는지 여부를 담는 변수

struct Player {
    int x, y;                           // 플레이어의 현재 위치를 담는 변수
    const char* shape;                  // 플레이어의 기호 또는 문자를 담는 변수
    float initialVelocity = 5;          // 처음 속도를 나타내는 변수
    bool isJumping = false;             // 플레이어가 현재 점프 중인지 여부를 담는 변수
    float elapsedTime = 0.0;            // 경과 시간을 담는 변수
    float gravity = 9.8;                // 중력 가속도를 담는 변수
    float height = 0;                   // 플레이어의 현재 높이를 담는 변수
    int score = 0;                      // 플레이어의 점수를 담는 변수
    unsigned long dashCooldown = 5000;  // 대쉬 스킬의 쿨타임 (단위: ms)
    bool isDashUsed = false;            // 대쉬 스킬 사용 여부를 확인하는 변수
    unsigned long dashRemainingTime;    // 대쉬 스킬의 남은 쿨타임 (단위: ms)
    bool isCooltimeDisplayed = false;   // 쿨타임이 표시되었는지 여부를 확인하는 변수
    unsigned long dieTimer = 10000;     // 설정한 시간 안에 점수를 획득하지 못하면 게임이 끝나는 시스템에 필요한 변수
};

Player* gamePlayer = nullptr;





int map[MAP_SIZE_WIDTH][MAP_SIZE_HEIGHT] =
{
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

};


void moveCharPos(int x, int y);                                             // 콘솔창에서 커서의 위치를 설정하는 함수
void initPlayer();                                                          // 플레이어를 선언 및 초기화하는 함수
void randomPointGenerator();                                                // 점수를 랜덤한 위치에 놓아주는 함수
void block();                                                               // 맵의 외곽선 밖으로 빠져나가지 못하게 하는 함수
void playerKey();                                                           // 플레이어를 조작하기 위한 함수
void moveChar(Player* player, int x, int y);                                // 플레이어를 해당 위치로 옮기게 하는 함수


int main()                                                                  // 메인메인메에에에에인 함수
{
    unsigned long start, end;                                               // 실시간으로 보이게 하기 위한 노력..
    system("pause");                                                        // 처음 실행 시 콘솔창이 작아서 콘솔창을 늘리거나 키운 후 실행하도록 하기 위한 코드
    system("cls");                                                          // 콘솔 내역 지우는 코드
    moveCharPos(COOLTIME_PRINT_POSITION_X, COOLTIME_PRINT_POSITION_Y);      // 콘솔의 커서위치를 설정하는 코드(원래같으면 콘솔 다 지우고 다시 써야하는것을 설정한 위치에서부터 쓸수 있음)
    printf("Dash Skill Ready!           ");                                 // 처음 시작하면 스킬은 
    for (int y = 0; y < MAP_SIZE_HEIGHT; y++)                               // 맵을 그리는 코드 (1은 빈칸 0은 벽)
    {       
        for (int x = 0; x < MAP_SIZE_WIDTH; x++)                            // 맵을 그리는 코드~
        {
            switch (map[y][x])
            {
            case 1:

                break;
            case 0:
                moveCharPos(x, y);
                printf("■");
                break;
            default:
                break;
            }
        }
    }
    randomPointGenerator();                                                 // 처음 시작하면 점수부터 나타내기
    initPlayer();                                                           // 플레이어를 생성학고 초기화 하기!
    while (true)                                                            // 무한반복문을 사용해서 프로그램이 계속 돌아가도록 
    {
        start = clock();                                                    // 쿨타임을 계산하기 위한 변수
        while (gamePlayer->isDashUsed) {                                    // 대쉬 스킬을 사용한 경우
            end = clock();                                                  // start는 while들어오기 전 값이 유지가 되면서 end값은 실시간으로 업데이트되며 값이 점점 커진다
                                                                            // end는 start보다 크거나 같을수 밖에 없다
            if (end - start > gamePlayer->dashCooldown) {                   // end - start는 while문 들어온 시점부터(스킬을 사용한 시점부터)얼만큼 지났는지를 나타냄
                                                                            // 스킬을 사용한 시점으로부터 dashCooldown (default 5000) 만큼 이상 지났는지 확인
                gamePlayer->isDashUsed = false;                             // 쿨타임이 다 지나면 플레이어의 대쉬사용 상태를 false로 재정의
                break;                                                      // while 안녕~
            }
            gamePlayer->dashRemainingTime = gamePlayer->dashCooldown - (end - start);                       // 그렇지 않으면 (쿨타임이 지나지 않으면) 남은 쿨타임을 계산
            moveCharPos(COOLTIME_PRINT_POSITION_X, COOLTIME_PRINT_POSITION_Y);                              // 콘솔창에 설정한 쿨타임 출력 위치로 커서를 옮긴 후
            printf("Dash CoolTime : %.1f Seconds        ", (float)gamePlayer->dashRemainingTime / 1000);    // 남은 쿨타임은 ms단위이기 때문에 1000으로 나눈 후 소수점 1자리까지 표시한 후 출력하기
            gamePlayer->isCooltimeDisplayed = true;                                                         // 쿨타임이 화면에 표시되었는지를 확인하는 코드
            playerKey();                                                                                    // 플레이어를 제어하기 위한 키보드 입력 받기
            block();                                                                                        // 플레이어가 벽을 넘지 못하도록 막는 함수 (굳이 필요할까..)
            Sleep(50);                                                                                      // 너무 빨리 실행하면 캐릭터가 소닉이 되어버리기 때문에 sleep 사용
        }
        if (gamePlayer->isCooltimeDisplayed) {                                                              // 만약 쿨타임이 화면에 표시되었다면 실행
            gamePlayer->isCooltimeDisplayed = false;                                                        // 쿨타임을 표시하는게 아니기 때문에 false로 재정의
            moveCharPos(COOLTIME_PRINT_POSITION_X, COOLTIME_PRINT_POSITION_Y);                              // 콘솔창에 설정한 쿨타임 출력 위치로 커서를 이동
            printf("Dash Skill Ready!           ");                                                         // 쿨타임이 이미 지났기 때문에 Dash Skill Ready라는 문구 출력 띄어쓰기 하는 이유는 쿨타임 출력할때 문자열 길이가 길어서 그거까지 없애기 위해 충분한 띄어쓰기!
        }
        playerKey();        // 플레이어를 제어하기위한 키보드 입력받고 처리하는 코드
        block();            // 플레이어가 벽을 뚫지 못하도록하는 함수
        Sleep(50);          // 소닉 방지
    }

    return 0;
}

void moveChar(Player* player, int x, int y) {                               // 캐릭터를 해당 위치로 옮기는 함수
    if (x == gPosPointX && y == gPosPointY) {                               // 점수와 같은 위치에 플레이어가 있을 경우
        if (isGetPointJump == player->isJumping) {                          // 포인트를 점프해야지만 획득이 가능한지와 현재 플레이어의 점프상태를 비교하여 점프로 획득시 1점 추가하는 코드
            moveCharPos(POINT_PRINT_POSITION_X, POINT_PRINT_POSITION_Y);
            (*player).score += 1;
            printf("Score : %d     ", player->score);
            randomPointGenerator();
        }
        else {                                                              // 그렇지 않을경우에는 10점 감점
            moveCharPos(POINT_PRINT_POSITION_X, POINT_PRINT_POSITION_Y);
            player->score -= 10;
            if (player->score < 0) { player->score = 0; }
            printf("Score : %d     ", player->score);
            randomPointGenerator();
        }
    }
    if (map[y][x] == 0) {                                                   // 이동하려는 위치가 벽또는 땅이면
        if (map[y - 1][x]) {                                                // y - 1이 벽또는 땅위 이면
            player->isJumping= false;                                       // 땅 위에 서 있다는 뜻이니 점프 상태를 false로 설정
        }
        return;                                                             // 함수 끝~
    }
    if (!map[y + 1][x] && player->isJumping) {                              // y + 1 (내가 밟고 있는 곳이)땅이고, 점프를 하고 있는 상태였다면 점프상태를 false로 설정
        player->isJumping = false;
    }
    if (x > 38) {                                                           // 맵 밖으로 나가려고 하면 차단하는 코드 182 ~ 213line까지
        moveCharPos(player->x, player->y);
        printf("  ");
        player->x = 38;
        player->y = y;
        moveCharPos(player->x, player->y);
        printf("%s", player->shape);
    }
    else if (x < 1) {
        moveCharPos(player->x, player->y);
        printf("  ");
        player->x = 1;
        player->y = y;
        moveCharPos(player->x, player->y);
        printf("%s", player->shape);
    }
    else if (y > 38) {
        moveCharPos(player->x, player->y);
        printf("  ");
        player->x = x;
        player->y = 38;
        moveCharPos(player->x, player->y);
        printf("%s", player->shape);
    }
    else if (y < 1) {
        moveCharPos(player->x, player->y);
        printf("  ");
        player->x = x;
        player->y = 1;
        moveCharPos(player->x, player->y);
        printf("%s", player->shape);
    }
    else {                                                                  // 원하는 위치로 이동시키는 코드
        moveCharPos(player->x, player->y);
        printf("  ");
        player->x = x;
        player->y = y;

        moveCharPos(player->x, player->y);
        printf("%s", player->shape);
    }
}

void randomPointGenerator() {                                               // 랜덤한 위치로 점수를 생성하는 함수
    while (1) {                                                             // 내가 있는 위치와 겹칠수 있어서 while문 사용하여 겹치지 않을때 까지 반복
        random_device posRandomDeviceX;                                     // 랜덤 라이브러리사용은 처음이라 조금 이상할수 있음
        mt19937 posX(posRandomDeviceX());
        random_device posRandomDeviceY;
        mt19937 posY(posRandomDeviceY());
        random_device pointJump;
        mt19937 isPointJump(pointJump());
        uniform_int_distribution<int> dis(1, 38);
        uniform_int_distribution<int> point(0, 1);
        gPosPointX = dis(posX);                                             // 랜덤한 좌표 외곽벽 좌표를 제외한 좌표설정
        gPosPointY = dis(posY);
        int isJumpPoints = point(isPointJump);
        if (map[gPosPointY][gPosPointX]) {                                  // 해당 좌표가 벽이 아닌지 확인
            moveCharPos(gPosPointX, gPosPointY);                            // 아니라면 그 자리에 커서 옮김
            if (!isJumpPoints) {                                            // 점프로 획득해야하는게 아니라면
                printf("☆");                                               // ☆출력
                isGetPointJump = false;                                     // jump로 획득해야하는지를 담고있는 변수를 false로 설정
            }
            else {                                                          // 점프로 획득해야한다고 하면
                for (int i = 0; i < 2; i++) {                               // 점프로 닿는 거리인지 확인 (천장에 있는데 점프로 획득하라고 하면 할 수 없기 때문에 걸러주는 부분)
                    if (map[gPosPointY + i][gPosPointX] == 0) {             // 점수를 놓는 위치가 점프로 닿는 거리이면서 벽이 아닌지 확인
                        printf("★");                                       // 조건을 만족시키면 ★출력
                        isGetPointJump = true;                              // jump로 획득해야하는지를 담고있는 변수를 true로 설정
                        return;                                             // 굳바이 for
                    }
                }
                printf("☆");                                               // 닿지 않는 거리라면 ☆로 출력하면서
                isGetPointJump = false;                                     // jump로 획득해야하는지를 담고있는 변수를 false로 설정
            }
            return;                                                         // while 안녕~
        }
    }
}

void playerKey()                                                            // 키보드의 입력을 받아 이동하는 함수
{
    if (GetAsyncKeyState(VK_LEFT))                                          // 왼쪽 화살표 누르면
    {
        moveChar(gamePlayer, gamePlayer->x - 1, gamePlayer->y);             // 왼쪽으로 이동하는 코드
    }
    if (GetAsyncKeyState(VK_RIGHT))
    {
        moveChar(gamePlayer, gamePlayer->x + 1, gamePlayer->y);
    }
    if (GetAsyncKeyState(VK_UP))
    {
        moveChar(gamePlayer, gamePlayer->x, gamePlayer->y - 1);
    }
    if (GetAsyncKeyState(VK_DOWN))
    {
        moveChar(gamePlayer, gamePlayer->x, gamePlayer->y + 1);
    }
    if (GetAsyncKeyState(VK_SPACE))                                         // 스페이스바 누르면 점프이지만 대쉬도 같이 사용가능
    {
        if (gamePlayer->isJumping == false)                                 // 현재 상태가 점프를 하지 않는 상태면
        {
            if (map[gamePlayer->y + 1][gamePlayer->x]) {                    // 내 위치 바로 아래가 땅이 아니면
                if (!gamePlayer->isDashUsed) {                              // 대쉬를 사용하지 않은 상태면
                    gamePlayer->isDashUsed = true;                          // 대쉬를 사용한 상태로 변경
                    if (GetAsyncKeyState(VK_LEFT)) {                        // 키보드의 입력에 따라 어느 방향으로 이동할지 결정
                        moveChar(gamePlayer, gamePlayer->x - 5, gamePlayer->y);
                    }
                    else if (GetAsyncKeyState(VK_RIGHT)) {
                        moveChar(gamePlayer, gamePlayer->x + 5, gamePlayer->y);
                    }
                    else if (GetAsyncKeyState(VK_UP)) {
                        moveChar(gamePlayer, gamePlayer->x, gamePlayer->y - 5);
                    }
                    else if (GetAsyncKeyState(VK_DOWN)) {
                        moveChar(gamePlayer, gamePlayer->x, gamePlayer->y + 5);
                    }
                }
                return;
            }
            gamePlayer->isJumping = true;                                   // 땅 위에 서 있다면 점프하는것으로 동작
            gamePlayer->height = gamePlayer->y;
            gamePlayer->elapsedTime = 0;
        }

    }
    if (gamePlayer->isJumping)      // 만약 점프하는 상태이면
    {
        // 물리학에서 점프하면 중력과 처음속도에 따라 움직이게하는 로직
        gamePlayer->elapsedTime += 0.1f;
        float change = -(gamePlayer->initialVelocity * gamePlayer->elapsedTime) + 0.5f * gamePlayer->gravity * gamePlayer->elapsedTime * gamePlayer->elapsedTime;
        //gamePlayer->PlayerY = hight + change;
        if (gamePlayer->height + change > 38) { moveChar(gamePlayer, gamePlayer->x, 38); }
        else { moveChar(gamePlayer, gamePlayer->x, gamePlayer->height + change); }
    }
}

void moveCharPos(int x, int y)      // 콘솔창 커서 위치를 조정하는 함수
{
    COORD pos;
    pos.X = x * 2;
    pos.Y = y;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void initPlayer()
{
    gamePlayer = (Player*)malloc(sizeof(Player));
    gamePlayer->x = 10;
    gamePlayer->y= 10;
    gamePlayer->shape = "웃";
    gamePlayer->initialVelocity = 5;          // 처음 속도를 나타내는 변수
    gamePlayer->isJumping = false;             // 플레이어가 현재 점프 중인지 여부를 담는 변수
    gamePlayer->elapsedTime = 0.0;            // 경과 시간을 담는 변수
    gamePlayer->gravity = 9.8;                // 중력 가속도를 담는 변수
    gamePlayer->height = 0;                   // 플레이어의 현재 높이를 담는 변수
    gamePlayer->score = 0;                      // 플레이어의 점수를 담는 변수
    gamePlayer->dashCooldown = 5000;
    gamePlayer->isDashUsed = false;
    gamePlayer->dashRemainingTime;
    gamePlayer->isCooltimeDisplayed = false;
    gamePlayer->dieTimer = 10000;
    moveChar(gamePlayer, 10, 10);
}

void block()
{
    if (gamePlayer->x <= 0)
    {

        gamePlayer->x++;
    }

    if (gamePlayer->x >= 39)
    {

        gamePlayer->x--;
    }
    if (gamePlayer->y <= 0)
    {

        gamePlayer->y++;
    }
    if (gamePlayer->y >= 39)
    {

        gamePlayer->y = 38;
        gamePlayer->gravity = 0;
    }
}
