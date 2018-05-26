#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

//# pragma comment(linker, "/STACK:102400000000000,102400000000000")

const int MAX_CNT = 5;   
const int MIN_CNT = 3;

class BplusTree{
private:
	struct Node{
		bool leaf;    
		string keys[MAX_CNT];   
		string cons[MAX_CNT];
		int keysize;     
		Node* child[MAX_CNT + 1];    
		Node* parent;      
		Node(){
			leaf = 0;
			keysize = 0;
			for (int i = 0; i < MAX_CNT; ++i)
				cons[i] = "";
			for (int i = 0; i < MAX_CNT + 1; ++i)
				child[i] = NULL;	
			parent = NULL;
		}
	};

	//find the position the key should be inserted
	Node* findpos(string s){
		Node *p = root;
		while (p->leaf == 0){
			if (s < p->keys[0]){   //the value is smaller than the smallest value in the tree
				p = p->child[0];
				if (p == NULL)
					return NULL;
				continue;
			}
			if (s >= p->keys[p->keysize - 1]){
				p = p->child[p->keysize];	
				if (p == NULL)
					return NULL;
				continue;
			}
			for (int i = 0; i < p->keysize - 1; ++i){
				if (s >= p->keys[i] && s < p->keys[i + 1]){
					p = p->child[i + 1];
					if (p == NULL)
						return NULL;
					break;
				}
			}
		}
		return p;
	}

	//split and bottom-up to the root
	void spilt2(Node *p, string s, string con, Node *child = NULL){
		Node *oo = p->parent;
		int num = 0;		
		for (int i = 0; i < MAX_CNT; ++i){
			if (s > p->keys[i])
				++num;
			else
				break;
		}
		Node *child1 = new Node();
		Node *child2 = new Node();
		
		child1->keysize = MIN_CNT;
		child1->parent = oo;
		child2->parent = oo;
		child2->keysize = MIN_CNT;
		if (p->leaf == 1){
			child1->leaf = 1;
			child2->leaf = 1;
		}
		else{
			child1->leaf = 0;
			child2->leaf = 0;
		}
		if (num > 2){
			for (int i = 0; i < MIN_CNT; ++i){
				child1->keys[i] = p->keys[i];
				child1->cons[i] = p->cons[i];
				child1->child[i] = p->child[i];
			}
			child1->child[MIN_CNT] = p->child[MIN_CNT];
			if (child != NULL){
				child2->child[num - MIN_CNT + 1] = child;
				child2->leaf = 0;
			}
			child2->child[0] = NULL;
			if (num == 3){
				child2->keys[0] = s; child2->cons[0] = con;
				child2->keys[1] = p->keys[3]; child2->cons[1] = p->cons[3]; child2->child[2] = p->child[4];
				child2->keys[2] = p->keys[4]; child2->cons[2] = p->cons[4]; child2->child[3] = p->child[5];
			}
			else if (num == 4){
				child2->keys[0] = p->keys[3]; child2->cons[0] = p->cons[3]; child2->child[1] = p->child[4];
				child2->keys[1] = s; child2->cons[1] = con;
				child2->keys[2] = p->keys[4]; child2->cons[2] = p->cons[4]; child2->child[3] = p->child[5];
			}
			else{
				child2->keys[0] = p->keys[3]; child2->cons[0] = p->cons[3]; child2->child[1] = p->child[4];
				child2->keys[1] = p->keys[4]; child2->cons[1] = p->cons[4]; child2->child[2] = p->child[5];
				child2->keys[2] = s; child2->cons[2] = con;
			}			
		}
		else{			
			for (int i = 0; i < MIN_CNT; ++i){
				child2->keys[i] = p->keys[MIN_CNT + i - 1];
				child2->cons[i] = p->cons[MIN_CNT + i - 1];
				child2->child[i + 1] = p->child[MIN_CNT + i];
			}
			if (child != NULL){
				child1->child[num + 1] = child;
				child1->leaf = 0;
			}
			
			if (num == 1){
				child1->keys[0] = p->keys[0]; child1->child[0] = p->child[0];
				child1->cons[0] = p->cons[0];
				child1->child[1] = p->child[1];
				child1->keys[1] = s; child1->cons[1] = con;
				child1->keys[2] = p->keys[1]; child1->cons[2] = p->cons[1]; child1->child[3] = p->child[2];
			}
			else if (num == 2){		
				child1->keys[0] = p->keys[0]; child1->cons[0] = p->cons[0]; child1->child[0] = p->child[0];
				child1->keys[1] = p->keys[1]; child1->cons[1] = p->cons[1]; child1->child[1] = p->child[1];
				child1->keys[2] = s; child1->cons[2] = con; child1->child[2] = p->child[2];
			}
			else if (num == 0){
				child1->keys[0] = s; child1->cons[0] = con;
				child1->keys[1] = p->keys[0]; child1->cons[1] = p->cons[0]; child1->child[2] = p->child[1];
				child1->keys[2] = p->keys[1]; child1->cons[2] = p->cons[1]; child1->child[3] = p->child[2];
			}			
		}
		for (int x = 0; x <= MIN_CNT; ++x){
			if (child1->child[x] != NULL)
				child1->child[x]->parent = child1;
		}
		for (int x = 1; x <= MIN_CNT; ++x){
			if (child2->child[x] != NULL)
				child2->child[x]->parent = child2;
		}
		//delete p;
		if (oo == NULL){  //generate a new root
			Node *nn = new Node();
			nn->keys[0] = child2->keys[0];
			nn->cons[0] = child2->cons[0];
			nn->child[0] = child1;
			nn->child[1] = child2;
			nn->keysize = 1;
			root = nn;
			child1->parent = root; child2->parent = root;
			return;
		}
		string m = p->keys[0];
		int mm = 0;		
		while (oo->keys[mm] != m){
			++mm;
			//the inserted element is the smallest
			if (mm == oo->keysize){
				if (oo->keysize < MAX_CNT){
					for (int i = oo->keysize - 1; i >= 0; --i){
						oo->keys[i + 1] = oo->keys[i];
						oo->cons[i + 1] = oo->cons[i];
						oo->child[i + 2] = oo->child[i + 1];
					}
					oo->keys[0] = child2->keys[0];
					oo->cons[0] = child2->cons[0];
					++oo->keysize;
					oo->leaf = 0;
					oo->child[0] = child1;
					oo->child[1] = child2;
				}
				else{
					string s1 = oo->keys[0];
					oo->child[0] = child1;
					oo->keys[0] = child2->keys[0];
					oo->cons[0] = child2->cons[0];
					Node *sjk = oo->child[1];
					oo->child[1] = child2;
					spilt2(oo, s1, oo->cons[0], sjk);
				}
				return;
			}
		}
		if (oo->keysize < MAX_CNT){
			for (int i = oo->keysize - 1; i > mm; --i){
				oo->keys[i + 1] = oo->keys[i];
				oo->cons[i + 1] = oo->cons[i];
				oo->child[i + 2] = oo->child[i + 1];
			}
			oo->keys[mm + 1] = child2->keys[0];
			oo->cons[mm + 1] = child2->cons[0];
			++oo->keysize;
			oo->leaf = 0;
			oo->child[mm + 1] = child1;
			oo->child[mm + 2] = child2;
		}
		else{
			oo->child[mm + 1] = child1;
			spilt2(oo, child2->keys[0], child2->cons[0], child2);
		}			
	}

	//change the first element bottom-to-up
	void changefirst(Node *p, string s, string t){
		Node *oo = p->parent;
		while (oo != NULL){
			int n = 0;
			while (s != oo->keys[n]){
				++n;
				if (n == oo->keysize)
					return;
			}
			oo->keys[n] = t;
			if (n != 0)
				return;
			oo = oo->parent;
		}
	}

	//when the node is the root
	void tt_ro(int m){
		if (root->keysize == 1){
			root = root->child[0];
			root->parent = NULL;
			return;
		}
		for (int i = m; i < root->keysize - 1; ++i){
			root->keys[i] = root->keys[i + 1];
			root->cons[i] = root->cons[i + 1];
			if (root->leaf == 0)
				root->child[i + 1] = root->child[i + 2];
		}
		root->keys[root->keysize - 1] = "";
		root->cons[root->keysize - 1] = "";
		root->child[root->keysize] = NULL;
		--root->keysize;		
	}

	//merge the two nodes
	void merge(Node *x, Node *y){
		for (int i = x->keysize; i < x->keysize + y->keysize; ++i){
			x->keys[i] = y->keys[i - x->keysize];
			x->cons[i] = y->cons[i - x->keysize];
			if (x->leaf == 0){
				x->child[i + 1] = y->child[i - x->keysize + 1];
				y->child[i - x->keysize + 1]->parent = x;
			}
		}
		x->keysize = x->keysize + y->keysize;
	}

	//when the parent node is invalid, traverse from bottom to up
	void bottom_up(Node *p, int m){
		if (p == root){
			tt_ro(m);
			return;
		}
		if (p->keys[0] < p->parent->keys[0]){
			t_remove(p, m);
			return;
		}
		if (p->keysize - 1 < MIN_CNT)
			borrow(p, m);
		else{
			if (m == 0)
				changefirst(p, p->keys[0], p->keys[1]);
			for (int i = m; i < p->keysize - 1; ++i){
				p->keys[i] = p->keys[i + 1];
				p->cons[i] = p->cons[i + 1];
				if (p->leaf == 0)
					p->child[i + 1] = p->child[i + 2];
			}
			p->keys[p->keysize - 1] = "";
			p->cons[p->keysize - 1] = "";
			p->child[p->keysize] = NULL;
			--p->keysize;			
		}
	}

	//remove when the string is in the smallest node 
	void t_remove(Node *p, int m){
		if (p == NULL)
			return;
		if (p == root){
			tt_ro(m);
			return;
		}
		for (int i = m; i < p->keysize - 1; ++i){
			p->keys[i] = p->keys[i + 1];
			p->cons[i] = p->cons[i + 1];
			if (p->leaf == 0)
				p->child[i + 1] = p->child[i + 2];
		}	
		p->keys[p->keysize - 1] = "";
		p->cons[p->keysize - 1] = "";
		p->child[p->keysize] = NULL;
		if (p->keysize - 1 >= MIN_CNT){
			--p->keysize;
			return;
		}
		else{
			Node *pp = p->parent;			
			Node *bro = pp->child[1];
			if (bro->keysize - 1 >= MIN_CNT){ 
				//move the first element of the brother and change...
				p->keys[p->keysize - 1] = bro->keys[0];
				p->cons[p->keysize - 1] = bro->cons[0];
				if (p->leaf == 0){
					p->child[p->keysize] = bro->child[1];
					bro->child[1]->parent = p;
					p->child[p->keysize]->parent = p;
				}
				changefirst(pp, pp->keys[0], bro->keys[1]);
				pp->keys[0] = bro->keys[1];
				pp->cons[0] = bro->cons[1];
				for (int i = 0; i < bro->keysize - 1; ++i){
					bro->keys[i] = bro->keys[i + 1];
					bro->cons[i] = bro->cons[i + 1];
					if (bro->leaf == 0)
						bro->child[i + 1] = bro->child[i + 2];
				}
				bro->keys[bro->keysize - 1] = "";
				bro->cons[bro->keysize - 1] = "";
				bro->child[bro->keysize] = NULL;
				--bro->keysize;
				return;
			}
			else{  
				//merge the two nodes and delete the element in the parent node
				--p->keysize;
				merge(p, bro);
				bottom_up(pp, 0);
			}
		}
	}

	//borrow backwards
	void borrow(Node *pos, int hh){		
		if (pos == NULL)			
			return;
		if (pos == root){
			tt_ro(hh);
			return;
		}
		Node *pp = pos->parent;
		//find the position of the index in the parent node
		string ss = pos->keys[0];
		int x = 0;
		while (ss != pp->keys[x]){
			++x;
			if (x == pp->keysize){  //error!!
				cerr << "error!!" << endl;
				exit(2);
			}
		}
		
		Node *brother = pp->child[x];
		if (brother == NULL){
			brother = pp->child[2];
			if (hh == 0){
				changefirst(pos, pos->keys[0], pos->keys[1]);
			}
			if (brother->keysize - 1 >= MIN_CNT){  //move the smallest element of the brother  
				//if (hh == 0)
				//	changefirst(pos, pos->keys[0], pos->keys[1]);
				for (int i = hh; i < pos->keysize - 1; ++i){
					pos->keys[i] = pos->keys[i + 1];
					pos->cons[i] = pos->cons[i + 1];
					if (pos->leaf == 0)
						pos->child[i + 1] = pos->child[i + 2];
				}
				pos->keys[pos->keysize - 1] = brother->keys[0];
				pos->cons[pos->keysize - 1] = brother->cons[0];
				if (pos->leaf == 0){
					pos->child[pos->keysize] = brother->child[1];
					pos->child[pos->keysize]->parent = pos;
					brother->child[1]->parent = pos;
				}
				for (int i = 0; i < brother->keysize - 1; ++i){
					brother->keys[i] = brother->keys[i + 1];
					brother->cons[i] = brother->cons[i + 1];
					if (brother->leaf == 0)
						brother->child[i + 1] = brother->child[i + 2];
				}
				pp->keys[1] = brother->keys[0];
				pp->cons[1] = brother->cons[0];
				brother->keys[brother->keysize - 1] = "";
				brother->cons[brother->keysize - 1] = "";
				brother->child[brother->keysize] = NULL;
				--brother->keysize;
			}
			else{   //merge the two nodes				
				for (int b = hh; b < pos->keysize - 1; ++b){
					pos->keys[b] = pos->keys[b + 1];
					pos->cons[b] = pos->cons[b + 1];
					if (pos->leaf == 0)
						pos->child[b + 1] = pos->child[b + 2];
				}				
				pos->keys[pos->keysize - 1] = "";
				pos->cons[pos->keysize - 1] = "";
				pos->child[pos->keysize] = NULL;
				--pos->keysize;
				merge(pos, brother);
				bottom_up(pp, 1);
			}
		}
		else if (brother->keysize - 1 >= MIN_CNT){  //move the largest element of the brother  
			for (int i = hh; i > 0; --i){
				pos->keys[i] = pos->keys[i - 1];
				pos->cons[i] = pos->cons[i - 1];
				if (pos->leaf == 0)
					pos->child[i + 1] = pos->child[i];
			}
			pos->keys[0] = brother->keys[brother->keysize - 1];
			pos->cons[0] = brother->cons[brother->keysize - 1];
			if (pos->leaf == 0){
				pos->child[1] = brother->child[brother->keysize];
				brother->child[brother->keysize]->parent = pos;
				pos->child[1]->parent = pos;
			}
			brother->keys[brother->keysize - 1] = "";
			brother->cons[brother->keysize - 1] = "";
			brother->child[brother->keysize] = NULL;
			pp->keys[x] = pos->keys[0];
			if (x == 0)
				changefirst(pp, pos->keys[1], pp->keys[0]);
			--brother->keysize;
		}
		else{   //merge the two nodes
			for (int i = hh; i < pos->keysize - 1; ++i){
				pos->keys[i] = pos->keys[i + 1];
				pos->cons[i] = pos->cons[i + 1];
				if (pos->leaf == 0)
					pos->child[i + 1] = pos->child[i + 2];
			}
			pos->keys[pos->keysize - 1] = "";
			pos->cons[pos->keysize - 1] = "";
			pos->child[pos->keysize] = NULL;
			--pos->keysize;
			merge(brother, pos);
			bottom_up(pp, x);
		}		
	}

	//put the element of the nodes into the queue in order
	void traverse1(Node *p){
		if (p == NULL)
			return;
		if (p->leaf == 1){
			for (int x = 0; x < p->keysize; ++x){
				nodesele.push(p->keys[x]);
				addr.push(p->cons[x]);
			}
			return;
		}
		//cout << p->keysize << endl;
		for (int i = 0; i <= p->keysize; ++i)
			traverse1(p->child[i]);
	}

	Node *root;

	queue<string>nodesele;
	queue<string>addr;

public:
	BplusTree(){
		root = new Node();
		//root = NULL;
	}
	~BplusTree(){
		delete root;
	}

	string find(string s){
		Node *pos = findpos(s);
		if (pos == NULL)
			return "";
		int n = 0;
		while (s != pos->keys[n]){
			++n;
			if (n == pos->keysize) //the element doesn't exist
				return "";
		}
		return pos->cons[n];
	}
	
	void replace(string s1, string s2){
		Node *pos = findpos(s1);
		int n = 0;
		while (s1 != pos->keys[n]){
			++n;
			if (n == pos->keysize) //the element doesn't exist
				return;
		}
		pos->cons[n] = s2;
	}

	void insert(string s, string line){
		if (root ->keysize == 0){
			root->keysize = 1;
			root->keys[0] = s;
			root->cons[0] = line;
			root->parent = NULL;
			root->leaf = true;
			return;
		}
		else{
			Node *pos = findpos(s);
			//insert the element in the array
			if (pos->keysize > 0 && pos->keysize < MAX_CNT){
				if (s < pos->keys[0]){
					for (int i = pos->keysize - 1; i >= 0; --i){
						pos->keys[i + 1] = pos->keys[i];
						pos->cons[i + 1] = pos->cons[i];
					}
					pos->keys[0] = s;
					pos->cons[0] = line;
					++pos->keysize;
					return;
				}
				if (s > pos->keys[pos->keysize - 1]){
					pos->keys[pos->keysize] = s;
					pos->cons[pos->keysize] = line;
					++pos->keysize;
					return;
				}				
				for (int i = 0; i < pos->keysize - 1; ++i){  //move elements
					if (s > pos->keys[i] && s < pos->keys[i + 1]){
						for (int k = pos->keysize - 1; k > i; --k){
							pos->keys[k + 1] = pos->keys[k];
							pos->cons[k + 1] = pos->cons[k];
						}
						pos->keys[i + 1] = s;
						pos->cons[i + 1] = line;
						++pos->keysize;
						return;
					}
				}
			}
			else if (pos->keysize == MAX_CNT){
				spilt2(pos, s, line);
			}				
		}
	}

	void remove(string s){
		if (root->keysize == 0) //when the tree is empty, return
			return;
		Node *pos = findpos(s);		
		int i = 0;
		while (s != pos->keys[i]){
			++i;
			if (i == pos->keysize)  //the element doesn't exist
				return;
		}
		if (pos == root){
			tt_ro(i);
			return;
		}
		if (s < pos->parent->keys[0]){
			t_remove(pos, i); //borrow forward
			return;
		}
		
		if (pos->keysize - 1 >= MIN_CNT){  
			if (i == 0)
				changefirst(pos, pos->keys[0], pos->keys[1]);
			for (int m = i; m < pos->keysize - 1; ++m){  //use the next element to replace
				pos->keys[m] = pos->keys[m + 1];
				pos->cons[m] = pos->cons[m + 1];
			}
			pos->keys[pos->keysize - 1] = "";
			pos->cons[pos->keysize - 1] = "";
			pos->child[pos->keysize] = NULL;
			--pos->keysize;
			
			return;
		}
		else{
			borrow(pos, i);  //borrow backward
		}	
	}
	
	void output(string filename){
		//clock_t start4 = clock();
		ofstream ffs(filename.c_str(), ios::out | ios::binary);
		traverse1(root);
		int tt = nodesele.size();
		for (int i = 0; i < tt; ++i){
			cout << nodesele.front() << " " << addr.front() << endl;
			nodesele.pop();
			addr.pop();
		}
		//cout << endl;
		//cout << "the total number is " << tt << endl;
		//double time = double(clock()-start4);
		//cout<<time<<endl;
	}
};
