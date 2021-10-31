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
    char*   m_Text;
};

struct lyrics lyrics[] =
{
// 0, 1,   "流れてく　時の中ででも　気だるさが　ほらグルグル廻って",
// 0, 3,   "私から　離れる心も　見えないわ　そう知らない？",
0, 1,   "Ｂａｄ　Ａｐｐｌｅ！！",
0, 3,   "",
0, 3,   "東方プロジェクト",
0, 4,   "",
0, 29,  "流れてく　時の中ででも　気だるさが　ほらグルグル廻って",
0, 36,  "私から　離れる心も　見えないわ　そう知らない？",
0, 43,  "自分から　動くこともなく　時の隙間に　流され続けて",
0, 50,  "知らないわ　周りのことなく　私は私　それだけ",
0, 57,  "夢見てる？　何も見てない？　語るも無駄な　自分の言葉？",
1, 03,  "悲しむなんて　疲れるだけよ　何も感じず 過ごせばいいの",
1, 10,  "戸惑う言葉　与えられても　自分の心　ただ上の空",
1, 17,  "もし私から　動くのならば　すべて変えるのなら　黒にする",
1, 24,  "こんな自分に　未来はあるの？こんな世界に　私はいるの？",
1, 31,  "今切ないの？今悲しいの？自分の事も　わからないのまま",
1, 38,  "歩むことさえ　疲れるだけよ　人のことなど　知りもしないわ",
1, 45,  "こんな私も　変われるのなら　もし変われるのなら　白になる？",
1, 52,  "　",
2, 06,  "流れてく　時の中ででも　気だるさがほら　グルグル廻って",
2, 13,  "私から　離れる心も　見えないわそう　知らない？",
2, 20,  "自分から　動くこともなく　時の隙間に　流され続けて",
2, 27,  "知らないわ　周りのことなく　私は私　それだけ？",
2, 34,  "　",
2, 34,  "夢見てる？　何も見てない？　語るも無駄な　自分の言葉？",
2, 40,  "悲しむなんて　疲れるだけよ　何も感じず　過ごせばいいの",
2, 47,  "戸惑う言葉　与えられても 自分の心　ただ上の空",
2, 54,  "もし私から　動くのならば　すべて変えるのなら　黒にする",
3, 02,  "動くのならば　動くのならば　すべて壊すわ　すべて壊すわ",
3,  8,  "悲しむならば　悲しむならば　私の心　白く変われる？",
3, 15,  "貴方の事も　私のことも　全ての事も　まだ知らないの",
3, 22,  "重い目蓋を　開けたのならば　すべて壊すのなら　黒になれ!!!",
3, 30,  "　",
59,59,  "　",
};

static int stat_goto_xy;    // profile only
static int stat_error;      // debug only

static HANDLE console_handle;

gotoxy(x,y)
{
    COORD cd = {x, y};
    SetConsoleCursorPosition(console_handle, cd);
}

typedef struct buffer buffer;
struct buffer
{
    char m_buff[SCREEN_HEIGHT][SCREEN_WIDTH * 3];   // must be y x, *3 is for utf-8
};


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
        bool position_is_next = false;
        
        if( y == INFO_LINE )
        {
            volatile int foo = 1;
        }

        for(int back_index=0, front_index=0, x=0; x<SCREEN_WIDTH; back_index++, front_index++, x++)
        {
            // utf8 check
            bool need_update = false;
            int add_back_index = 0;
            if( backbuff->m_buff[y][back_index] < 0 )
            {
                if( backbuff->m_buff[y][back_index+1] != frontbuff->m_buff[y][front_index+1] )
                {
                    need_update = true;
                }
                else
                if( backbuff->m_buff[y][back_index+2] != frontbuff->m_buff[y][front_index+2] )
                {
                    need_update = true;
                }

                if( backbuff->m_buff[y][back_index+1] < 0 )
                {
                    add_back_index = 2;
                }
                else
                {
                    add_back_index = 1;
                }
            }

            if( need_update || backbuff->m_buff[y][back_index] != frontbuff->m_buff[y][front_index] )
            {
                if( !position_is_next )
                {
                    gotoxy(x,y);
                    stat_goto_xy++;
                }

                if( backbuff->m_buff[y][back_index] < 0)
                {
                    // hack
                    // printf("%c%c%c") is too slow for utf8 char
                    if( backbuff->m_buff[y][back_index+1] <= 0 && true )
                    {
                        int org_x = back_index;
                        int print_str_length = 0;
                        static char str_buff[4096];

                        for(; backbuff->m_buff[y][back_index] != frontbuff->m_buff[y][front_index] ||
                              backbuff->m_buff[y][back_index+1] != frontbuff->m_buff[y][front_index+1] ||
                              backbuff->m_buff[y][back_index+2] != frontbuff->m_buff[y][front_index+2]
                            ;)
                        {
                            print_str_length ++;
                            back_index += 3;
                            front_index += 3;
                            x++;

                            if( x >= SCREEN_WIDTH )
                            {
                                break;
                            }

                            if( backbuff->m_buff[y][back_index] >= 0 )
                            {
                                break;
                            }
                        }

                        print_str_length *= 3;
                        memcpy(str_buff, &backbuff->m_buff[y][org_x], print_str_length);
                        str_buff[print_str_length] = 0;
                        printf("%s", str_buff);
                    }
                    else
                    {
                        printf("%c%c%c", backbuff->m_buff[y][back_index], backbuff->m_buff[y][back_index+1], backbuff->m_buff[y][back_index+2]);
                    }
                }
                else
                {
                    if( frontbuff->m_buff[y][front_index+1] < 0 )
                    {
                        if( backbuff->m_buff[y][back_index] )
                        {
                            printf("%c%c", backbuff->m_buff[y][back_index], backbuff->m_buff[y][back_index+1]);
                            back_index++;
                        }
                        else
                        {
                            // hack, fix lyrics
                            printf("　　");
                        }
                        x++;
                    }
                    else
                    {
                        if( backbuff->m_buff[y][back_index] )
                        {
                            printf("%c", backbuff->m_buff[y][back_index]);
                        }
                        else
                        {
                            printf(" ");
                        }
                    }
                }
                position_is_next = true;
            }
            else
            {
                position_is_next = false;
            }

            if( frontbuff->m_buff[y][front_index] < 0 )
            {
                if( frontbuff->m_buff[y][front_index+1] < 0 )
                {
                    front_index += 2;
                }
                else
                {
                    front_index += 1;
                }
            }
            back_index += add_back_index;
        }
    }
}

main(){
    // SetConsoleOutputCP(65001);
    console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
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

    // for fps
    LARGE_INTEGER sklick, eklick, cpu_khz;  
    float delta_time;
    float elapse_time = 0;
    float wait_time = WAIT_TIME;
    QueryPerformanceFrequency(&cpu_khz);
    float fps_time = 0;
    float fps_count = 0;

    MoveWindow(GetConsoleWindow(), 837, 80, 980, 763, false);

    bool has_do_fps = false;

    buffer buffx[2];
    buffer* backbuffer = &buffx[0];
    buffer* frontbuffer = &buffx[1];
    buffclear(backbuffer);
    buffclear(frontbuffer);

    QueryPerformanceCounter(&sklick);

    int line = 0;
    int skip_frame = 0;
    int stat_skip_frame = 0;
    int stat_tick = 0;
    int stat_sleep = 0;
    int stat_frame = 0;

    getch();
    PlaySoundA("../bad-apple.wav", NULL, SND_FILENAME|SND_ASYNC);

    while( !feof(fp) )
    {
        fgets(buff, MAX_BUFF, fp);
        if(strlen(buff) == 1)
        {
            line = 0;

            // do fps
            if( !has_do_fps )
            {
                stat_frame++;
                // swap buff
                if( !skip_frame )
                {
                    // DWORD tick1 = GetTickCount();
                    buffprinttoscreen(frontbuffer, backbuffer);
                    buffer* temp = backbuffer;
                    backbuffer = frontbuffer;
                    frontbuffer = temp;
                    buffclear(backbuffer);
                    // WORD tick2 = GetTickCount();
                    // stat_tick =  tick2 - tick1;
                }
                else
                {
                    buffclear(backbuffer);
                    skip_frame--;
                    stat_skip_frame++;
                    assert(skip_frame >= 0);
                    buffclear(backbuffer);
                }


                for(;;)
                {
                    QueryPerformanceCounter(&eklick);
                    delta_time = (eklick.QuadPart-sklick.QuadPart) / (float)cpu_khz.QuadPart;
                    
                    if( delta_time >= wait_time )
                    {
                        wait_time += WAIT_TIME;
                        wait_time -= delta_time;

                        if( wait_time > WAIT_TIME )
                        {
                            stat_error |= 1;
                        }

                        if( wait_time <= 0 )
                        {
                            skip_frame++;
                        }

                        break;
                    }
                    else
                    {
                        if( delta_time < 0.01 )
                        {
                            Sleep(1);
                            stat_sleep++;
                        }
                    }
                }

                fps_time += delta_time;
                fps_count++;
                if( fps_time > 1 )
                {
                    fps = fps_count / fps_time;
                    fps_time = 0;
                    fps_count = 0;
                }
                elapse_time += delta_time;
                sklick = eklick;

                // update lyrics
                {
                    struct lyrics * lyrics_next = &lyrics[lyrics_index+1];
                    if( lyrics_next->m_Min * 60 + lyrics_next->m_Sec <= elapse_time)
                    {
                        lyrics_index++;
                    }
                    buffprintf(backbuffer, 8, LYRICS_LINE, "%s", lyrics[lyrics_index].m_Text);
                }

                buffprintf(backbuffer, 0, INFO_LINE, "fps: %7.2f  time %02d:%02d:%03d  frame: %d(%d)", fps, (int)(elapse_time/60), (int)(elapse_time)%60, (int)(elapse_time*1000)%1000, stat_frame, stat_frame-stat_skip_frame);
                // buffprintf(backbuffer, 0, INFO_LINE+1, "gotoxy: %-5d  tick: %-5d  error: %#x  sleep: %d", stat_goto_xy, stat_tick, stat_error, stat_sleep);

                has_do_fps = true;
            }

        }
        else
        {
            has_do_fps = false;
            frame_index++;
            buffprintf(backbuffer, 0, line, "%07d ", frame_index);
            buffprintf(backbuffer, 8, line, buff);
            line++;
        }
    }

    gotoxy(8,0);

    return 0;
}

