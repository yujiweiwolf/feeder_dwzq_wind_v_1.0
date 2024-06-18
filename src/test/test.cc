#include <iostream>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include <regex>


using namespace std;


void run() {
    string code = "SRa& b";
    int pos = code.find_first_of("& ");
    cout << "code = [" << code << "], pos = " << pos << endl;
}


int main(int argc, char* argv[]) {
    run();
    cout << "press any key to quit ......" << endl;
    getchar();
    return 0;
}