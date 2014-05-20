#ifndef maze_h
#define maze_h

#include<stdio.h>
#include<vector>
#include<stdlib.h>
#include<time.h>
using namespace std;

class Maze
{
private:

    static const int MAXS = 100; //maksymalny rozmiar mapy
    static const char SEP = '#'; //znak oznaczaj�cy separator
    static const int dx[4];
    static const int dy[4];

    char map[MAXS][MAXS]; //macierz znak�w symbolizuj�ca labirynt 
    int height,width; //wymiary labiryntu

    bool valid(int sty,int stx,int y,int x);
    void Generate(int sty,int stx);

public:

    Maze(int _height,int _width);

    char* operator[](int i);

    void GetSize(int& _height,int& _width);
    void Show();

};
#endif
