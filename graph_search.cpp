/* 
 * Authors: FERRY / BIBAS
 */

#include <ctime>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "rocksdb/db.h"
#include "rocksdb/statistics.h"
#include <time.h>
#include <stdlib.h> 
#include <stdio.h>
#include <sstream>
#include <ctime>
#include <vector> 
#include <unordered_set>


using namespace std;
using namespace rocksdb;

/* nombre de keys à générer */
const int nb_keys = 100000;
/* nombre de niveaux à retourner */
const int nb_niveaux = 5;
int total_print ;
int connectionParNoeud = 80;
int iterationsStats = 10;
string txtFileName = "statsRecherche.txt";


//function non utilisee
void print_result(DB* db,string key,int niveau) {
   	
std::string value;
string niv_disp ;
for (int i = 0; i < niveau ; i++) {
niv_disp = niv_disp + "_ ";
}
   	Status status = db->Get(ReadOptions(), key, &value);
/* tokenisation de la valeur ( = récupération de la liste csv) */	
std::istringstream iss(value);
std::string token;
//cout << value << endl;	
while(getline(iss, token, ','))
{
if(!token.empty()){	     
//cout << niv_disp + token << endl;	
if (niveau < nb_niveaux) { 
total_print++;	
print_result(db, token, (niveau+1));
}
}
}    
}


long getPath(DB* db, string keyStart, string keyEnd) {
    std::vector<string> currentNodes;
    std::unordered_set<std::string> CurrentNodesSet;
    currentNodes.clear();    
    string currentNode = keyStart;
    currentNodes.push_back(keyStart);
    CurrentNodesSet.insert (keyStart);
    std::string value;
    bool foundEndKey = false;
    std::vector<string> nextNodes;
    std::unordered_set<std::string> NextNodesSet;

    int distance = 0;
    long nbNodes=0;
    while(foundEndKey==false){
    
        nbNodes++;
        for ( auto it = CurrentNodesSet.begin(); it != CurrentNodesSet.end(); ++it ) {
        
                Status status = db->Get(ReadOptions(), *it, &value);
                

    		/* tokenisation de la valeur ( = récupération de la liste csv) */	
    		
        std::istringstream iss(value);
    		std::string token;
    		//cout << value << endl;	
    		while(getline(iss, token, ','))
    		{
    		    if(!token.empty()){
                            NextNodesSet.insert (token);
                        }
                }

            std::unordered_set<std::string>::const_iterator got = NextNodesSet.find (keyEnd);
            if ( got == NextNodesSet.end() ) {
            }
            else {
                cout << "FOUND! at distance : " << to_string(distance +1) <<endl;
                foundEndKey= true;
                break;
            }

         }
         
         if(foundEndKey==true){
            break;
         } else {
            distance++;
            for ( auto it2 = NextNodesSet.begin(); it2 != NextNodesSet.end(); ++it2 )
            CurrentNodesSet.insert (*it2);
         //vide nexNodes

            NextNodesSet.clear();

         }
    }
return nbNodes;
}


int main(int argc, char** argv) {
 rocksdb::DB *db;
 rocksdb::Options options;
 options.create_if_missing = true;
 options.statistics = CreateDBStatistics();
 rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb/", &db);
 assert(status.ok());
 std::time_t tStart = std::time(0);

/* initialize random seed: */
  srand (time(NULL));
 
 for(long int i=0; i < nb_keys ; i++){
std::string value;
// value = concaténation de 10 keys choisies au hasard
for (int j = 0;j<connectionParNoeud;j++) {
value = value + "," + std::to_string(rand() % nb_keys);
}	
db->Put(rocksdb::WriteOptions(), to_string(i), value);


}
  

std::time_t tFinishCreating = std::time(0);
cout << "GRAPH CREATED - Time = "  << tFinishCreating - tStart << "sec" << endl;  

long nbNodes;
ofstream myfile (txtFileName);
  if (myfile.is_open())
  {
	for (int j = 0;j<iterationsStats;j++) {
  	//time_t tStart = std::time(0);
    std::clock_t tStart = std::clock(); 	
  	nbNodes = getPath(db,to_string(rand() % nb_keys),to_string(rand() % nb_keys));
  	std::clock_t tEnd=std::clock(); 
    cout <<  to_string(((double)(tEnd-tStart)/CLOCKS_PER_SEC)) << endl ;
    myfile << to_string(nbNodes) + ";" + to_string(((double)(tEnd-tStart)/CLOCKS_PER_SEC)) + "\n";
  	}    

  myfile.close();
  }
  else cout << "Unable to open file";


 delete db;

 return 0;
}
