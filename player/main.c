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

main(){
    // SetConsoleOutputCP(65001);
    system("mode 120,40");
    SetConsoleTitleA("Bad Apple!!");

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

    getch();

    PlaySoundA("../bad-apple.wav", NULL, SND_FILENAME|SND_ASYNC);
    bool has_do_fps = false;

    QueryPerformanceCounter(&sklick);
    while( !feof(fp) )
    {
        fgets(buff, MAX_BUFF, fp);
        if(strlen(buff) == 1)
        {
            // do fps
            if( !has_do_fps )
            {
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
                        for(int i=0; i<100; i++)
                        {
                            gotoxy(i, LYRICS_LINE);
                            printf(".\b ");
                        }
                    }
                    // print lyrics
                    gotoxy(0, LYRICS_LINE);
                    printf("        %s", lyrics[lyrics_index].m_Text);
                }

                gotoxy(0,INFO_LINE);
                printf("fps: %.2f\ttime %02d:%02d", fps, (int)(elapse_time/60), (int)(elapse_time)%60);
                gotoxy(0,0);

                has_do_fps = true;
            }

        }
        else
        {
            has_do_fps = false;
            frame_index++;
            printf("%07d ", frame_index);
            printf(buff);
        }
    }



    return 0;
}

