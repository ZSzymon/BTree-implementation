#include <iostream>
#include "Btree.h"
#include "map"
#include "set"

#include "assert.h"
class Test{
   BTree<int,int> tree;
   std::map<int,int> toInsert,toDelete,notDeleted;
   std::set<pair<int,int>> set;

   void prepareNumbersToInsert(){
       int howMany = 100000000;
       int key,val;
       for(int i=0;i<howMany; i++){
           key = rand()%INT_MAX;
           val = rand()%INT_MAX;
           auto pairToInsert = make_pair(key,val);

           toInsert.insert(pairToInsert);

       }
   }
   void prepareNumberToDelete(){
       auto howMany = toInsert.size();
       size_t i=0;
       for(auto it : toInsert){
           if(i<howMany/2 && i>howMany/5){
               toDelete.insert(it);
           }else{
               notDeleted.insert(it);
           }
           i++;
       }
   }
public:
   Test(){
       prepareNumbersToInsert();
       prepareNumberToDelete();
   }
   void insertTest(){
       for(auto it : toInsert){
           tree.insert(it.first, it.second);
           auto inserted = tree.search(it.first);
           assert(inserted && inserted->key == it.first);

       }
   }
   void removeTest(){
       for(auto it : toDelete){
           tree.remove(it.first);
           auto removed = tree.search(it.first);
           assert(!removed);

       }
       for(auto it : notDeleted){
           auto notDeletedElement = tree.search(it.first);
           assert(notDeletedElement);

       }
   }


};


int main(){

    Test test;
    test.insertTest();
    test.removeTest();


    return 0;
}

