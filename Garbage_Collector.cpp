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
        void setNext(int id) { next = id; }
        void clearNext() { next = -1; }
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
        int root = -1;

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
            //why not just delete them without creating the vector?
            vector<int>toDelete;
            for (auto& pair : objects){
                toDelete.push_back(pair.first);
            }

            for (int id : toDelete) {
                objects.erase(id);
            }
        }

        void setRoot(int id) { root = id; }
        void clearRoot() { root = -1; }

        bool empty() {
            return objects.empty();
        }
};

vector<int> findReachable(Container<Entity*>& container, int root) {
    vector<int> result;
    set<int> visited;//why use a set? because you can just reach a member via .count(id);
    int current = root;
    while (current != -1 && container.exists(current) && !visited.count(current)){
        visited.insert(current);
        result.push_back(current);
        current = container[current]->next; //what if next == -1? no problem because of the condition current != -1;
    }
    return result;
}

vector<int> collectAndErase(Container<Entity*>& container, int root){
    vector<int> reachableList = findReachable(container, root);
    set<int> reachableSet(reachableList.begin(), reachableList.end());//what is this initialization?

    vector<int> toDelete;
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
    cout << "Number of ommands:" << endl;
    int q;
    cin >> q;

    Container<Entity*> container;
    for (int i = 0; i < q; i++) {
        string token;
        cin >> token;
        if (token == "NEW") {
            string tag;
            cin >> tag;
            if (tag == "N") {
                int id, value;
                cin >> id >> value;
                Entity* n = new Number(id, value);
                container.add(id, n);
            } else if (tag == "W") {
                int id;
                string text;
                cin >> id >> text;
                Entity* w = new Word(id, text);
                container.add(id, w);
            }
        } else if (token == "SETNEXT") {
            int a, b;
            cin >> a >> b;
            if (container.exists(a) && container.exists(b)) {
                container[a]->setNext(b);
                cout << "Object " << a << " now points to " << b << endl;
            } else {
                cout << "Invalid operation" << endl;
            }
        } else if (token == "SETROOT"){
            int id;
            cin >> id;
            if (container.exists(id)) {
                container.setRoot(id);
                cout << "Root now points to " << id << endl;
            } else {
                cout << "Invalid operation" << endl;
            }
        } else if (token == "CLEARROOT") {
            container.clearRoot();
            cout << "Root cleared" << endl;
        } else if (token == "REACHABLE") {
            vector<int> reachables = findReachable(container, container.root);
            cout << "Reachable:";
            if (!reachables.empty()) {
                for (auto& id : reachables) {
                    cout << " " << id;
                }
            } else {
                cout << " none";
            }
            cout << endl;
        } else if (token == "GC") {
            vector<int> collected = collectAndErase(container, container.root);
            cout << "Collected:";
            if (!collected.empty()) {
                for (auto& id : collected) {
                    cout << " " << id;
                }
            } else {
                cout << " none";
            }
            cout << endl;
        } else if (token == "STATUS") {
            if (!container.empty()) {
                for(auto& pair : container) {
                    if (pair.second->next != -1) {
                        cout << pair.second->describe() << ", next = " << pair.second->next << endl;
                    } else {
                        cout << pair.second->describe() << ", next = none" << endl;
                    }
                }
            } else {
                cout << "None" << endl;
            }
        } else if (token == "SAVE") {
            saveToFile(container, "world.txt");
        } else if (token == "LOAD") {
            loadFromFile(container, "world.txt");
        }
    }

    return 0;
}