#include "maze.h"

    const int Maze::dx[4] = {1,-1,0,0}; //Dwie tablice u�atwiaj�ce poruszanie si� po gridzie, para (dx[i],dy[i]) odpowiada przesuni�ciu 
    const int Maze::dy[4] = {0,0,1,-1}; //si� odpowiednio na wsch�d, zach�d, po�udnie i p�noc od zadanego pola. 
										//Dzi�ki temu aby sprawdzi� s�siednie pola wystarczy w p�tli odwo�ywa� si� do p�l current_x + dx[i],current_y + dy[i].



	//Funkcja sprawdzaj�ca czy mo�emy z zadanego pola wydr��y� pole s�siednie.
    bool Maze::valid(int sty,int stx,int y,int x,int layer)
    {
		if (!(x>0 && y>0 && y<height - 1 && x<width - 1 && map[layer][y][x] == SEP))return false; //Je�eli s�siednie pole wychodzi poza zakres lub zosta�o ju� wydr��one zwr�� fa�sz.

        for(int i=0;i<4;i++)
		if ((x + dx[i] != stx || y + dy[i] != sty) && map[layer][y + dy[i]][x + dx[i]] != SEP)return false; //Je�eli jakikolwiek z s�siad�w analizowanego pola 
																								 //(nie licz�c pola z kt�rego przyszli�my) jest ju� wydr��ony 
																								 //to wydr��enie analizowanego pola doprowadzi do cyklu, zatem zwr�� fa�sz.
        return true; //W innym przypadku zwr�� prawd�.
    }


	//G��wne mi�sko - losowo dr��y korytarze w pocz�tkowo ca�kowicie wype�nionej separatorami planszy. 
	//G��wna idea przypomina dfsa - losowo i szcz�liwie przemy przed siebie dr���c sobie kolejne korytarze.

	void Maze::Generate(int sty, int stx, int layer)
	{
		int nx, ny, r; bool leaf = true;
		vector<int>validx;
		vector<int>validy;

		map[layer][sty][stx] = '.';  //Oznacz pole za wydr��one.
		do
		{
			validx.clear();
			validy.clear();

			for (int i = 0; i < 4; i++)           //Sprawd� wszystkich s�siad�w ostatnio wydr��onego pola.
			{
				nx = stx + dx[i];
				ny = sty + dy[i];

				if (valid(sty, stx, ny, nx,layer))	//Je�eli da si� wydr��y� s�siednie pole - wrzu� je na vector.	
				{
					validx.push_back(nx);
					validy.push_back(ny);
					leaf = false;
				}
			}

			if (validx.size())			//Je�eli s� jacy� s�siedzi do kt�rych mo�na przej�� to wylosuj jednego z nich i odwied� go. 
			{
				r = rand() % validx.size();
				Generate(validy[r], validx[r],layer);
			}

		} while (validx.size());  //Po odwiedzeniu s�siada powtarzaj powy�sze dop�ki nie b�dzie ju� s�siad�w do kt�rych mo�na przej��.

		if (leaf)
		{
			leafx.push_back(stx);
			leafy.push_back(sty);
		}
	}


    Maze::Maze(int _height,int _width,int _layers) //Konstruktor, odpala generator liczb pseudolosowych, zape�nia plansz� separatorami i generuje labirynt pocz�wszy od pola (1,1). 
    {

        srand(time(0));
		height = _height; width = _width; layers = _layers;
		cur_layer = 0;
		int sty = 1, stx = 1;
		
		for (int k = 0; k < layers; k++)
		{
			for (int i = 0; i < height; i++)
			for (int j = 0; j < height; j++)
				map[k][i][j] = SEP;
			
			Generate(sty, stx,k);
			
			if(k) map[k][sty][stx] = 'd';

			int r = rand() % leafx.size();
			sty = leafy[r]; stx = leafx[r];
			
			if (k != layers - 1){
				map[k][sty][stx] = 'u';
				this->elevator_i = sty;
				this->elevator_j = stx;
			}

			leafx.clear();
			leafy.clear();
		}
    }

    char* Maze::operator[](int i) //Operator u�atwiaj�cy odczyt danych. Zamiast obiekt.map[i][j] mo�na stosowa� obiekt[i][j].
	{ 
        return map[cur_layer][i];
    }

	int Maze::GetLayers()
	{
		return layers;
	}

	void Maze::NextLayer(){ cur_layer++; }
	void Maze::PreviousLayer(){ cur_layer--; }

    void Maze::GetSize(int& _height,int& _width) //Pobranie wymiar�w labiryntu.
    {
        _height = height; _width = width;
    }
	
    void Maze::Show(int layer)	//Wy�wietlanie mapy.
    {
        for(int i=0;i<height;i++)
        {
            for(int j=0;j<width;j++)
                printf("%c",map[layer][i][j]);
            printf("\n");
        }
		for (int i = 0; i < leafy.size(); i++)
			printf("%d %d\n", leafy[i], leafx[i]);

    }

	int Maze::getElevatorI(){
		return this->elevator_i;
	}

	int Maze::getElevatorJ(){
		return this->elevator_j;
	}
