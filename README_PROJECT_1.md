# Mark-and-sweep Garbage Collection Simulator
## What is this?
This is a simulation of garbage collection, via mark-and-sweep. It was built using C++, using a generic templated container, driven by a text-based command interface.  
## Why this exists?
This demonstrates proficiency in different C++ and OOP concepts:  
- Inheritance  
Used to create different type of objects. The classes Word and Number both inherit from the abstract Entity base class, letting them be stored and processed uniformly.
- Abstract classes and Pure virtual functions  
Used to create pointers to derived class objects, containing a pure virtual function that forces the derived class to implement their own versions of the same function, so that runtime polymorphism could be used. 
- Templates  
Used to store generic types of objects inside the templated container class. It works for int, string, Entity* types without modification.
- Operator overloading  
Used to allow our container to use the operator[] to get access to the contained objects by id instead of a getter function (much cleaner and less error prone).
- Custom iterators  
Used to allow iteration through our container. It supports range based `for` via `begin()` and `end()` methods.  
- dynamic_cast
Used for a real purpose (type discovery during serialization)
- File persistence  
Used to store the data inside of a text file, then reading from that same file through a type-tag line format so polymorphic objects can be reconstructed as the correct derived type on load.
- Reachability/mark-and-sweep algorithm  
Traverses the container starting from root, tracking visited/reachable ids in a `set`/`vector`, external to the objects themselves, to correctly handle cycles.  
## Usage Example

**Input:**
```text
7
NEW N 1 80
NEW N 2 99
NEW W 3 mark-and-sweep
SETROOT 1
SETNEXT 1 2
GC
STATUS
```

**Output:**
```text
Created object 1
Created object 2
Created object 3
Root now points to 1
Object 1 now points to 2
Collected: 3
Number (id=1): 80, next = 2
Number (id=2): 99, next = none
```
## Design decisions with reasoning
 - Why does next stores an `int` (id) instead of a raw `Entity*` pointer?
 Storing an int id instead means "does this still exist?" is answerable with container.exists(id), rather than trying to determine whether a raw pointer still points at something valid, which C++ gives us no built-in way to check.  
 - Why set<int> visited in findReachable, instead of just walking the chain with no tracking?  
 Because if there was no marking anything while visited each object, then if there was a chain that recycles, essentially a loop chain, then we would have an infinite loop. The choice of a set rather than another container was because checking whether an id has already been checked needs to be fast, and a set's .count()  check is efficient for that kind of membership test compared to other containers.  
 - Why does collectAndErase build a separate toDelete vector instead of erasing directly while iterating over the container?  
 Deleting an item while iterating automatically invalidates the iterator, resulting in undefined behavior, so we iterate through the container using a replica of the ids which is independent from the container itself. Erasing from the map has no effect on the vector of unreachable ids.  
 - Why does the file format include a tag `N`/`W`?  
 When creating an object via input, we usually specify which type of object that we are creating with 'N' for Number and 'W' for word. In order to recreate that input, we need to include the same input format, as if we were inputting data live. Once an object is written as plain text in a file, all type information is gone — a line like 1 45 -1 by itself doesn't tell you whether that was a Number or something else entirely. The tag is what restores the type information that would otherwise be lost the moment you flatten a polymorphic object down to text.  
 - Why does `get()` just do `return objects[id];` with no existence check inside it?  
There is no single "not found" value that works for every possible T. Any attempt to hardcode one only compiles for some instantiations and fails to compile for others, which is exactly the bug. The actual solution moves the responsibility to the caller. get() assumes that the object exists and returns objects[id] directly. The caller is expected to call exists(id) first. `map::operator[]` uses the same logic.  
- Why doesn't `clear()` handle memory cleanup (deleting the objects) itself, requiring the separate `freeAndClear` helper instead?  
freeAndClear first loops through and calls delete pair.second; on every entity, freeing the actual memory each pointer points to, then calls container.clear() afterwards to erase all the now-dangling pointers from the map. If the order were reversed, we would lose our only reference to those pointers, they would be gone from the map before ever getting the chance to delete them, which would be a memory leak.  
## What is deliberately left out?
- Memory is only freed via the explicit FreeAndClear path — not automatically on every clear()/reassignment inside Container<T> itself, since that would break genericity.  
- No malformed-input handling in loadFromFile (a corrupted or hand-edited world.txt isn't validated before parsing).  
- No alternative garbage collection strategy implemented (e.g., reference counting) for comparison — mark-and-sweep only.  
- No GUI — deliberately a CLI/text interface, consistent with real systems tools.  
