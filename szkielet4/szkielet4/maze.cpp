#include "maze.h"

    const int Maze::dx[4] = {1,-1,0,0};
    const int Maze::dy[4] = {0,0,1,-1};

    bool Maze::valid(int sty,int stx,int y,int x)
    {
        if(!(x>0 && y>0 && y<height-1 && x<width-1  && map[y][x] == SEP))return false;

        for(int i=0;i<4;i++)
            if( (x+dx[i] != stx || y+dy[i] != sty) && map[y+dy[i]][x+dx[i]] != SEP)return false;

        return true;
    }

    void Maze::Generate(int sty,int stx)
    {
        int nx,ny,r;
        vector<int>validx;
        vector<int>validy;

        map[sty][stx] = '.';
        do
        {
            validx.clear();
            validy.clear();

            for(int i=0;i<4;i++)
            {
                nx = stx + dx[i];
                ny = sty + dy[i];

                if(valid(sty,stx,ny,nx))
                {
                    validx.push_back(nx);
                    validy.push_back(ny);
                }
            }

            if(validx.size())
            {
                r = rand()%validx.size();
                Generate(validy[r],validx[r]);
            }
        }
        while(validx.size());
    }



    Maze::Maze(int _height,int _width)
    {

        srand(time(0));
        height = _height; width = _width;

        for(int i=0;i<height;i++)
            for(int j=0;j<height;j++)
                map[i][j] = SEP;

        Generate(1,1);
    }

    char* Maze::operator[](int i){
        return map[i];
    }

    void Maze::GetSize(int& _height,int& _width)
    {
        _height = height; _width = width;
    }

    void Maze::Show()
    {
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
                printf("%c",map[i][j]);
            printf("\n");
        }
    }


