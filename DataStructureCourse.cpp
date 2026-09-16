#include <algorithm>

class Intlist {
    void add (int x);
    void insertAt (int i, int x);
    int get(int i) const;
    bool removeAt(int i);
    int size() const;
};
//selection sort
void selectionSort(int a[], int n) {
    for (int i = 0; i < n; i++) {
        int j = i;
        for (int k = i + 1; k < n; k++) {
            if (a[k] < a[j]) j = k;
        }
        std::swap(a[i], a[j]);
    }
}