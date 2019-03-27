//
//  main.cpp
//  MineSweep
//
//  Created by Zijian Xie on 3/27/19.
//  Copyright © 2019 Jason Xie. All rights reserved.
//

//mine sweeping
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <ctime>
#include <iomanip>
#include <ctime>
#include <time.h>
#include <chrono>
#include <thread>
#include <pthread.h>
using namespace std;
std::thread::id main_thread_id = std::this_thread::get_id();

class Node{
    public:
    Node(){};
    Node(int a, int b){x = a; y = b;}
    int x;
    int y;
};

class Clock{
	int h;
	int m;
	int s;
public:
	Clock(int hour, int minute, int sec){
		h = hour;
		m = minute;
		s = sec;
	}
	
	void set(int hour, int minute, int sec){
		h = hour;
		m = minute;
		s = sec;
	}
	
	 void tick(){
		//time_t curTime = time(NULL);
		this_thread::sleep_for(std::chrono::milliseconds(1000));
		//while(time(NULL) == curTime){
			if(--s < 0){
				s = 59;
				if(--m < 0){
					m = 59;
					--h;
				}
			}
		//}
	 }
	 
	 void show(){
		cout<<'\r';
		if(h<10) cout<<0;
		cout<<h<<":";
		if(m<10) cout<<0;
		cout<<m<<":";
		if(s<10) cout<<0;
		cout<<s<<flush;//Flushes the output sequence os as if by calling
	 }
	 
	 void run(){
		while(h != 0 || m != 0 || s != 0){
			tick();
			show();
		}
		throw("Time \'s Up!");
		return;
	 }
};

class MineSweepingGame{
public:
    MineSweepingGame(int n, int level){
        this->n = n;
        grid.resize(n, vector<int>(n, 0));
        visited.resize(n, vector<int>(n, 0));
        output.resize(n, vector<char>(n, '*'));
        mineMark.resize(n, vector<int>(n, 0));
        if(level == 1){
          total_mine = n*n*0.1;
          time_limit = 300;
        }else if(level == 2){
          total_mine = n*n*0.2;
          time_limit = 250;
        }else{
          total_mine = n*n*0.3;
          time_limit = 150;
        }
        initGrid();
		cout<<"You will have "<<time_limit/360<<" seconds to finish the game. Now it is counting! "<<endl;
    }
    
    void initGrid(){
        srand(unsigned(time(NULL)));
        int count = 0;
        while(true){
            int x = rand() % n;
            int y = rand() % n;
            if(mineMark[x][y] != -1){
                count++;
                mineMark[x][y] = -1;
                grid[x][y] = -1;
            }
            if(count >= total_mine) break;
        }
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                if(mineMark[i][j] == -1) continue;
                int numOfMine = 0;
                for(auto d : dir){
                    int ii = i + d[0];
                    int jj = j + d[1];
                    if(ii >= 0 && ii < n && jj >= 0 && jj < n){
                        numOfMine += mineMark[ii][jj] == -1;
                    }
                }
                grid[i][j] = numOfMine;
            }
        }
        return;
    }
    
    void bfs(int i, int j){
        //这里也就是我们需要把这些可以揭开的地方给揭开了 要怎样才能揭开呢？bfs来查找
        //如果周围没有雷  那自然是不能揭开的。 再或者我们抵达的某一个点，他自己本身就是雷 而这个点四周里面有不是雷的，这样他周围的就可以被显示出来
        queue<Node*> q;
        q.push(new Node(i, j));
        visited[i][j] = 1;
        while(!q.empty()){
            auto node = q.front(); q.pop();
            output[node->x][node->y] = grid[node->x][node->y] + '0';
            for(auto d : dir){
                int ii = node->x + d[0];
                int jj = node->y + d[1];
                if(ii >= 0 && ii < n && jj >= 0 && jj < n && !visited[ii][jj]){
                    if(grid[ii][jj] == 0){
                        visited[ii][jj] = 1;
                        output[ii][jj] = grid[ii][jj] + '0';
                        q.push(new Node(ii, jj));
                    }
                    if(grid[ii][jj] > 0 && grid[node->x][node->y] == 0){
                        visited[ii][jj] = 1;
                        output[ii][jj] = grid[ii][jj] + '0';
                        q.push(new Node(ii, jj));
                    }
                }
            }
        }
        return;
    }
    
    void printGrid(){
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                cout << setw(5) << left << output[i][j];
            }
            cout<<endl;
        }
    }
    
    void updateOneGrid(int x, int y){
        visited[x][y] = 1;
        output[x][y] = grid[x][y] + '0';
    }
    
    int n;
    int total_mine;
    vector<vector<int>> mineMark;
    vector<vector<int>> visited;
    vector<vector<int>> dir = {{0,1}, {1,0},{0,-1},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1}};
    vector<vector<int>> grid;
    vector<vector<char>> output;
    long time_limit = 300*360;
};

void PlayGame(int n, int level){
    MineSweepingGame* game = new MineSweepingGame(n, level);
    int x;
    int y;
    int count = 0;
    game->printGrid();
	while(true){
		if(count >= n*n - game->total_mine){
			throw("Win");
			break;
		}
		cout<<"Please input your location (index = 0): "<<endl;
		cin>>x>>y;
		if(x < 0 || x >= n || y < 0 || y >= n){
			cout<<"Invaild Input!"<<endl;
			continue;
		}
		if(game->mineMark[x][y] == -1){
			cout<<"You were BOOOOOOOOOOOM! Try it again!"<<endl;
			throw("You were BOOOOOOOOOOOM! Try it again!");
			break;
		}
		if(game->visited[x][y] == 1){
			cout<<"You have selected this place, please select a new position."<<endl;
			continue;
		}
		if(game->grid[x][y] == 0){
			game->bfs(x, y);
			game->printGrid();
		}else{
			//game->grid[x][y] > 0
			game->updateOneGrid(x, y);
			game->printGrid();
		}
	}
	return;
}

void CountDown(int n, int level){
	int hour = 0;
	int minute = (level == 1 ? 2 : (level == 2 ? 5 : 10));
	int second = 0;
	Clock* clock = new Clock(hour, minute, second);
	clock->run();
}

int main(){
    int n = 10;
    int level = 1;
    cout<<"Please enter a number between 10 and 50 (inclusive). It will be the size of your game "<<endl;
    cin>>n;
    cout<<"Please enter game level: enter 1 for easy, enter 2 for medium and enter 3 for hard"<<endl;
    cin>>level;
    if(n < 10 || n > 50 || level < 0 || level > 3){
        cout<<"Error! Goodbye."<<endl;
        return 0;
    }
	try{
		thread player(PlayGame, n, level);
		thread time(CountDown, n, level);
		player.join();
		time.join();
	}catch(const string msg){
		if(msg == "Win"){
			cout<<"You found all mines! Congratulations!"<<endl;
		}else{
			cout<<msg<<endl;
		}
	}
    return 0;
}
