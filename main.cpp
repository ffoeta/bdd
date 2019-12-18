#pragma comment(lib, "bdd.lib")
#include "bdd.h"

#include <vector>
#include <iomanip>
#include <string>
#include <fstream>

//Ð’ = 14

//n1 - 4
//n2 - 6
//n3 - 4
//n4 - 2
//n5 

/*
      |---------------------|
      |      |       |      |
      |---------------------|
      |   *  |   0   |      |
      |---------------------|
      |   *  |       |      |
      |---------------------|
*/

//n6 ?
//n7 < K

using namespace std;

const bool IS_DEBUG = true;

// количество домов
const int N = 9;

// 4 свойства у каждого дома:
// - цвет крыши
// - марка сигарет
// - машина
// - образование

const int M = 4;

// размерность
const int SIDE_LEN = 3;

// битов на кодирование
const int LOG_N = 4;

// всего входных сигналов
const int TOTAL_BIT = M * N * LOG_N;

// коснтанта для 7 условия
const int K = 24;

// массив p[k][i][j]
// "катое свойства итого объекта значени j"
bdd p[M][N][N];

// вывод
void fun(char *varset, int size);

int has_top_neighbor(int i)
{
    return i - SIDE_LEN >= 0;
}

int has_bot_neighbor(int i)
{
    return i + SIDE_LEN <= N;
}

int has_left_neighbor(int i)
{
    return i % SIDE_LEN != 0;
}

int has_right_neighbor(int i)
{
    return (i + 1) % SIDE_LEN != 0;
}


//получить соседа сверху
int get_top_neighbor(int i)
{
    return i - SIDE_LEN;
}
//получить соседа снизу
int get_bot_neighbor(int i)
{
    return i + SIDE_LEN;
}
//получить соседа слева
int get_left_neighbor(int i)
{
    return i - 1;
}
//получить соседа справа
int get_right_neighbor(int i)
{
    return i + 1;
}

//левый сосед
bdd left_neighbors(int neighbor_k, int neighbor_v, int main_k, int main_v)
{
    bdd task = bddtrue;

    for (int i = 0; i < N; ++i)
    {
        if (has_left_neighbor(i))
        {
            task &= !(p[main_k][i][main_v] ^ p[neighbor_k][get_left_neighbor(i)][neighbor_v]);
        } else
        {
            task &= !p[main_k][i][main_v];
        }
    }
    return task;
}
//нижний сосед
bdd down_neighbors(int neighbor_k, int neighbor_v, int main_k, int main_v)
{
    bdd task = bddtrue;
    for (int i = 0; i < N; ++i)
    {
        if (has_bot_neighbor(i) && has_left_neighbor(i))
        {
            task &= !(p[main_k][i][main_v] ^ p[neighbor_k][get_left_neighbor(get_bot_neighbor(i))][neighbor_v]);
        }
        if (!has_bot_neighbor(i) || has_left_neighbor(i))
        {
            task &= !(p[main_k][i][main_v]);
        }
        if (!(has_top_neighbor(i)) || !(has_right_neighbor(i)))
        {
            task &= !p[neighbor_k][i][neighbor_v];     
        }
    }  
    return task;
}

bdd neighbors(int k1, int v1, int kCntr, int vCntr)
{
    bdd task = bddfalse;
    for (int i = 0; i < N; ++i)
    {
        if (has_bot_neighbor(i) && has_left_neighbor(i))
            task |= !(p[kCntr][i][vCntr] ^ p[k1][get_left_neighbor(get_bot_neighbor(i))][v1]);

        if (has_top_neighbor(i) && has_left_neighbor(i))
            task |= !(p[kCntr][i][vCntr] ^ p[k1][get_left_neighbor(get_top_neighbor(i))][v1]);

        if (!(has_right_neighbor(i) && has_top_neighbor(i)) && !(has_right_neighbor(i) && has_bot_neighbor(i)))
            task &= !p[k1][i][v1];
        if (!(has_top_neighbor(i) && has_left_neighbor(i)) && !(has_bot_neighbor(i) && has_left_neighbor(i)))
            task &= !p[kCntr][i][vCntr];
    }
    return task;
}

int main()
{
    bdd_init(10000000, 1000000);
    bdd_setvarnum(TOTAL_BIT);
    bdd_setcacheratio(100);

    // Ð¸Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ p[k][i][j]
    for (int k = 0; k < M; ++k)
    {
        int I = 0;
        for (auto i = 0; i < N; ++i)
        {
            // Ñ†Ð¸ÐºÐ» ÑÐ²Ð¾Ð¹ÑNÐ²
            for (auto j = 0; j < N; ++j)
            {
                p[k][i][j] = bddtrue;
                for (auto l = 0; l < LOG_N; ++l)
                    if (j >> l & 1)
                        p[k][i][j] &= bdd_ithvar(I + LOG_N * k + l);
                    else
                        p[k][i][j] &= bdd_nithvar(I + LOG_N * k + l);
            }
            I += LOG_N * M;
        }
    }

    bdd task = bddtrue;
    {
        for (int k = 0; k < M; ++k)
        {
            for (int i = 0; i < N; ++i)
            {
                bdd temp = bddfalse;
                for (int j = 0; j < N; ++j)
                    temp |= p[k][i][j];
                task &= temp;
            }
        }
    }
    

    // Ð¾Ð³Ñ€Ð°Ð½Ð¸Ñ‡ÐµÐ½Ð¸Ðµ n1
    {
        task &= p[3][0][0];
        task &= p[3][8][8];
        task &= p[2][0][7];
        task &= p[3][1][1];

        // // Ð´Ð¾Ð±Ð°Ð²Ð¸Ð¼ ÐµÑ‰Ñ‘
        task &= p[3][2][2];
        task &= p[2][2][3];

        task &= p[0][0][8];
        task &= p[1][1][4];
        task &= p[1][3][2];
        task &= p[3][4][3];

        task &= p[1][5][6];
        task &= p[0][2][4];
        task &= p[2][6][1];
        task &= p[2][4][6];
        task &= p[3][7][4];
        // task &= p[0][4][1];
        // task &= p[0][6][7];
        // task &= p[0][8][5];
        // task &= p[0][3][2];
        // task &= p[1][4][3];
        // task &= p[1][0][8];
        // task &= p[2][1][4];
        // task &= p[2][7][2];
        // task &= p[1][0][8];
        // task &= p[1][6][5];
        // task &= p[3][3][6];
        // task &= p[2][3][0];
        // task &= p[1][2][1];
        // task &= p[0][1][6];
    }
    cout << bdd_satcount(task) << " solution(s) after n1\n"<<endl;


    // Ð¾Ð³Ñ€Ð°Ð½Ð¸Ñ‡ÐµÐ½Ð¸Ðµ n2
    {
        for (int i = 0; i < N; ++i)
            task &= !(p[3][i][8] ^ p[1][i][0]);
        for (int i = 0; i < N; ++i)
            task &= !(p[3][i][7] ^ p[0][i][3]);
        for (int i = 0; i < N; ++i)
            task &= !(p[0][i][8] ^ p[2][i][7]);

        for (int i = 0; i < N; ++i)
            task &= !(p[1][i][6] ^ p[2][i][5]);
        for (int i = 0; i < N; ++i)
            task &= !(p[2][i][7] ^ p[0][i][8]);
        for (int i = 0; i < N; ++i)
            task &= !(p[1][i][7] ^ p[3][i][5]);

    }
    cout << bdd_satcount(task) << " solution(s) after n2\n"<<endl;

    // Ð¾Ð³Ñ€Ð°Ð½Ð¸Ñ‡ÐµÐ½Ð¸Ðµ n3
    {
        task &= down_neighbors(1, 3, 2, 3);
        task &= left_neighbors(1, 2, 3, 4);
        task &= left_neighbors(2, 2, 3, 7);
        task &= down_neighbors(2, 1, 1, 3);
    }
    cout << bdd_satcount(task) << " solution(s) after n3\n"<<endl;

    // Ð¾Ð³Ñ€Ð°Ð½Ð¸Ñ‡ÐµÐ½Ð¸Ðµ n4
    {
        int p1, v1, p2, v2;

        // #1
        p1 = 2, v1 = 0, p2 = 1, v2 = 7;
        task &= neighbors(p1, v1, p2, v2);

        // #2
        p1 = 1, v1 = 6, p2 = 3, v2 = 4;
        task &= neighbors(p1, v1, p2, v2);

        // #3 
        p1 = 2, v1 = 5, p2 = 1, v2 = 3;
        task &= neighbors(p1, v1, p2, v2);

        // #4
        p1 = 1, v1 = 2, p2 = 2, v2 = 6;
        task &= neighbors(p1, v1, p2, v2);

        // #5 
        p1 = 2, v1 = 1, p2 = 0, v2 = 2;
        task &= neighbors(p1, v1, p2, v2);

        if (IS_DEBUG)
            cout << bdd_satcount(task) << " solution(s) after n4\n"
                 << endl;
    }

    // Ð¾Ð³Ñ€Ð°Ð½Ð¸Ñ‡ÐµÐ½Ð¸Ðµ Ð½Ð° Ð¿Ð¾Ð²NÐ¾Ñ€ ÑÐ²Ð¾Ð¹ÑNÐ²
    {
        for (int k = 0; k < M; ++k)
            for (int i = 0; i < N - 1; ++i)
                for (int j = 0; j < N; ++j)
                    for (int l = i + 1; l < N; ++l)
                        task &= p[k][i][j] >> !p[k][l][j];

        if (IS_DEBUG)
        {
            cout << bdd_satcount(task) << " solutio n(s) after n5\n"
                 << endl;
        }
    }

    // Ð¾Ð³Ñ€Ð°Ð½Ð¸Ñ‡ÐµÐ½Ð¸Ðµ n7
    {
        bdd temp;
        for (int i = 0; i < N; i++)
        {
            temp = bddtrue;
            for (int j1 = 0; j1 < N; j1++)
            {
                for (int j2 = 0; j2 < N; j2++)
                    for (int j3 = 0; j3 < N; j3++)
                        for (int j4 = 0; j4 < N; j4++)
                            if (j1 + j2 + j3 + j4 > K)
                                temp &= !(p[0][i][j1] & p[1][i][j2] & p[2][i][j3] & p[3][i][j4]);
            }
            task &= temp;
        }

        if (IS_DEBUG)
            cout << bdd_satcount(task) << " solution(s) after n7\n"
                 << endl;
    }

    

    // Ð²Ñ‹Ð²Ð¾Ð´ Ñ€ÐµÐ·ÑƒÐ»ÑŒNÐ°NÐ¾Ð²
    cout << bdd_satcount(task) << " solution(s):\n"
         << endl;
    cout << endl;

    bdd_allsat(task, fun);

    // Ð·Ð°Ð²ÐµÑ€ÑˆÐµÐ½Ð¸Ðµ Ñ€Ð°Ð±Ð¾NÑ‹ Ð±Ð¸Ð±Ð»Ð¸Ð¾NÐµÐºÐ¸
    bdd_done();

    return 0;
}

// ÐÐ¸Ð¶Ðµ Ñ€ÐµÐ°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¹, ÑƒÐ¿Ñ€Ð°Ð²Ð»ÑÑŽÑ‰Ð¸Ñ… Ð²Ñ‹Ð²Ð¾Ð´Ð¾Ð¼ Ñ€ÐµÐ·ÑƒÐ»ÑŒNÐ°NÐ¾Ð².
// Ð ÐµÐºÐ¾Ð¼ÐµÐ½Ð´ÑƒÐµNÑÑ ÑÐ°Ð¼Ð¾ÑNÐ¾ÑNÐµÐ»ÑŒÐ½Ð¾ Ñ Ð½Ð¸Ð¼Ð¸ Ð¾Ð·Ð½Ð°ÐºÐ¾Ð¼Ð¸NÑŒÑÑ.
// Ð’ ÑÐ¾Ð±ÑNÐ²ÐµÐ½Ð½Ñ‹Ñ… Ð·Ð°Ð´Ð°Ð½Ð¸ÑÑ… ÑÐ»ÐµÐ´ÑƒÐµN Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°NÑŒ ÑNÐ¸ Ñ„ÑƒÐ½ÐºÑ†Ð¸Ð¸
// Ð¸Ð»Ð¸ Ð¿Ñ€Ð¸Ð´ÑƒÐ¼Ð°NÑŒ ÑÐ¾Ð±ÑNÐ²ÐµÐ½Ð½Ñ‹Ðµ.

char var[TOTAL_BIT];

void print()
{
    const int columSize1 = 6;
    const int columSize2 = 4;
    const string mesT1 = "Obj";
    const string mesT2 = "#";
    const string div = " |";

    cout << setfill('-');
    cout << setw(columSize1 + div.length() + (columSize2 + div.length()) * M) << '-' << endl;

    cout << setfill(' ');
    cout << setw(columSize1) << mesT1 << div;
    for (int i = 0; i < M; ++i)
    {
        int numCount = 0;
        int p = i + 1;
        while (p > 0)
        {
            p = p / 10;
            ++numCount;
        }

        cout << setw(columSize2 - numCount - 1) << mesT2 << " " << i + 1 << div;
    }
    cout << endl;

    for (int i = 0; i < N; i++)
    {
        cout << setfill('-');
        cout << setw(columSize1 + div.length() + (columSize2 + div.length()) * M) << '-' << endl;

        cout << setfill(' ');
        cout << setw(columSize1) << i << div;
        for (int j = 0; j < M; j++)
        {
            int J = i * M * LOG_N + j * LOG_N;
            int num = 0;
            for (unsigned k = 0; k < LOG_N; k++)
                num += (int)(var[J + k] << k);
            cout << setw(columSize2) << num << div;
        }

        cout << endl;
    }
    cout << setfill('-');
    cout << setw(columSize1 + div.length() + (columSize2 + div.length()) * M) << '-' << endl;
    cout << endl;
}

void build(char *varset, unsigned n, unsigned I)
{
    if (I == n - 1)
    {
        if (varset[I] >= 0)
        {
            var[I] = varset[I];
            print();
            return;
        }
        var[I] = 0;
        print();
        var[I] = 1;
        print();
        return;
    }
    if (varset[I] >= 0)
    {
        var[I] = varset[I];
        build(varset, n, I + 1);
        return;
    }
    var[I] = 0;
    build(varset, n, I + 1);
    var[I] = 1;
    build(varset, n, I + 1);
}

void fun(char *varset, int size)
{
    // freopen("output", "w", stdout);
    build(varset, size, 0);
}