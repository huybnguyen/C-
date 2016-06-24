#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <map>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <stack>

using namespace std;

void compare(int group_size , string s1, string s2, int s1_index, int s2_index, int sequence1_length, int sequence2_length, vector <float> &my_vector);
// void compare2(int group_size, int sequence_length, float value1_s1, float value1_s2, int my_next);
//void my_recursion(int group_size,int my_length, int sequence_length, float last_k, float value1, float value2, vector <float>& my_vector, map <pair<int,int> , float> my_map, int k, int my_index, int start);

float result2 = 0;  //storing K2
float result3 = 0;  //storing K3(S2,S2)
float result2_1 = 0; //storing K2
float result3_1 = 0; //storing K2
float result2_2 = 0; //storing K3(S1,S1)
float result3_2 = 0; //storing K3(S2,S2)
vector <float> my_k2_1; //storing all K2(S1,S1) with different length
vector <float> my_k2_2; //storing all K2(S2,S2) with different length
vector <float> my_k2; //storing all K2(S1,S2) with different length
vector < vector<string> > matrix; //2D vector
double beta = 0.01;
string my_colum, my_row; //a copy of first row and colum of the BLOSUM62 matrix

int main()
{

	string filename;
  string line, sequence_file1, sequence_file2;
	cout<< "Please enter the name of the file: ";
	cin >> filename;
  cout<< "Please enter the sequence 1: ";
  cin >> sequence_file1;
  cout << "Please enter the sequence 2: ";
  cin >> sequence_file2;

  ifstream inputFile (filename.c_str());
  ifstream sequence1 (sequence_file1.c_str());
  ifstream sequence2 (sequence_file2.c_str());

  if(inputFile.fail() || sequence1.fail() || sequence2.fail())   //Return error message if not recognize the file
  {
    cerr << "Error Opening File" << endl;
    return(1);
  }

  while (inputFile)
  {
    if (!getline( inputFile, line )) break;
    istringstream ss( line );    //getting the stuff from the read in file.
    vector <string> data;
    while (ss)
    {
      string s;
      if (!getline( ss, s, ' ' )) break;  //stop if there no longer space in between
        data.push_back( s );   //grabbing the string
    }

    matrix.push_back( data ); //pushing data into the matrix
  }

  string dummyline;  
  string data1, data2;
  getline(sequence1, data1);  //ignore the headers
  getline(sequence2, data2);
  string mydata, mydata2;
  while(sequence1)   //reading and storing sequence 1
  {
    if(!getline(sequence1, data1)) break;
    mydata.append(data1); //if not end of file then store mydata string to new string.

  }

  while(sequence2) //reading and storing sequence 2
  {
    if(!getline(sequence2, data2)) break;
    mydata2.append(data2);  //if not end of file then store mydata string to new string.
  }

  //cout<<"Sequence1: "<<mydata<<" Sequence2: "<<mydata2<<endl;  //for testing
  string s1, s2;
  if(mydata.length() <= mydata2.length())  //comparing string to make s1 as the one with lower length.
  {
    s1.append(mydata); 
    s2.append(mydata2);
  }
  else{
    s1.append(mydata2);
    s2.append(mydata);
  }

  int s1_position = 0;       //storing positions
  int s2_position = 0;
  int s1_position_1 = 0;
  int s2_position_1 = 0;
  int s1_position_2 = 0;
  int s2_position_2 = 0;


  for(int i=0; i<matrix[0].size(); i++)
  {
    my_colum.append(matrix[i+1][0]);
  }

  for(int i=0; i<matrix.size(); i++)
  {
    my_row.append(matrix[0][i]);
  }


  float vals = 0;  //storing values grabbing from the matrix 
  float vals1 = 0;
  float vals2 = 0;

 
int group_size = 1, s1_index = 0, s2_index = 0;
compare(group_size, s1, s2, s1_index, s2_index, s1.length(), s2.length(), my_k2);
//cout<<"END OF RECURSIVE S1,S2"<<endl;
compare(group_size, s1, s1, s1_index, s2_index, s1.length(), s1.length(), my_k2_1);
//cout<<"END OF RECURSIVE S1,S1"<<endl;
compare(group_size, s2, s2, s1_index, s2_index, s2.length(), s2.length(), my_k2_2);
//cout<<"END OF RECURSIVE S2,S2"<<endl;

  for(int i=0;i<my_k2.size();i++){  //adding up all the K2 in K2 table for S1,S2
    result2 += my_k2[i];
    //cout<<"K2V: "<<my_k2[i]<<endl;
  }

  for(int i=0;i<my_k2_1.size();i++){ //adding up all the K2 in K2 table for S1,S1
    result2_1 += my_k2_1[i];
    //cout<<"K2V: "<<my_k2[i]<<endl;
  }

  for(int i=0;i<my_k2_2.size();i++){ //adding up all the K2 in K2 table for S2,S2
    result2_2 += my_k2_2[i];
    //cout<<"K2V: "<<my_k2[i]<<endl;
  }

   // result3 = result2 + my_k2[0]; //adding in the K2 of L1 for S1,S2
   // result3_1 = result2_1 + my_k2_1[0]; //adding in the K2 of L1 for S1,S1
   // result3_2 = result2_2 + my_k2_2[0]; //adding in the K2 of L1  for S2,S2
  //cout<<"result 2: "<<result2<<endl;
  //cout<<"result 2_1: "<<result2_1<<endl;
  //cout<<"result 2_2: "<<result2_2<<endl;
  float total = 0;  //Storing the value for K3 hat
  float answer = 0; //Final answer
  total = result2/sqrt(result2_1 * result2_2);
  if(s1 == s2)
   total = floor(total);
  answer = sqrt(2*abs(1-total));
  
  cout<<"K2: "<<my_k2[0]<<endl;
  cout<<"K3(S1,S2): "<<result2<<endl;
  cout<<"K3(S1,S1): "<<result2_1<<endl;
  cout<<"K3(S2,S2): "<<result2_2<<endl;
  cout<<"K3_hat: "<<total<<endl;
  cout<<"Result: "<<setprecision(6)<<answer<<endl;


  // map <pair<int,int> , float>::iterator it;
  // cout<<s1.length()<<" "<<s2.length()<<endl;
  // // cout<<"TABLE"<<endl;
  // for(it = value_table2.begin(); it != value_table2.end(); ++it)
  // {
  //   cout << it->first.first << " " << it->first.second << " " << it->second << "\n";
  // }
  
  return 0;
} //main()



void compare(int group_size, string s1, string s2, int s1_index, int s2_index, int sequence1_length, int sequence2_length, vector <float> &my_vector)
{
  //cout<<"GROUP_SIZE: "<<group_size<<endl;
  if(group_size > sequence1_length){
    return;
  }
  else{
    // int advance1 = group_size;
    // int advance2 = group_size;
    float my_result = 0, total = 0, my_value = 1;
    int x = 0;
    vector <float> solution;
    int k = 0;
    string new_s1 = "";
    string new_s2 = "";
    while(1){
      new_s1 = s1.substr(s1_index, group_size);
      //cout<<"here"<<endl;
      while(s2_index <= sequence2_length - group_size){
        //cout<<"here2"<<endl;
        new_s2 = s2.substr(s2_index, group_size);
        //cout<<"NEW: "<<new_s2<<endl;
        if(group_size < 2){
          //cout<<"string: "<<new_s1[0]<<" found at: "<<my_colum.find(new_s1[0])<<" string: "<<new_s2[0]<<" found at: "<<my_row.find(new_s2[0])<<" value: "<<pow(stof(matrix[my_colum.find(new_s2[0])+1][my_row.find(new_s1[0])+1]), beta) <<" Original: "<<matrix[my_colum.find(new_s2[0])+1][my_row.find(new_s1[0]) +1]<<endl;
          float value = pow(stof(matrix[my_colum.find(new_s1[0])+1][my_row.find(new_s2[0]) + 1]), beta);//my_map[make_pair(new_s1[0],new_s2[0])];
          my_result += value;
          //cout<<"LETTER: "<<new_s1<<" and LETTER: "<<new_s2<<"value: "<<my_map[make_pair(new_s1[0],new_s2[0])]<<" result: "<<my_result<<endl; 
          //cout<<"LETTER: "<<new_s1<<" and LETTER: "<<new_s2<<"value: "<<value<<" result: "<<my_result<<endl;
        }
        
        else{
          for(int k = 0; k<new_s2.length(); k++){
            //cout<<"checking: "<<new_s1[k]<<" and "<<new_s2[k]<<endl;
            //solution *= my_map[make_pair(new_s1[k],new_s2[k])];
            //cout<<"string: "<<new_s1[k]<<" found at: "<<my_colum.find(new_s1[k])<<" string: "<<new_s2[k]<<" found at: "<<my_row.find(new_s2[k])<<" value: "<<pow(stof(matrix[my_colum.find(new_s2[k])+1][my_row.find(new_s1[k])+1]), beta) <<" Original: "<<matrix[my_colum.find(new_s2[k])+1][my_row.find(new_s1[k]) +1]<<endl;
            float value = pow(stof(matrix[my_colum.find(new_s1[k])+1][my_row.find(new_s2[k])+1]), beta);
            solution.push_back(value);
            //cout<<"LETTER: "<<new_s1<<" and LETTER: "<<new_s2<<"value: "<<value<<endl;
          }

          for(int i = 0 ; i<solution.size();i++){
            my_value *= solution[i];
            //cout<<"testing "<<my_value<<endl;
          }
          
          //cout<<"myvalue: "<<my_value<<endl;
          total += my_value;
          //cout<<"total: "<<total<<endl;
          my_value = 1;
          solution.clear();
        }
        s2_index++;
      }
      s2_index = 0;
      s1_index++;
      //cout<<"index: "<<s1_index<<" " << sequence1_length - group_size<<endl;
      if(s1_index > sequence1_length - group_size)
        break;     
    }
    if(group_size > 1){
      my_vector.push_back(total);
      //cout<<total<<endl;
    }
    else{
      my_vector.push_back(my_result);
    }
    //cout<<"DONE"<<endl;
    
    s2_index = 0;
    s1_index = 0;
    group_size++;  
    compare(group_size, s1, s2, s1_index, s2_index, sequence1_length, sequence2_length, my_vector);
  }
}

