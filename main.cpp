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
    ܇, �R, ��, ʿ, ��, ��, ��,
    BEGIN, END
};
enum pieces Chess[] = { ܇, �R, ��, ʿ, ��, ��, �� };
const char* chessName[] = { "܇","�R","��","ʿ","��","��","��" };
struct chess {
public:
    enum pieces id;//����
    DWORD type;//�� ��
    short x;
    short y;
    bool if_river;
};
//��ͼ
struct chess map[row][col];
struct state {
    int begr;
    int begc;
    int endr;
    int endc;
    int state;
    bool isPieceSelected;  // ���ѡ��״̬��־
}state = { -1,-1,-1,-1,BEGIN, false };

//��ʽ������
void init() {
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            map[i][j].id = NONE;
            map[i][j].if_river = false;
            map[i][j].x = j * width + outsidex;
            map[i][j].y = i * width + outsidey;
            //�ڷ�
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
                //����
                if (i == 2 && (j == 1 || j == 7)) {
                    map[i][j].id = Chess[5];
                }
                //�ڱ�
                if (i == 3 && j % 2 == 0) {
                    map[i][j].id = Chess[6];
                }
            }
            //�췽
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
                //����
                if (i == 7 && (j == 1 || j == 7)) {
                    map[i][j].id = Chess[5];
                }
                //���
                if (i == 6 && j % 2 == 0) {
                    map[i][j].id = Chess[6];
                }
            }
        }
    }
}

//��ͼ
std::wstring CharToWChar(const char* str) {
    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], len);
    return wstr;
}

// ���� 4 ��ֱ�Ƿ���
void drawSelectionBox(int x, int y) {
    int boxSize = 25;  // �����С
    setlinecolor(YELLOW);  // ������ɫ
    setlinestyle(PS_SOLID, 2);  // ������ʽ�Ϳ��
    // ���Ͻ�
    line(x - boxSize, y - boxSize, x - (boxSize/2), y - boxSize);
    line(x - boxSize, y - boxSize, x - boxSize, y - (boxSize / 2));
    // ���Ͻ�
    line(x + boxSize, y - boxSize, x + (boxSize/2 ), y - boxSize);
    line(x + boxSize, y - boxSize, x + boxSize, y - (boxSize / 2));
    // ���½�
    line(x - boxSize, y + boxSize, x - (boxSize/2), y + boxSize);
    line(x - boxSize, y + boxSize, x - boxSize, y + (boxSize/2 ));
    // ���½�
    line(x + boxSize, y + boxSize, x + (boxSize/2), y + boxSize);
    line(x + boxSize, y + boxSize, x + boxSize, y + (boxSize/2));
}

void draw() {
    setfillcolor(RGB(252, 215, 162));
    setlinestyle(PS_SOLID, 2);
    for (size_t i = 0; i < row; i++) {
        for (size_t j = 0; j < col; j++) {
            //����������Ϊ���Ļ��뾶20��԰
            if (map[i][j].id != NONE) {
                settextcolor(map[i][j].type);
                setlinecolor(map[i][j].type);
                fillcircle(map[i][j].x, map[i][j].y, 20);
                fillcircle(map[i][j].x, map[i][j].y, 17);

                std::wstring wstr = CharToWChar(chessName[map[i][j].id]);
                outtextxy(map[i][j].x - 7, map[i][j].y - 8, wstr.c_str());
                // ��������ӱ�ѡ�У����� 4 ��ֱ�Ƿ���
                if (state.isPieceSelected && state.begr == i && state.begc == j) {
                    drawSelectionBox(map[i][j].x, map[i][j].y);
                }
            }
        }
    }
}

//�ƶ�����
int if_Block(struct state* state) {
    int cnt = 0;
    //��ͬ��
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
    //��ͬ��
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
    //std::cout <<"�����ӣ�" << cnt << std::endl;
    return cnt;
}

//�� ���ƶ�
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
    // ʿ���ƶ�����ֻ���ھŹ�������б���ƶ�һ��
    if (startType == RED) {
        // �췽ʿ�ľŹ���Χ���� 7��8��9 �У��� 3��4��5 ��
        if ((state->endr >= 7 && state->endr <= 9) && (state->endc >= 3 && state->endc <= 5)) {
            // �ж��Ƿ���б���ƶ�һ��
            if (std::abs(state->endr - state->begr) == 1 && std::abs(state->endc - state->begc) == 1) {
                if (!if_Block(state)) {
                    if_canmove = true;
                }
                else {
                    std::cout << "�����м����赲��" << std::endl;
                }
            }
        }
    }
    else if (startType == BLACK) {
        // �ڷ�ʿ�ľŹ���Χ���� 0��1��2 �У��� 3��4��5 ��
        if ((state->endr >= 0 && state->endr <= 2) && (state->endc >= 3 && state->endc <= 5)) {
            // �ж��Ƿ���б���ƶ�һ��
            if (std::abs(state->endr - state->begr) == 1 && std::abs(state->endc - state->begc) == 1) {
                if (!if_Block(state)) {
                    if_canmove = true;
                }
                else {
                    std::cout << "�����м����赲��" << std::endl;
                }
            }
        }
    }
    return if_canmove;
}

// �����ƶ�����
bool isJiangMoveLegal(struct state* state) {
    int startType = map[state->begr][state->begc].type;
    if (startType == RED) {
        // �췽���ľŹ���Χ���� 7��8��9 �У��� 3��4��5 ��
        if ((state->endr >= 7 && state->endr <= 9) && (state->endc >= 3 && state->endc <= 5)) {
            // ֻ��ֱ���ƶ�һ��
            if ((std::abs(state->endr - state->begr) == 1 && state->begc == state->endc) ||
                (std::abs(state->endc - state->begc) == 1 && state->begr == state->endr)) {
                if (!if_Block(state)) {
                    return true;
                }
            }
        }
    }
    else if (startType == BLACK) {
        // �ڷ����ľŹ���Χ���� 0��1��2 �У��� 3��4��5 ��
        if ((state->endr >= 0 && state->endr <= 2) && (state->endc >= 3 && state->endc <= 5)) {
            // ֻ��ֱ���ƶ�һ��
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

// �ڵ��ƶ�����
bool isPaoMoveLegal(struct state* state) {
    int blockCount = if_Block(state);
    if (state->begr == state->endr || state->begc == state->endc) {
        if (map[state->endr][state->endc].id == NONE) {
            // �ƶ�����λ���м䲻��������
            if (blockCount == 0) {
                return true;
            }
        }
        else {
            // ���ӣ��м��������ֻ��һ������
            if (blockCount == 1) {
                return true;
            }
        }
    }
    return false;
}

// ����ƶ�����
bool isZuMoveLegal(struct state* state) {
    int startType = map[state->begr][state->begc].type;
    if (startType == RED) {
        if (state->begr > 4) {
            // δ���ӣ�ֻ����ǰ�ƶ�һ��
            if (state->endr == state->begr - 1 && state->begc == state->endc) {
                return true;
            }
        }
        else {
            // ���Ӻ󣬿�����ǰ�������ƶ�һ��
            if ((state->endr == state->begr - 1 && state->begc == state->endc) ||
                (state->endr == state->begr && std::abs(state->endc - state->begc) == 1)) {
                return true;
            }
        }
    }
    else if (startType == BLACK) {
        if (state->begr < 5) {
            // δ���ӣ�ֻ����ǰ�ƶ�һ��
            if (state->endr == state->begr + 1 && state->begc == state->endc) {
                return true;
            }
        }
        else {
            // ���Ӻ󣬿�����ǰ�������ƶ�һ��
            if ((state->endr == state->begr + 1 && state->begc == state->endc) ||
                (state->endr == state->begr && std::abs(state->endc - state->begc) == 1)) {
                return true;
            }
        }
    }
    return false;
}

// ����Ƿ���һ����ʤ
bool checkWin() {
    bool redJiangExists = false;
    bool blackJiangExists = false;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (map[i][j].id == ��) {
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
        MessageBox(NULL, _T("�ڷ���ʤ��"), _T("��Ϸ����"), MB_OK);
        return true;
    }
    if (!blackJiangExists) {
        PlaySound(TEXT("./res/winmusic.wav"), NULL, SND_FILENAME | SND_ASYNC);
        MessageBox(NULL, _T("�췽��ʤ��"), _T("��Ϸ����"), MB_OK);
        return true;
    }

    return false;
}

void chessMove() {
    bool if_canmove = false;
    //����ͬһλ�� ����������� λ�ò�Խ�� ͬɫ֮�䲻�ܳ�
    //&& map[state.begr][state.begc].type != map[state.endr][state.endc].type
    if (!(state.begr == state.endr && state.begc == state.endc) && state.endr != -1 && state.endc != -1 && map[state.begr][state.begc].id != NONE
        && (map[state.endr][state.endc].id == NONE || map[state.begr][state.begc].type != map[state.endr][state.endc].type)) {
        switch (map[state.begr][state.begc].id)
        {
        case ܇:
            //ֱ���ƶ�
            if (state.begr == state.endr || state.begc == state.endc) {
                if (!if_Block(&state))if_canmove = true;
                else std::cout << "�����ƶ��Ƿ���" << std::endl;
            }
            break;
        case �R:
            //�����ƶ�
            if (if_Move(1, &state))if_canmove = true;
            else std::cout << "����ƶ��Ƿ���" << std::endl;
            break;
        case ��:
            //�����ƶ�
            if (if_Move(2, &state))if_canmove = true;
            else std::cout << "����ƶ��Ƿ���" << std::endl;
            break;
        case ʿ:
            //ֱ�ߵ����ƶ�
            if (isShiMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "ʿ���ƶ��Ƿ���" << std::endl;
            break;
        case ��:
            if (isJiangMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "�����ƶ��Ƿ���" << std::endl;
            break;
        case ��:
            if (isPaoMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "�ڵ��ƶ��Ƿ���" << std::endl;
            break;
        case ��:
            if (isZuMoveLegal(&state)) {
                if_canmove = true;
            }
            else std::cout << "����ƶ��Ƿ���" << std::endl;
            break;
        default:
            break;
        }
        
        if (if_canmove) {
            map[state.endr][state.endc].id = map[state.begr][state.begc].id;
            map[state.begr][state.begc].id = NONE;
            map[state.endr][state.endc].type = map[state.begr][state.begc].type;
            map[state.endr][state.endc].if_river = map[state.begr][state.begc].if_river;
            state.isPieceSelected = false;  // �ƶ���ɺ�ȡ��ѡ��״̬
            if (checkWin()) {
                // ��Ϸ�������˳�ѭ��
                PlaySound(TEXT("./res/winmusic.wav"), NULL, SND_FILENAME | SND_ASYNC);
                exit(0);
            }
        }
    }
}

//������
void mouseEvent() {
    //��Ϣ�ṹ�����
    ExMessage msg;
    //�������
    if (peekmessage(&msg, WM_LBUTTONDOWN)) {
        if (msg.message == WM_LBUTTONDOWN) {
            //ƫ�ư������
            int col1 = (msg.x - outsidex + width / 2) / width;
            int row1 = (msg.y - outsidey + width / 2) / width;
            //std::cout << "(" << col1 << "," << row1 << ")" << std::endl;
            if (state.state == BEGIN) {
                state.begr = row1;
                state.begc = col1;
                state.state = END;
                state.isPieceSelected = true;  // ������ӱ�ѡ��
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
//��������

int main() {
    //��ʼ������
    initgraph(500, 604, EX_SHOWCONSOLE);
    //����ģʽ ͸��
    setbkmode(TRANSPARENT);
    //����ͼƬ
    IMAGE img_board;
    loadimage(&img_board, _T("./res/chessboard.jpg"));
    //��������0 0

    PlaySound(TEXT("./res/backgroundmusic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
 
    init();

    // ˫�����ͼ����ֹ����
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