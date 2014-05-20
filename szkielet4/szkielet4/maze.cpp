#include "maze.h"

    const int Maze::dx[4] = {1,-1,0,0}; //Dwie tablice u³atwiaj¹ce poruszanie siê po gridzie, para (dx[i],dy[i]) odpowiada przesuniêciu 
    const int Maze::dy[4] = {0,0,1,-1}; //siê odpowiednio na wschód, zachód, po³udnie i pó³noc od zadanego pola. 
										//Dziêki temu aby sprawdziæ s¹siednie pola wystarczy w pêtli odwo³ywaæ siê do pól current_x + dx[i],current_y + dy[i].



	//Funkcja sprawdzaj¹ca czy mo¿emy z zadanego pola wydr¹¿yæ pole s¹siednie.
    bool Maze::valid(int sty,int stx,int y,int x)
    {
        if(!(x>0 && y>0 && y<height-1 && x<width-1  && map[y][x] == SEP))return false; //Je¿eli s¹siednie pole wychodzi poza zakres lub zosta³o ju¿ wydr¹¿one zwróæ fa³sz.

        for(int i=0;i<4;i++)
            if( (x+dx[i] != stx || y+dy[i] != sty) && map[y+dy[i]][x+dx[i]] != SEP)return false; //Je¿eli jakikolwiek z s¹siadów analizowanego pola 
																								 //(nie licz¹c pola z którego przyszliœmy) jest ju¿ wydr¹¿ony 
																								 //to wydr¹¿enie analizowanego pola doprowadzi do cyklu, zatem zwróæ fa³sz.
        return true; //W innym przypadku zwróæ prawdê.
    }


	//G³ówne miêsko - losowo dr¹¿y korytarze w pocz¹tkowo ca³kowicie wype³nionej separatorami planszy. 
	//G³ówna idea przypomina dfsa - losowo i szczêœliwie przemy przed siebie dr¹¿¹c sobie kolejne korytarze.

    void Maze::Generate(int sty,int stx)
    {
        int nx,ny,r;
        vector<int>validx;
        vector<int>validy;

        map[sty][stx] = '.';  //Oznacz pole za wydr¹¿one.
        do              
        {					        
            validx.clear();          
            validy.clear();

            for(int i=0;i<4;i++)           //SprawdŸ wszystkich s¹siadów ostatnio wydr¹¿onego pola.
            {							
                nx = stx + dx[i];
                ny = sty + dy[i];

                if(valid(sty,stx,ny,nx))	//Je¿eli da siê wydr¹¿yæ s¹siednie pole - wrzuæ je na vector.	
                {
                    validx.push_back(nx);
                    validy.push_back(ny);
                }
            }

            if(validx.size())			//Je¿eli s¹ jacyœ s¹siedzi do których mo¿na przejœæ to wylosuj jednego z nich i odwiedŸ go. 
            {
                r = rand()%validx.size();
                Generate(validy[r],validx[r]);
            }
			
        }
        while(validx.size());  //Po odwiedzeniu s¹siada powtarzaj powy¿sze dopóki nie bêdzie ju¿ s¹siadów do których mo¿na przejœæ.
    }



    Maze::Maze(int _height,int _width) //Konstruktor, odpala generator liczb pseudolosowych, zape³nia planszê separatorami i generuje labirynt pocz¹wszy od pola (1,1). 
    {

        srand(time(0));
        height = _height; width = _width;

        for(int i=0;i<height;i++)
            for(int j=0;j<height;j++)
                map[i][j] = SEP;

        Generate(1,1);
    }

    char* Maze::operator[](int i) //Operator u³atwiaj¹cy odczyt danych. Zamiast obiekt.map[i][j] mo¿na stosowaæ obiekt[i][j].
	{ 
        return map[i];
    }

    void Maze::GetSize(int& _height,int& _width) //Pobranie wymiarów labiryntu.
    {
        _height = height; _width = width;
    }
	
    void Maze::Show()	//Wyœwietlanie mapy.
    {
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
                printf("%c",map[i][j]);
            printf("\n");
        }
    }


