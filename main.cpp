#include"easyx.h"
#include<iostream>
#include<string>
#include<conio.h> 

#include <windows.h>

#pragma comment(lib, "winmm.lib")
#define row 10
#define col 9
// 52 52 31 40
#define width 52
#define outsidex 41
#define outsidey 65

enum pieces {
    NONE = -1,
    車, 馬, 象, 士, 将, 炮, 卒,
    BEGIN, END
};
enum pieces Chess[] = { 車, 馬, 象, 士, 将, 炮, 卒 };
const char* chessName[] = { "車","馬","象","士","将","炮","卒" };
struct chess {
public:
    enum pieces id;//名称
    DWORD type;//红 黑
    short x;
    short y;
    bool if_river;
};
//地图
struct chess map[row][col];
struct state {
    int begr;
    int begc;
    int endr;
    int endc;
    int state;
    bool isPieceSelected;  // 添加选中状态标志
}state = { -1,-1,-1,-1,BEGIN, false };

//初式化数据
void init() {
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            map[i][j].id = NONE;
            map[i][j].if_river = false;
            map[i][j].x = j * width + outsidex;
            map[i][j].y = i * width + outsidey;
            //黑方
            if (i <= 4) {
                map[i][j].type = BLACK;
                if (i == 0) {
                    if (j <= 4) {
                        map[i][j].id = Chess[j];
                    }
                    if (j > 4) {
                        map[i][j].id = Chess[8 - j];
                    }
                }
                //黑炮
                if (i == 2 && (j == 1 || j == 7)) {
                    map[i][j].id = Chess[5];
                }
                //黑兵
                if (i == 3 && j % 2 == 0) {
                    map[i][j].id = Chess[6];
                }
            }
            //红方
            if (i > 4) {
                map[i][j].type = RED;
                if (i == 9) {
                    if (j <= 4) {
                        map[i][j].id = Chess[j];
                    }
                    if (j > 4) {
                        map[i][j].id = Chess[8 - j];
                    }
                }
                //红炮
                if (i == 7 && (j == 1 || j == 7)) {
                    map[i][j].id = Chess[5];
                }
                //红兵
                if (i == 6 && j % 2 == 0) {
                    map[i][j].id = Chess[6];
                }
            }
        }
    }
}

//绘图
std::wstring CharToWChar(const char* str) {
    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], len);
    return wstr;
}

// 绘制 4 个直角方框
void drawSelectionBox(int x, int y) {
    int boxSize = 25;  // 方框大小
    setlinecolor(YELLOW);  // 方框颜色
    setlinestyle(PS_SOLID, 2);  // 线条样式和宽度
    // 左上角
    line(x - boxSize, y - boxSize, x - (boxSize/2), y - boxSize);
    line(x - boxSize, y - boxSize, x - boxSize, y - (boxSize / 2));
    // 右上角
    line(x + boxSize, y - boxSize, x + (boxSize/2 ), y - boxSize);
    line(x + boxSize, y - boxSize, x + boxSize, y - (boxSize / 2));
    // 左下角
    line(x - boxSize, y + boxSize, x - (boxSize/2), y + boxSize);
    line(x - boxSize, y + boxSize, x - boxSize, y + (boxSize/2 ));
    // 右下角
    line(x + boxSize, y + boxSize, x + (boxSize/2), y + boxSize);
    line(x + boxSize, y + boxSize, x + boxSize, y + (boxSize/2));
}

void draw() {
    setfillcolor(RGB(252, 215, 162));
    setlinestyle(PS_SOLID, 2);
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            //以棋子坐标为中心画半径20的园
            if (map[i][j].id != NONE) {
                settextcolor(map[i][j].type);
                setlinecolor(map[i][j].type);
                fillcircle(map[i][j].x, map[i][j].y, 20);
                fillcircle(map[i][j].x, map[i][j].y, 17);

                std::wstring wstr = CharToWChar(chessName[map[i][j].id]);
                outtextxy(map[i][j].x - 7, map[i][j].y - 8, wstr.c_str());
                // 如果该棋子被选中，绘制 4 个直角方框
                if (state.isPieceSelected && state.begr == i && state.begc == j) {
                    drawSelectionBox(map[i][j].x, map[i][j].y);
                }
            }
        }
    }
}

//移动棋子
int if_Block(struct state* state) {
    int cnt = 0;
    //相同行
    if (state->begr == state->endr) {
        if (state->begc == max(state->begc, state->endc)) {
            for (int i = state->endc + 1; i < state->begc; i++) {
                if (map[state->begr][i].id != NONE)cnt++;
            }
        }
        else {
            for (int i = state->begc + 1; i < state->endc; i++) {
                if (map[state->begr][i].id != NONE)cnt++;
            }
        }
    }
    //相同列
    if (state->begc == state->endc) {
        if (state->begr == max(state->begr, state->endr)) {
            for (int i = state->endr + 1; i < state->begr; i++) {
                if (map[i][state->begc].id != NONE)cnt++;
            }
        }
        else {
            for (int i = state->begr + 1; i < state->endr; i++) {
                if (map[i][state->begc].id != NONE)cnt++;
            }
        }
    }
    //std::cout <<"有棋子：" << cnt << std::endl;
    return cnt;
}

//马 象移动
bool if_Move(int n, struct state* state) {
    int tempc = (max(state->begc, state->endc) - min(state->begc, state->endc));
    int tempr = (max(state->begr, state->endr) - min(state->begr, state->endr));
    if (n == 1) {
        //std::cout << "tempc + tempr" << tempc + tempr << std::endl;
        if (tempc >= 1 && tempr >= 1 && tempc + tempr == 3)return true;
    }
    if (n == 2) {
        if (tempc == 2 && tempr == 2 && tempc + tempr == 4)return true;
    }
    return false;
}

bool isShiMoveLegal(struct state* state) {
    bool if_canmove = false;
    int startType = map[state->begr][state->begc].type;
    // 士的移动规则：只能在九宫格内沿斜线移动一格
    if (startType == RED) {
        // 红方士的九宫格范围：第 7、8、9 行，第 3、4、5 列
        if ((state->endr >= 7 && state->endr <= 9) && (state->endc >= 3 && state->endc <= 5)) {
            // 判断是否沿斜线移动一格
            if (std::abs(state->endr - state->begr) == 1 && std::abs(state->endc - state->begc) == 1) {
                if (!if_Block(state)) {
                    if_canmove = true;
                }
                else {
                    std::cout << "错误！中间有阻挡！" << std::endl;
                }
            }
        }
    }
    else if (startType == BLACK) {
        // 黑方士的九宫格范围：第 0、1、2 行，第 3、4、5 列
        if ((state->endr >= 0 && state->endr <= 2) && (state->endc >= 3 && state->endc <= 5)) {
            // 判断是否沿斜线移动一格
            if (std::abs(state->endr - state->begr) == 1 && std::abs(state->endc - state->begc) == 1) {
                if (!if_Block(state)) {
                    if_canmove = true;
                }
                else {
                    std::cout << "错误！中间有阻挡！" << std::endl;
                }
            }
        }
    }
    return if_canmove;
}

// 将的移动规则
bool isJiangMoveLegal(struct state* state) {
    int startType = map[state->begr][state->begc].type;
    if (startType == RED) {
        // 红方将的九宫格范围：第 7、8、9 行，第 3、4、5 列
        if ((state->endr >= 7 && state->endr <= 9) && (state->endc >= 3 && state->endc <= 5)) {
            // 只能直线移动一格
            if ((std::abs(state->endr - state->begr) == 1 && state->begc == state->endc) ||
                (std::abs(state->endc - state->begc) == 1 && state->begr == state->endr)) {
                if (!if_Block(state)) {
                    return true;
                }
            }
        }
    }
    else if (startType == BLACK) {
        // 黑方将的九宫格范围：第 0、1、2 行，第 3、4、5 列
        if ((state->endr >= 0 && state->endr <= 2) && (state->endc >= 3 && state->endc <= 5)) {
            // 只能直线移动一格
            if ((std::abs(state->endr - state->begr) == 1 && state->begc == state->endc) ||
                (std::abs(state->endc - state->begc) == 1 && state->begr == state->endr)) {
                if (!if_Block(state)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// 炮的移动规则
bool isPaoMoveLegal(struct state* state) {
    int blockCount = if_Block(state);
    if (state->begr == state->endr || state->begc == state->endc) {
        if (map[state->endr][state->endc].id == NONE) {
            // 移动到空位，中间不能有棋子
            if (blockCount == 0) {
                return true;
            }
        }
        else {
            // 吃子，中间必须有且只有一个棋子
            if (blockCount == 1) {
                return true;
            }
        }
    }
    return false;
}

// 卒的移动规则
bool isZuMoveLegal(struct state* state) {
    int startType = map[state->begr][state->begc].type;
    if (startType == RED) {
        if (state->begr > 4) {
            // 未过河，只能向前移动一格
            if (state->endr == state->begr - 1 && state->begc == state->endc) {
                return true;
            }
        }
        else {
            // 过河后，可以向前、左、右移动一格
            if ((state->endr == state->begr - 1 && state->begc == state->endc) ||
                (state->endr == state->begr && std::abs(state->endc - state->begc) == 1)) {
                return true;
            }
        }
    }
    else if (startType == BLACK) {
        if (state->begr < 5) {
            // 未过河，只能向前移动一格
            if (state->endr == state->begr + 1 && state->begc == state->endc) {
                return true;
            }
        }
        else {
            // 过河后，可以向前、左、右移动一格
            if ((state->endr == state->begr + 1 && state->begc == state->endc) ||
                (state->endr == state->begr && std::abs(state->endc - state->begc) == 1)) {
                return true;
            }
        }
    }
    return false;
}

// 检查是否有一方获胜
bool checkWin() {
    bool redJiangExists = false;
    bool blackJiangExists = false;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (map[i][j].id == 将) {
                if (map[i][j].type == RED) {
                    redJiangExists = true;
                }
                else {
                    blackJiangExists = true;
                }
            }
        }
    }
    if (!redJiangExists) {
        PlaySound(TEXT("./res/winmusic.wav"), NULL, SND_FILENAME | SND_ASYNC);
        MessageBox(NULL, _T("黑方获胜！"), _T("游戏结束"), MB_OK);
        return true;
    }
    if (!blackJiangExists) {
        PlaySound(TEXT("./res/winmusic.wav"), NULL, SND_FILENAME | SND_ASYNC);
        MessageBox(NULL, _T("红方获胜！"), _T("游戏结束"), MB_OK);
        return true;
    }

    return false;
}

void chessMove() {
    bool if_canmove = false;
    //不是同一位置 点击的是其中 位置不越界 同色之间不能吃
    //&& map[state.begr][state.begc].type != map[state.endr][state.endc].type
    if (!(state.begr == state.endr && state.begc == state.endc) && state.endr != -1 && state.endc != -1 && map[state.begr][state.begc].id != NONE
        && (map[state.endr][state.endc].id == NONE || map[state.begr][state.begc].type != map[state.endr][state.endc].type)) {
        switch (map[state.begr][state.begc].id)
        {
        case 車:
            //直线移动
            if (state.begr == state.endr || state.begc == state.endc) {
                if (!if_Block(&state))if_canmove = true;
                else std::cout << "车的移动非法！" << std::endl;
            }
            break;
        case 馬:
            //日字移动
            if (if_Move(1, &state))if_canmove = true;
            else std::cout << "马的移动非法！" << std::endl;
            break;
        case 象:
            //田字移动
            if (if_Move(2, &state))if_canmove = true;
            else std::cout << "象的移动非法！" << std::endl;
            break;
        case 士:
            //直线单格移动
            if (isShiMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "士的移动非法！" << std::endl;
            break;
        case 将:
            if (isJiangMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "将的移动非法！" << std::endl;
            break;
        case 炮:
            if (isPaoMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "炮的移动非法！" << std::endl;
            break;
        case 卒:
            if (isZuMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "卒的移动非法！" << std::endl;
            break;
        default:
            break;
        }
        
        if (if_canmove) {
            map[state.endr][state.endc].id = map[state.begr][state.begc].id;
            map[state.begr][state.begc].id = NONE;
            map[state.endr][state.endc].type = map[state.begr][state.begc].type;
            map[state.endr][state.endc].if_river = map[state.begr][state.begc].if_river;
            state.isPieceSelected = false;  // 移动完成后取消选中状态
            if (checkWin()) {
                // 游戏结束，退出循环
                PlaySound(TEXT("./res/winmusic.wav"), NULL, SND_FILENAME | SND_ASYNC);
                exit(0);
            }
        }
    }
}

//鼠标操作
void mouseEvent() {
    //消息结构体变量
    ExMessage msg;
    //左键按下
    if (peekmessage(&msg, WM_LBUTTONDOWN)) {
        if (msg.message == WM_LBUTTONDOWN) {
            //偏移半个格子
            int col1 = (msg.x - outsidex + width / 2) / width;
            int row1 = (msg.y - outsidey + width / 2) / width;
            //std::cout << "(" << col1 << "," << row1 << ")" << std::endl;
            if (state.state == BEGIN) {
                state.begr = row1;
                state.begc = col1;
                state.state = END;
                state.isPieceSelected = true;  // 标记棋子被选中
            }
            else if (state.state == END)
            {
                state.endr = row1;
                state.endc = col1;
                state.state = BEGIN;
            }
            chessMove();
        }
    }
}
//背景音乐

int main() {
    //初始化窗口
    initgraph(500, 604, EX_SHOWCONSOLE);
    //背景模式 透明
    setbkmode(TRANSPARENT);
    //背景图片
    IMAGE img_board;
    loadimage(&img_board, _T("./res/chessboard.jpg"));
    //贴在坐标0 0

    PlaySound(TEXT("./res/backgroundmusic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
 
    init();

    // 双缓冲绘图，防止闪屏
    BeginBatchDraw();
    while (true) {
        cleardevice();
        putimage(0, 0, &img_board);
        draw();
        mouseEvent();
        FlushBatchDraw();
    }
    EndBatchDraw();
    getchar();
    return 0;
}