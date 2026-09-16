#include <iostream>
using namespace std;

class MiniContainer {
public:
    int arr[5] = {1, 2, 3, 4, 5};
    class Iterator {
    public:
        int* ptr;
        Iterator(int* ptr) : ptr(ptr) {}
        int& operator*() {}
        void operator++(){}
        bool operator!=(){}
    };
    auto begin() { Iterator iterator(&arr[0]); }
    auto end() {Iterator iterator(&arr[5]); }
};

int main() {


    return 0;
}