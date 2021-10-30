#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

#pragma execution_character_set( "utf-8" )
#pragma comment(lib, "Winmm.lib")

#define MAX_BUFF (4096)
#define LYRICS_LINE (36)
#define INFO_LINE (38)
#define WAIT_TIME (0.033333f)

char buff[MAX_BUFF];

#define SCREEN_WIDTH    (120)
#define SCREEN_HEIGHT   (40)


struct lyrics
{
    float   m_Min;
    float   m_Sec;
    char *  m_Text;
};

struct lyrics lyrics[] =
{
0, 1,  "Bad Apple!!",
0, 2,   " ",
0, 3,  "東方プロジェクト",
0, 4,   " ",
0, 29,  "流れてく 時の中ででも 気だるさが ほらグルグル廻って",
0, 36,  "私から 離れる心も 見えないわ そう知らない？",
0, 43,  "自分から 動くこともなく 時の隙間に 流され続けて",
0, 50,  "知らないわ 周りのことなく 私は私 それだけ",
0, 57,  "夢見てる？ 何も見てない？ 語るも無駄な 自分の言葉？",
1, 03,  "悲しむなんて 疲れるだけよ 何も感じず 過ごせばいいの",
1, 10,  "戸惑う言葉 与えられても 自分の心 ただ上の空",
1, 17,  "もし私から 動くのならば すべて変えるのなら 黒にする",
1, 24,  "こんな自分に 未来はあるの？こんな世界に 私はいるの？",
1, 31,  "今切ないの？今悲しいの？自分の事も わからないのまま",
1, 38,  "歩むことさえ 疲れるだけよ 人のことなど 知りもしないわ",
1, 45,  "こんな私も 変われるのなら もし変われるのなら 白になる？",
1, 52,  " ",
2, 06,  "流れてく 時の中ででも 気だるさがほら グルグル廻って",
2, 13,  "私から 離れる心も 見えないわそう 知らない？",
2, 20,  "自分から 動くこともなく 時の隙間に 流され続けて",
2, 27,  "知らないわ 周りのことなく 私は私 それだけ？",
2, 34,  " ",
2, 34,  "夢見てる？ 何も見てない？ 語るも無駄な 自分の言葉？",
2, 40,  "悲しむなんて 疲れるだけよ 何も感じず 過ごせばいいの",
2, 47,  "戸惑う言葉 与えられても 自分の心 ただ上の空",
2, 54,  "もし私から 動くのならば すべて変えるのなら 黒にする",
3, 02,  "動くのならば　動くのならば　すべて壊すわ　すべて壊すわ",
3,  8,  "悲しむならば　悲しむならば　私の心　白く変われる？",
3, 15,  "貴方の事も　私のことも　全ての事も　まだ知らないの",
3, 22,  "重い目蓋を　開けたのならば　すべて壊すのなら　黒になれ!!!",
3, 30,  " ",
59,59,  " ",
};

gotoxy(x,y)
{
    COORD cd = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cd);
}

struct buffer
{
    char m_buff[SCREEN_HEIGHT][SCREEN_WIDTH * 3];   // must be y x, *3 is for utf-8
};

typedef struct buffer buffer;


buffprintf(buffer* buff, int x, int y, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    vsprintf(&buff->m_buff[y][x], format, ap);

    va_end(ap);
}

buffclear(buffer* buff)
{
    memset(buff, 0, sizeof(buffer));
}

buffprinttoscreen(buffer* frontbuff, buffer* backbuff)
{
    for( int y=0; y<SCREEN_HEIGHT; y++ )
    {
        for(int x=0, x_hack = 0; x<SCREEN_WIDTH * 3; x++, x_hack++ )
        {
            // -30 check
            bool need_update = false;
            if( backbuff->m_buff[y][x] == -30 )
            {
                // need_update = true;
                if( backbuff->m_buff[y][x+1] != frontbuff->m_buff[y][x+1] )
                {
                    need_update = true;
                }
                else
                if( backbuff->m_buff[y][x+2] != frontbuff->m_buff[y][x+2] )
                {
                    need_update = true;
                }
            }
            
            if( need_update || backbuff->m_buff[y][x] != frontbuff->m_buff[y][x] )
            {
                assert(x_hack<120);
                gotoxy(x_hack,y);
                if( backbuff->m_buff[y][x] == -30)
                {
                    if( backbuff->m_buff[y][x+1] == -106 )
                    {
                        printf("%c%c%c", backbuff->m_buff[y][x], backbuff->m_buff[y][x+1], backbuff->m_buff[y][x+2]);
                    }
                    else
                    {
                        assert(false);
                        printf("%c%c", backbuff->m_buff[y][x], backbuff->m_buff[y][x+1]);
                    }
                   
                }
                else
                {
                    printf("%c", backbuff->m_buff[y][x]);
                }
            }

            if( backbuff->m_buff[y][x] == -30 )
            {
                x += 2;
            }
        }
    }
}


#define printf ...

main(){
    // SetConsoleOutputCP(65001);
    {
        char command[256];
        sprintf(command, "mode %d,%d", SCREEN_WIDTH, SCREEN_HEIGHT);
        system(command);
        SetConsoleTitleA("Bad Apple!!");
    }

    FILE * fp = fopen("../bad-apple.txt", "r");
    assert(fp);

    int frame_index = 0;
    int lyrics_index = 0;
    float fps = 0;

    LARGE_INTEGER sklick, eklick, cpu_khz;  
    float delta_time;
    float elapse_time = 0;
    float wait_time = WAIT_TIME;

    QueryPerformanceFrequency(&cpu_khz);

    // getch();

    PlaySoundA("../bad-apple.wav", NULL, SND_FILENAME|SND_ASYNC);
    bool has_do_fps = false;

    buffer buffx[2];
    buffer* backbuffer = &buffx[0];
    buffer* frontbuffer = &buffx[1];
    buffclear(backbuffer);
    buffclear(frontbuffer);

    QueryPerformanceCounter(&sklick);
    int line = 0;
    while( !feof(fp) )
    {
        fgets(buff, MAX_BUFF, fp);
        if(strlen(buff) == 1)
        {
            line = 0;

            // do fps
            if( !has_do_fps )
            {
                // swap buff
                {
                    WORD tick1 = GetTickCount();
                    buffprinttoscreen(frontbuffer, backbuffer);
                    buffer* temp = backbuffer;
                    backbuffer = frontbuffer;
                    frontbuffer = temp;
                    buffclear(backbuffer);
                    WORD tick2 = GetTickCount();
                    buffprintf(backbuffer, 0, 39, "%d", tick2 - tick1);
                }

                for(;;)
                {
                    QueryPerformanceCounter(&eklick);
                    delta_time = (eklick.QuadPart-sklick.QuadPart) / (float)cpu_khz.QuadPart;
                    if( delta_time > wait_time )
                    {
                        wait_time += WAIT_TIME;
                        wait_time -= delta_time;
                        break;
                    }
                    else
                    {
                        if( delta_time < 0.02 )
                        {
                            Sleep(1);
                        }
                    }
                }

                elapse_time += delta_time;
                fps = 1.f/delta_time;
                sklick = eklick;

                // update lyrics
                {
                    struct lyrics * lyrics_next = &lyrics[lyrics_index+1];
                    if( lyrics_next->m_Min * 60 + lyrics_next->m_Sec <= elapse_time)
                    {
                        lyrics_index++;
                        // clear traces
                        // for(int i=0; i<100; i++)
                        // {
                        //     gotoxy(i, LYRICS_LINE);
                        //     printf(".\b ");
                        // }
                    }
                    // print lyrics
                    buffprintf(backbuffer, 8, LYRICS_LINE, "%s", lyrics[lyrics_index].m_Text);
                }

                buffprintf(backbuffer, 0, INFO_LINE, "fps: %.2f\ttime %02d:%02d", fps, (int)(elapse_time/60), (int)(elapse_time)%60);

                has_do_fps = true;
            }

        }
        else
        {
            has_do_fps = false;
            frame_index++;
            buffprintf(backbuffer, 0, line, "%07d ", frame_index);
            buffprintf(backbuffer, 8, line, buff);
            // buffprintf(backbuffer, 8, line, "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░░░▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░▓▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░");
            line++;
        }
    }



    return 0;
}

