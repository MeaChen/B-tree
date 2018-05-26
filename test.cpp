#include "B+Tree.h"
#include "DB.h"
#include <fstream>
#include <ctime>
#include <stdlib.h>

int main(){	
	string datafile = "df.dat", indexfile = "inf.dat";
	string deletedfile = "deleted.dat", replacedfile = "replaced.dat";

	DB datt(datafile, indexfile,deletedfile,replacedfile);
	
	datt.db_test();  //test the program
}
