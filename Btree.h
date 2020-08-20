#ifndef BTREE_H
#define BTREE_H
#include <vector>
#include <iostream>

using std::cout;
using std::pair;
using std::pair;
using std::make_pair;

template<class K, class V,class Comparator = std::less_equal<K>>
struct BaseNode{

    K key;
    V value;
    static Comparator comp;
    BaseNode(K key, V value){
    this->key = key; this->value = value;
    }

    bool operator<(BaseNode<K,V,Comparator> &bn){

        return comp(this->key, bn.key) < 1;
    }
    bool operator>(BaseNode<K,V,Comparator> &bn){
        return comp(this->key, bn.key) > 1;
    }
    bool operator>=(BaseNode<K,V,Comparator> &bn){
        return comp(this->key, bn.key) >= 1;
    }
    bool operator<=(BaseNode<K,V,Comparator> &bn){
        return comp(this->key, bn.key) <= 1;
    }
    bool operator==(BaseNode<K,V,Comparator> &bn){
        return comp(this->key, bn.key) == 0;
    }



};



template<class K, class V,class Comparator = std::less_equal<K>>
struct BTNode{
public:
    typedef BaseNode<K,V,Comparator> base_node;
    typedef BTNode<K,V,Comparator> Node;
private:
    BTNode *searchBTNodeContainingKey(K key){
        int i;
        for(i=0;i<this->keysSize;i++){
            if(!isLeaf)
                childs[i]->search(key);
            if(keys[i]->key == key)
                return this;
        }
        if(!isLeaf)
            childs[i]->search(key);

    }

public:

    int t,keysSize;
    std::vector<base_node*> keys;
    std::vector<Node*> childs;
    bool isLeaf;

    BTNode(int t,bool isLeaf)
        :t(t),keysSize(0){
        this->isLeaf = isLeaf;
    }
    //void insertNotNull(BaseNode *newNode);
    bool areKeysFull(){
        return keys.size() == 2*t-1;
    }
    bool isChildFull(Node* child){
        return child->areKeysFull();
    }
    auto getMiddleIt(){
        return keys.begin() + keysSize/2;
    }

    base_node *getBiggestInSubTree(int id);

    base_node *getSmallestInSubTree(int id){
        typename std::vector<BTNode*>::iterator current = begin(childs)+id+1;
        while(!(*current)->isLeaf)
            current = begin((*current)->childs);
        return (*current)->keys.front();
    }


    BTNode *getNodeWithKey(K key){
        int i=0;
        //klcuze w porzadku rosnacym wiec dlatego do momentu kiedy key wiekszy od klucza[i]
        for( ; i<keysSize && key > keys[i]->key; i++){}

        if( i < keysSize && keys[i]->key == key)
            return this;

        if(isLeaf)
            return nullptr;
        if(i < childs.size())
            return childs[i]->getNodeWithKey(key);
    }


    void insertNotNull(base_node *newNode){
        if(isLeaf){
            auto it = keys.begin();
            while(it != keys.end() && (*(*it)).key < *(&newNode->key) &&  *it != nullptr){
                it++;
            }

            keys.insert(it,newNode);
            keysSize++;
        }else{
            size_t i=0;
            while(i< keysSize && newNode->key > keys[i]->key){
                i++;
            }
            if(isChildFull(childs[i])){
                splitChild(childs[i],i);
    //po splicie sprawdz w ktorym dziecko jest dla niego miejsce.
                if(newNode->key > keys[i]->key )
                    i++;
            }
            childs[i]->insertNotNull(newNode);
        }
    }

    void printTree(){

        int i;
        for(i=0;i<this->keysSize;i++){
            if(!isLeaf)
                childs[i]->printTree();
            cout<<keys[i]->key.first<<" "<<keys[i]->value<<"\n";
        }
        if(!isLeaf)
            childs[i]->printTree();
    }



    void printKeysInRange(K min ,K max){

        if(!isLeaf)
            childs.back()->printKeysInRange(min,max);
        int i = keys.size()-1;
        for(; i>= 0; i--){
            if(keys[i]->key >=min && keys[i]->key <=max){
                cout<<keys[i]->value<<"\n";
            }
            if(!isLeaf && i<childs.size()){
                childs[i]->printKeysInRange(min,max);
            }
        }
    }


    void splitChild(BTNode *childToSplit, int indexOfChild){
        int t = childToSplit->t;
        BTNode *newChild = new BTNode(childToSplit->t, childToSplit->isLeaf);

        auto itMiddle = childToSplit->keys.begin() + childToSplit->keysSize/2;

        //przepisz klucze.
        newChild->keys.assign(itMiddle+1,childToSplit->keys.end());
        newChild->keysSize = t-1;
        childToSplit->keys.erase(itMiddle,childToSplit->keys.end());

        if(!childToSplit->isLeaf){
            //int numberOfElements = distance(child->childs.begin()+t,child->childs.end());
            newChild->childs.assign(childToSplit->childs.begin()+t,childToSplit->childs.end());
            childToSplit->childs.erase(childToSplit->childs.begin()+t,childToSplit->childs.end());
        }
        //reduce size.
        childToSplit->keysSize = t-1;

        //insertInNewChildToChilds.
        this->childs.insert(this->childs.begin()+indexOfChild+1,newChild);

        //insertNewKey  (itMiddle);
        this->keys.insert(this->keys.begin()+indexOfChild,*itMiddle);
        this->keysSize++;

    }

    void merge(int id){
        BTNode *child = this->childs[id];
        BTNode *sibiling = this->childs[id+1];
        //do dzicka dodaj klucz.
        child->keys.push_back(this->keys[id]);
        //dodaj dzieci brata(brat jest wiekszy wiec na koniec moich dzieci.)
        child->keys.insert(end(child->keys),begin(sibiling->keys),end(sibiling->keys));
        if(!child->isLeaf){
            child->childs.insert(end(child->childs),begin(sibiling->childs),end(sibiling->childs));
        }
        //usun klucz ktory z node. bo on jest w polowie child.
        keys.erase(begin(keys) + id);//moze tutaj//
        //w marge byla zmiana przy marge(id).
        //usun sibling.
        this->childs.erase(begin(this->childs) + id+1);
        child->keysSize += sibiling->keysSize+1;
        this->keysSize--;

    }

    pair<int, bool> getIndexOfKey(K key){
        int i=0;
        while((i < keysSize && keys[i]->key <= key) ){
            i++;
            if(i-1 <keysSize && keys[i-1]->key == key)
                return make_pair<int,bool>(int(i-1),true);
        }
        return make_pair<int,bool>(int(i),false);
    }

    void removeFromInternalNode(int id){
        K key = keys[id]->key;
        //jezeli da sie zwiac poprzednika
        if( childs[id]->keysSize >=t ){
            base_node *predicant = getBiggestInSubTree(id);
            keys[id] = predicant;
            //usun poprzednik
            childs[id]->remove(predicant->key);
        }
        else if(childs[id+1]->keysSize >=t){

            base_node *successor = getSmallestInSubTree(id);
            keys[id] = successor;

            childs[id+1]->remove(successor->key);
        }
        else{
            //w innym przypadku zlacz i usun.
            merge(id);
            childs[id]->remove(key);
        }
        return;
    }

    base_node *search(K key){
        int i;
        for(i=0;i<this->keysSize;i++){
            if(!isLeaf)
                childs[i]->search(key);
            if(keys[i]->key == key)
                return keys[i];
        }
        if(!isLeaf)
            childs[i]->search(key);

    }

    void removeInLeaf(int id){
        auto itToDelete = keys.begin() + id;
        keys.erase(itToDelete);
    }

    void removeFromLeafNode(typename std::vector<base_node*>::iterator it){
        keys.erase(it);
        this->keysSize--;
        return;
    }

    void remove(K key){

        auto infoAboutKey = getIndexOfKey(key);
        auto id = infoAboutKey.first;
        bool isInThisNode = infoAboutKey.second;


        if(isInThisNode == true){
            if(isLeaf){
                removeFromLeafNode(keys.begin() + id);
            }else{
                removeFromInternalNode(id);
            }
        }else{
            //is not in this node
            if(isLeaf)
            {
                //Elementu nie ma w drzewie.

                //assert(!isLeaf);
            }

            bool isInLastChild = id == keysSize;
            if(childs[id]->keysSize < t){
                provideEnoughtSpace(id);
                 //id = getIndexOfKey(key).first;
            }

            //childs[id]->remove(key);
           if(isInLastChild && id > keysSize)
                childs[id-1]->remove(key);
           else
                childs[id]->remove(key);

        }
        return;

    }

    bool hasChildEnoughtToBorrow(int id){
        if(childs[id]->keysSize >=t)
            return true;
        else
            return false;
    }

    void provideEnoughtSpace(int id){
        //porzycz od pierwszego.
        if(id > 0 && hasChildEnoughtToBorrow(id-1)){
            borrowFromPrev(id);
        }
        //albo od drugiego
        else if(id != keysSize && hasChildEnoughtToBorrow(id+1)){
            borrowFromNext(id);
        }else{
            //a jak nie to ich złacz.
            if(id != keysSize)
                merge(id);
            else
                merge(id-1);
        }

    }

    void borrowFromNext(int id){
        BTNode *child = childs[id];
        BTNode *sibliging = childs[id+1];
        //wez klucz z noda. wrzuc do dziecka.
        child->keys.push_back(keys[id]);
        if(!child->isLeaf){
            //jezeli to nie leaf to wez takze pierwsze dziecko brata (bo to dzieci wieksze od ostatniego elementu.
            //a brat wiekszy od child.
            child->childs.push_back(sibliging->childs.front());
        }
        //z brata do kluczy.
        keys[id] = sibliging->keys.front();
        sibliging->keys.erase(sibliging->keys.begin());

        if(!sibliging->isLeaf){
            sibliging->childs.erase(begin(sibliging->childs));
            //te dziecko teraz jest w child wiec usun.
        }

        child->keysSize +=1;
        sibliging->keysSize -=1;

    }

    void borrowFromPrev(int id){
        BTNode *child = childs[id];
        BTNode *sibliging = childs[id-1];

        child->keys.insert(begin(child->keys), keys[id-1]);
        if(!child->isLeaf){
            child->childs.insert(begin(child->childs),sibliging->childs.back());
        }
        keys[id-1] = sibliging->keys.back();
        sibliging->keys.pop_back();
        sibliging->childs.pop_back();
        child->keysSize +=1;
        sibliging->keysSize -=1;


        return;
    }

    base_node *searchV2(K key){
        int i=0;
        //klcuze w porzadku rosnacym wiec dlatego do momentu kiedy key wiekszy od klucza[i]
        for( ; i<keysSize && key > keys[i]->key; i++){}

        if( i < keysSize && keys[i]->key == key)
            return keys[i];

        if(isLeaf)
            return nullptr;
        if(i < childs.size())
            return childs[i]->searchV2(key);
    }

    K getMinKey(BTNode *currentNode){
        if(currentNode && currentNode->isLeaf)
            return (*currentNode->keys.begin())->key;
        if(!currentNode->isLeaf)
            getMinKey(*currentNode->childs.begin());
    }
    K getMaxKey(BTNode *currentNode){
        if(currentNode && currentNode->isLeaf)
            return (*(--currentNode->keys.end()))->key;
        if(!currentNode->isLeaf)
            getMaxKey(*(--currentNode->childs.end()));
    }


};

template<class K, class V, class Comparator>
typename BTNode<K,V,Comparator>::base_node *BTNode<K,V,Comparator>::getBiggestInSubTree(int id){
    typename std::vector<Node*>::iterator current = begin(childs)+id;
    while(!(*current)->isLeaf)
        current = (*current)->childs.end()-1;
    return (*current)->keys.back();
}


template <class K,class V,class Comparator = std::less_equal<K>>
class BTree{
    class _Iterator{
        _Iterator end,notEnd;
        bool wasEnd= false;
        BTree *btree;
        BaseNode<K,V,Comparator> *baseNode = nullptr;
        void setEnd(){
            end.baseNode = btree->getMax();
        }
    public:
        _Iterator& operator++(){

            if(baseNode){
                //baseNode = getSuccesor(baseNode);

            }
            if(baseNode == end)
                wasEnd = true;
            return *this;
       }
        _Iterator &beginIt(){
            //retrun begin the smallest element.
            _Iterator it;
            it.btree->baseNode = it.btree->getMin();
            return *it;

        }
        _Iterator &endIt(){
            return wasEnd ? end : notEnd;
        }
        _Iterator(BTree *treeRoot){
            setTree(treeRoot);
            end.setTree(treeRoot);
        }
        void setTree(BTree *tree){
            btree = tree;
        }
    };

public:
    class Iterator{
       //builderForIterator.
        _Iterator *it;
        Iterator(){
            it = new _Iterator(this);
        }
    };
    typedef BTNode<K,V,Comparator> Node;
    typedef BaseNode<K,V,Comparator> base_node;
    Node *root;
    int t;
    BTree(int t=15):t(t){
        root = nullptr;
    }
    K getMin(){
        return root->getMinKey(root);
    }
    K getMax(){
        return root->getMaxKey(root);
    }
    void printTree(){
        if(root)
            root->printTree();
    }
    void printKeysInRange(int min, int max){
        if(root)
            root->printKeysInRange(min,max);
    }
    //void insert(int key,string name="");
    //void remove(int key);
    base_node *search(int key){
        if(!root)
            return nullptr;
        return root->searchV2(key);
    }



    void insert(K key, V name){
        base_node *newEl = new base_node(key,name);
        if(!root){
            root = new Node(t,true);
            root->keys.insert(root->keys.begin(),newEl);
            root->keysSize++;
        }else{
            if(!root->areKeysFull()){
                root->insertNotNull(newEl);
            }else{
                Node *newRoot = new Node(t,false);
                newRoot->childs.push_back(root);
                newRoot->splitChild(root,0);

                int i=0;
                if(newEl->key > newRoot->keys[i]->key)
                    i++;

                newRoot->childs[i]->insertNotNull(newEl);
                root = newRoot;
            }
        }
    }

    void remove(K key){
        if(!root)
            return;

        root->remove(key);

        if(root->keysSize == 0){
            //w przypadku kiedy nastapił marge dwóch jedynych dzieci. root nie ma elementow.
            //prawdziwy root jest w pierwszym dziecko starego roota.
            if(!root->isLeaf){
                root = root->childs.front();
            }else
                root = nullptr;
                //no chyba ze usunieto wszystko wiec wtedy :)

        }
    }

    base_node *getSuccessorKey(K key){
        //przypadek graniczny tutaj trzeba obsluzyc, gdy succesor jest w innym nodzie. 14,14
        Node *node = root->getNodeWithKey(key);
        int id = node->getIndexOfKey(key).first;
        if(!node->isLeaf){
            return node->getSmallestInSubTree(id+1);
        } else if(node->isLeaf && id < (node->keysSize-1) )//kS 19-1 == 18
            return node->keys[id+1];
        else
            return nullptr;
    }
    base_node *getPredicatorKey(K key){
        Node *node = root->getNodeWithKey(key);
        int id = node->getIndexOfKey(key).first;
        if(!node->isLeaf){
            return node->getBiggestInSubTree(id-1);
        } else if(node->isLeaf && id>0 && id<=node->keysSize )//kS 19-1 == 18
            return node->keys[id-1];
        else
            return nullptr;
    }
};


#endif // BTREE_H
