#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <map>

using namespace std;

string taa[1000001];

class DB{
public:
	DB(string filename1, string filename2, string filename3, string filename4){
		datafile = filename1;
		indexfile = filename2;
		deletedfile = filename3;
		replacedfile = filename4;
		records = "record.log";   //be used to record if the operation has been finished
		ofstream ofsf(records.c_str(),ios::binary);
		ofsf<<"end"<<endl; //normal statement
		srand((unsigned)time(NULL));
		//for(int i = 0; i < 1000001; ++i)
		//	taa[i] = ""; 
		int number = 0;
		for (char a = 'a'; a <= 'z'; ++a){
			alpha[number] = a;
			++number;
		}
		for (char c = 'A'; c <= 'Z'; ++c){
			alpha[number] = c;
			++number;
		}
		for (char m = '0'; m <= '9'; ++m){
			alpha[number] = m;
			++number;
		}		
		linenumber = 0;		
		total.clear();
	}

	void db_generate(){  //generate a datafile by random
		ofstream ofs(datafile.c_str(), ios::out | ios::binary);
		while (total.size() < 10){
			string s1 = randomstring(5);
			if (total.count(s1) == 0){				
				string s2 = randomstring(20);
				total[s1] = s2;
				ofs << s1 << " " << s2 << endl;
			}
		}
	}
	void db_open(){    //read the data from datafile into cache
		db_generate();
		ifstream ifs(datafile.c_str(), ios::in | ios::binary);
		string s1, s2;
		while (ifs >> s1 >> s2){			
			++linenumber;
			taa[linenumber] = s1;
			btree.insert(s1, s2);
		}
		btree.output(indexfile);
	}

	void db_trecord(){  // to ensure consistency
		ifstream dds(records.c_str(), ios::in | ios::binary);
		string rec;
		while (true){
			dds >> rec;
			if (dds.eof()){
				if (rec == "end")
					btree.output(indexfile);
				return;
			}
		}
	}
	string db_find(string s){ 
		string li = btree.find(s);
		return li;
	}

	void db_insert(string s1, string s2){  //the insert operation
		if(btree.find(s1) != ""){    //the element has existed
			return;
		}
		ofstream mm(datafile.c_str(), ios::app | ios::binary);
		mm << s1 << " " << s2 << endl;
		ofstream sd1(records.c_str(), ios::binary);
		++linenumber;
		sd1 << linenumber << " insert" << endl;
		taa[linenumber] = s1;
		btree.insert(s1, s2);	
		total[s1] = s2;	
		sd1 << "end" << endl;
		btree.output(indexfile);
	}
	
	void db_replace(string s1, string s2){ //the replace operation
		ofstream sd2(records.c_str(), ios::app | ios::binary);
		sd2 << " replace" << endl;
		btree.replace(s1, s2);		
		ofstream fsd(replacedfile.c_str(), ios::app | ios::binary);
		string a = total[s1];
		total.erase(s1);
		total[s2] = a;	
		sd2 << "end" << endl;	
		fsd << "replace " << s1 << " with " << s2 << endl;
		
		btree.output(indexfile);
	}

	void db_remove(string s){	//the remove operation		
		ofstream sd3(records.c_str(), ios::app | ios::binary);
		sd3 << " delete" << endl;
		btree.remove(s);
		total.erase(s);
		sd3 << "end" << endl;
		ofstream fsd(deletedfile.c_str(), ios::app | ios::binary);
		fsd << "delete " << s << endl;
		
		btree.output(indexfile);
	}
	
	void db_test(){
		db_open();			
		clock_t start1 = clock();
		//test the program
		for (int j = 0; j < 1000; ++j){
			for (int i = 0; i < 10; ++i){  
				db_trecord();
				if (i == 3 || i == 8){  //the replace operation
					int asd = rand() % linenumber;
					while (taa[asd] == "" || asd == 0)
						asd = rand() % linenumber;
					string gg = randomstring(20);
					ofstream u(records.c_str(), ios::binary);
					u << asd;
					db_replace(taa[asd], gg);
					continue;
				}
				if (i == 6){   //the remove operation
					int asd = rand() % linenumber;
					while (taa[asd] == "" || asd == 0)
						asd = rand() % linenumber;
					ofstream u(records.c_str(), ios::binary);
					u << asd;
					db_remove(taa[asd]);
					taa[asd] = "";
					continue;
				}
				string s1 = randomstring(5);
				string s2 = randomstring(20);				
				db_insert(s1, s2);
				
				if (i == 7){    //to test the correctness and the find operation
					if(total[s1] != db_find(s1)){					
						cout << "error!!" << endl;
						exit(2);
					}
				}
			}
			
		}	
		double elapsedTime1 = double(clock() - start1); //to get the time
		cout << elapsedTime1 << endl;
		
		/*
		for (int i = 0; i < 10000; ++i){
			string s1 = randomstring(5);
			string s2 = randomstring(20);
			db_insert(s1, s2);
			total[s1]=s2;
		}
		
		int asd = rand() % linenumber;
		clock_t start2 = clock();
		for(int i=0;i<10000;++i){
			while (taa[asd] == "" || asd == 0)
				asd = rand() % linenumber;					
			db_remove(taa[asd]);
			taa[asd] = " ";
		}
		double elapsedTime = double(clock() - start2);    
		cout << elapsedTime << endl;
					
		int asds = rand() % linenumber;
		while (taa[asds] == "" || asd == 0)
			asds = rand() % linenumber;
		string gg = randomstring(20);
		clock_t start3 = clock();
		db_replace(taa[asd], gg);
		double elapsedTime1 = double(clock() - start3);    
		cout << elapsedTime1 << endl;
		*/
	}

private:	

	map<string,string> total;	
	int linenumber;
	char alpha[62];
	BplusTree btree;
	string datafile;
	string indexfile;
	string deletedfile;
	string replacedfile;
	string records;
	string randomstring(int length){
		string key = "";
		for (int i = 0; i < length; ++i)
			key += alpha[rand() % 62];
		if (key != "")
			return key;
		else
			return randomstring(length);
	}
};
