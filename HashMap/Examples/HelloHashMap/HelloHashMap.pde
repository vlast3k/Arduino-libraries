#include <HashMap.h>

//define the max size of the hashtable
const byte HASH_SIZE = 5; 
//storage 
HashType<char*,int> hashRawArray[HASH_SIZE]; 
//handles the storage [search,retrieve,insert]
HashMap<char*,int> hashMap = HashMap<char*,int>( hashRawArray , HASH_SIZE ); 

void setup(){
  //setup hashmap
  hashMap[0]("name",18);
  hashMap[1]("test",200);
  hashMap[2]("qwer",1234);
  hashMap[3]("abc",123);
  hashMap[4]("AlphaBeta",20);
  
  Serial.begin(9600);
  Serial.println( hashMap.getIndexOf("test"),DEC );
  Serial.println( hashMap.getValueOf("test") );
  
  hashMap.debug();
}

void loop(){/*nothing to loop*/}