#include<iostream>
#include<vector>
#include<algorithm>
#include<sstream>
#include<string>
#include<memory>
#include<assert.h>

using namespace std;

class Test {
private:
    string str;
public:
    Test(string str):str(str){}
    Test(Test &t):str(t.str){}
    ~Test(){
        cout << str << " destructor\n";
    }
    void print() {
        cout << str << '\n';
    }
};

int main() {
    unique_ptr<Test> t1(new Test("t1"));
    unique_ptr<Test> t2(move(t1));
    //unique_ptr<Test> t4(t1);
    unique_ptr<Test> t5(new Test(*t1));
    if (int i; cin >> i) {
        switch(i){
            case 0:{
                unique_ptr<Test> t3(move(t1));
                t3->print();
                break;
            }
            case 1:{
                unique_ptr<Test> t3(move(t2));
                t3->print();
                break;
            }
            case 2:
            default:{
                t2->print();
                break;
            }
        }
    }
    t2->print();
    return 0;
}