#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <set>
using namespace std;

class Entity {
    public:
        int id;
        int next = -1;
        virtual string describe() const = 0;
        virtual ~Entity() {}
};

class Number : public Entity {
    public:
        int value;
        Number(int i, int v) : value(v) { id = i; }
        string describe() const override {
            return "Number (id=" + to_string(id) + "): " + to_string(value);
        }
};

class Word : public Entity {
    public:
        string text;
        Word(int i, string t) : text(t) { id = i; }
        string describe() const override {
            return "Word (id=" + to_string(id) + "): " + text;
        }
};


template <typename T>
class Container {
    public:
        map<int, T> objects;

        void add(int id, T value) {
            if (!objects.count(id)){
                objects[id] = value;
                cout << "Created object " << id << endl;
            } else {
                cout << "Object " << id << " already exists" << endl;
            }
        }

        bool exists(int id) {
            return objects.count(id) > 0;
        }

        T get(int id) {
            return objects[id];
        }

        int size() {
            return objects.size();
        }

        T& operator[](int id) {
            return objects[id];
        }

        auto begin() { return objects.begin(); }
        auto end() { return objects.end(); }

        void clear(){
            vector<int>toDelete;
            for (auto& pair : objects){
                toDelete.push_back(pair.first);
            }

            for (int id : toDelete) {
                objects.erase(id);
            }
        }
};

vector<int> findReachable(Container<Entity*>& container, int root) {
    vector<int> result;
    set<int> visited;
    int current = root;
    while (current != -1 && container.exists(current) && !visited.count(current)){
        visited.insert(current);
        result.push_back(current);
        current = container[current]->next;
    }
    return result;
}

vector<int> collectAndErase(Container<Entity*>& container, int root){
    vector<int> reachableList = findReachable(container, root);
    set<int> reachableSet(reachableList.begin(), reachableList.end());

    vector<int>toDelete;
    for (auto& pair : container){
        if (!reachableSet.count(pair.first)) {
            toDelete.push_back(pair.first);
        }
    }

    for (int id : toDelete){
        container.objects.erase(id);
    }

    return toDelete;
}

void saveToFile(Container<Entity*>& container, string fileName) {
    ofstream out(fileName);
    for (auto& pair : container) {
        Entity* e = pair.second;
        if(Number* n = dynamic_cast<Number*>(e)){
            out << "N " << n->id << " " << n->value << " " << n->next << endl;
        } else if (Word* w = dynamic_cast<Word*>(e)){
            out << "W " << w->id << " " << w->text << " " << w->next << endl;
        }
    } 
    out.close();
}

void FreeAndClear(Container<Entity*>& container) {
    for (auto& pair : container) {
        delete pair.second;
    }

    container.clear();
}

void loadFromFile(Container<Entity*>& container, string fileName) {
    Container<Entity*> secondContainer;
    ifstream in(fileName);
    string tag;
    while (in >> tag) {
        int id, next;
        if (tag == "N"){
            int value;
            in >> id >> value >> next;
            Number* n = new Number(id, value);
            n->next = next;
            secondContainer.add(id, n);
        } else if (tag == "W") {
            string text;
            int nextval;
            in >> id >> text >> nextval;
            Word* w = new Word(id, text);
            w->next = nextval;
            secondContainer.add(id, w);
        }
    }
    FreeAndClear(container);
    container = secondContainer;
}

//------------------------------------main---------------------------------------//

int main() {

    return 0;
}