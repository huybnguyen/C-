#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;
int main(int argc, char *argv[]) {
    ifstream inputFile;
    string s2(argv[1]);
    inputFile.open(s2);
    char line[50];
    int X = 0;
    int Y = 0;
    int range = 0;
    char arrayX[10];
    char arrayY[10];
    char arrayrange[10];
    int n = 0;
    char buff[10];
    char* token;
    while(!inputFile.eof()) {
 		inputFile.getline(line, 50);
 		strcpy(arrayX, strtok(line , " "));
 		X = atoi(arrayX);
    	strcpy(arrayY, strtok(NULL, " "));
    	Y = atoi(arrayY);
   		strcpy(arrayrange , strtok(NULL, " "));
   		range = atoi(arrayrange);
   		for(int i = 1; i<= range;i++){
   			if(i % X == 0 && i % Y ==0){
   				cout<<"FB ";
   			}
   			else if(i % X == 0){
   				cout<<"F ";		
   			}
   			else if(i % Y == 0){
   				cout<<"B ";
   			}
   			else{
   				cout<<i<<" ";
   			}
   		}
   		cout<<endl;
    }
    inputFile.close();
    return 0;
} 