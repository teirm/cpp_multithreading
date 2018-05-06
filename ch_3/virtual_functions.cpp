/*
 * Just a quick check on virtual functions
 */

#include <iostream>
#include <string>

class Animal {

public:
    Animal(std::string name) { this->name = name; }
    virtual ~Animal() {}
    virtual void setName(std::string s) { this->name = s; }
    virtual std::string getName() const { return this->name; }
    virtual void speak() { std::cout << "Generic Animal" << std::endl; } 
private:
    std::string name;
};

class Walrus : public Animal {
public:
    Walrus(std::string name) : Animal(name) { }
    ~Walrus() {}; 
    void speak() { std::cout << "WALRUSSSS!" << std::endl; }
};

class Seal : public Animal{
public:
    Seal(std::string name) : Animal(name) { }
    ~Seal() {}; 
    void speak() { std::cout << "Seal of approval" << std::endl; }
};


void getSound(Animal& animal) {
    animal.speak();
}

int main()
{
    Walrus w("Wally");
    Seal s("Seally");
    Animal u("Unknown");

    getSound(w);
    getSound(s);
    getSound(u);

    return 0;
}
