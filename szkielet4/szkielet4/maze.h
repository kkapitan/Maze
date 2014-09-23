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

    static const int MAXS = 50; //maksymalny rozmiar mapy
	static const int MAXL = 5;
    static const char SEP = '#'; //znak oznaczaj¹cy separator
    static const int dx[4];
    static const int dy[4];

	int elevator_i;
	int elevator_j;
	
	vector<int>leafx;
	vector<int>leafy;

	char map[MAXL][MAXS][MAXS]; //macierz znaków symbolizuj¹ca labirynt 
    int height,width,layers; //wymiary labiryntu
	
	int cur_layer;

    bool valid(int sty,int stx,int y,int x,int layer);
    void Generate(int sty,int stx,int layer);

public:

    Maze(int _height,int _width,int layers);

    char* operator[](int i);

    void GetSize(int& _height,int& _width);
	int GetLayers();
	void NextLayer();
	void PreviousLayer();
	void Show(int layer);

	int getElevatorI();
	int getElevatorJ();
};
#endif
