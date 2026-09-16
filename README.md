# Documentation of generic Garbage Collector

## include
`#include <iostream>`For the input and output stream.  
`#include <set>` To be able to quickly seach an item within the container.  
`#include <map>`To store the objects inside of the Container objects.  
`#include <vector>`To be able to dynamically add id's then delete then, giving our generic container a method to delete objects created by id.  
`#include <fstream>`To give our program the faculty to save and load objects from a file that create.  
## Classes  
### Entity 
This is an abstract class from which two derived classes are going to be created.  
- State    
`id` to identify the created objects since they are going to be stored by id inside of a container and `next`, the id of the object that it points to.   
- Methods   
`virtual string describe() const = 0;`Pure virtual function forcing the derived classes to implement their own versions since it is only an abstract class.  
`void setNext(int id) { next = id; }`Function taking an integer parameter and setting next to it.  
`void clearNext() { next = -1;}` Function setting next to -1, therefore making the object point to nothing.   
`virtual ~Entiyty() {}`a virtual destroyer that destroys the object.  
### Number
This is a derived class from entity.  
- state  
`int value;` Stores an integer.  
- Methods  
Number(int i, int v) : value(v) { next = i; } : Constructor taking two integers i and v, and setting value to v and next to i(id).  
`string describe() const override {}` : function overriding the pure virtual function inside the base class, implementing its own version, allowing runtime polymorphism. The program chooses which version of the function to use at runtime. It returns a string containing the id and the value converted to strings using to_string().  

### Word
This is also a derived class from Entity  
- state  
`string text` : string type variable containing a word;  
- Methods  
`Word(int i, string t)`{} : constructor instantiating text and id;  
`string describe() const override` : function return a string that descibes the type of the derived class.

### Container
This is a template class designed to contain generic objects (as pointers) and the root of all objects.  
- state  
`map<int, T> objects` a map to contain all created and inserted objects and arrange them by id.  
`int root` To store the id of the first object of the object chain.  
- Methods  
`void add(int id, T value) {}` : Function that takes id and a generic value as parameters. It searches the map to see if an object already exist at that id. If the object exists, then it outputs a messages saying that there is alrready an object existing at this index, else it inserts the object at the id index and output a message saying: "Created object `id`"  
`bool exists(int id) {}` : Function that tells whether an object already exist in the map by returning a bool.  
`T get(int id) {}` : Function that lets you get an object by its id;  
`int size() {}` : Function that returns the size of the map, essentially the size of our generic container.  
`T& operator[] {}` : Function that lets us use the operator[] to actually get an object by id. The reason for this is that instead of using the earlier version of a getter like container.get(id), we just use container[id] which is cleaner and more standard like a map.  
`auto begin() {}` : Function that return an index pointing to the start of our container.  
`auto end() {}` : Function that returns an index pointing to the end of our container.  
`void clear() {}` : Function that store all the ids to be deleted inside a vector because iterating inside a map while deleting an element invalidates the iterator pointing to that element which leads to undefined behavior. After storing all of those ids inside a vector, we iterate through the vector storing those ids and delete the elements since we're using the ids inside the vector to iterate through the map, therefore not tempering with the ids in the map itself.  
`void setRoot(int id) {}` : Function that sets the id of an objects as the root.  
`clear root() {}` : Function that clears the root, setting it to -1.  
`bool empty() {}` : Function that return a bool that says whether our container is empty or not.  
## Separate functions
### Vector<int> findReachable(Container<Entity*>& container, int root) { }  
This Function creates a vector to store all the reachable objects. It proceeds to create a `set<int> visited` so that when iterating throught our container, if something is visited, then it doesn't need to be visited again. Now, we have loop that iterated though our container starting at the root, which is stored inside of current depending on 3 conditions:  
- `current != -1` : If current == -1, that means that id does not exist inside the container.    
- `container.exists(current)` : Ensures that There is an object at that id.  
- `!visited.count(current)` : Ensures that while iterating through the container, we do not visit something twice, and do not store an id in the result vector twice.  
Inside the loop, if those 3 conditions are respected, we insert the id inside of the set to let the loop know the third condition is no more respected for that specific id. Secondly, we insert the id inside of the vector and lastly, we set `current` to the `next id` if there is.  
So this function essentially travels through the container like we traverse in a linked list, and gathers the id of every object that is reachable from the root and returns the list of all those ids.  
### vector<int> collectAndErase(Container<Entity*>& container, int root) { }   
This function uses our previous findReachable() function to store the reachable ids in a `vector<int> reachableList` and then creates a `set<int> reachableSet` and initializes it with all the ids in reachableList. Proceeds to create a `vector<int> toDelete` and then iterates through our container and if the ids are not reachable, they are pushed inside the toDelete vector. Then we it iterates through the container with the ids stored inside toDelete and deletes all the unreachable objects and then returns the list of the objects deleted.  
### saveToFile(Container<Entity*>& container, string fiilename) { }
This function creates an `oftream out` and names it with `filename`.
Then it iterates through the container with range for loop.
It creates a Entity object pointer to store either a `Word` or a `Number` object. The to distinguish whether the object be a Word or Number object, it establishes two conditions:
- `Number* n = dynamic_cast<Number*>(e)` : This is dynamic cast. If it succeeds then the object is a Number type object.
- `Word* w = dynamic_cast<Number*>(e)` : If this conversion succeeds, then the object is a Word type object.  
In either case, it stores a string inside of the output stream out formatted as such: N/W id value/text next. Then closes the output stream out.  
### void freeAndClear(Container<Entity*>& container) { }  
This is a function that iterates through the container and delete the objects stored at each id, and then uses the method `container.clear()` to delete the ids also. Deleting the objects at the index and taking them out of the container is not the same thing.  
### void loadFromFile(Container<Entity*>& container, string fileName) { }
This function creates a second container to store the first containers data before clearing it and resetting it again to show how we can create objects from stored data in an output stream also.  
The function creates an input stream in using fileName. creates a tag to analyse the first token in the file.   
In the loop, a string input is taken from the file. This token could be `N` or `W` based on the formatting earlier in out.  
Declares int id and next;
- `N` : Declares int value, takes input id, value and next, then creates a Number* n and instantiates the this Number pointer with id, value and next then add it to the second Container.
- `W` : Declares string text and int nextVal, takes the input of id, text and next, creates a Word pointer w and instantiate it with id, text and next, then adds it to the second container.  
After the iteration, the first container is cleared then initialized with the values of the secondContainer.  
## In main
Main takes the input of an integer representing the number of commands that are going to be passed from the user. then proceed to loop through all those commands. These commands all do something different:  
- NEW  
This command creates an object based on the type. If the second token after tag is N, then it takes id and value as input, creates an Entity* with them and adds it to the container. Else, if the token is W, the it takes int id and  string text as input, creates an Entity* using them for instantiation, then adds it to the container.
- SETNEXT  
This command takes in 2 ids, then set object a's next to b, if both of those objects already exist inside our container, and then outputs "Object a now points to b". If the condition is not respected, then it outputs "Invalid operation".
- SETROOT  
This command takes an int id, and if the object at the id exists, then it sets it as the root and outputs "Root now points to id" else "Invalid operation".
- CLEARROOT  
This command clears the root using the container's method "clearRoot" and outputs "Root cleared".  
-REACHABLE  
This command outputs all the reachable ids by iterating through the vector<int> returned by the function findReachable(container, container.root). If the vector is empty, it output: "Reachable: none".  
- GC  
This command uses the function collectAndErase to erase all unreachable ids and then outputs: "Collected: unreachable_ids" and none if the vector was empty.  
- STATUS  
This command iterates through our container if it is not empty. Depending on whether the object is a Number or Word type, then it outputs the desciption along with what it points to, essentially outputs "object_type (id=x): value/text, next = next_id/none".  
- SAVE  
This command saves the data (the objects inside the container) inside of a file name "world.txt."  
- LOAD  
This command recreates the objects from the file data.


``


