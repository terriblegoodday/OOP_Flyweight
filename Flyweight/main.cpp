//
//  main.cpp
//  Flyweight
//
//  Created by Eduard Dzhumagaliev on 5/30/20.
//  Copyright © 2020 Eduard Dzhumagaliev. All rights reserved.
//

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

enum Engine {
    electric,
    combustion
};

struct SharedState {
    string brand;
    string model;
    string color;
    
    SharedState() {};
    SharedState(string brand, string model, string color): brand(brand), model(model), color(color) {};
    
    friend ostream & operator<<(ostream & destination, const SharedState & source) {
        destination << "[ " << source.brand << " , " << source.model << " , " << source.color << " ]";
        
        return destination;
    };
};

struct UniqueState {
    string owner;
    string plates;
    string tripComputer;
    bool hasAutopilot;
    Engine engine;
    vector<string> buffs;
    
    UniqueState() {};
    UniqueState(string owner, string plates): owner(owner), plates(plates) {};
    
    friend std::ostream & operator<<(std::ostream &os, const UniqueState &us)
    {
        os << "[ " << us.owner << " , " << us.plates << " ]";
        return os;
    }
};

class Flyweight {
private:
    SharedState * sharedState;
    
public:
    Flyweight(): sharedState(new SharedState) {};
    Flyweight(SharedState * sharedState): sharedState(new SharedState(*sharedState)) {};
    Flyweight(const Flyweight & other): sharedState(new SharedState(*other.sharedState)) {};
    
    Flyweight & operator=(const Flyweight & source) {
        if (this == &source) return *this;
        this->sharedState->brand = source.sharedState->brand;
        this->sharedState->color = source.sharedState->color;
        this->sharedState->model = source.sharedState->model;
        return *this;
    };
    
    ~Flyweight() {
        if (sharedState != nullptr) delete sharedState;
    }
    
    SharedState * getSharedState() {
        return sharedState;
    }
};

class FlyweightFactory {
private:
    unordered_map<string, Flyweight> flyweights;
    
    string hash(const SharedState & ss) {
        return ss.brand + " - " + ss.model + " - " + ss.color;
    };
    
public:
    FlyweightFactory(vector<SharedState> & sharedStates) {
        for (SharedState & sharedState: sharedStates) {
            this->flyweights[hash(sharedState)] = Flyweight(&sharedState);
        }
    }
    
    Flyweight * getFlyweight(SharedState sharedState) {
        string key = this->hash(sharedState);
        if (this->flyweights.find(key) == this->flyweights.end()) {
            cout << "FlyweightFactory: Can't find a flyweight, creating new one" << endl;
            this->flyweights[key] = Flyweight(&sharedState);
        } else {
            cout << "FlyweightFactory: Reusing existing flyweight" << endl;
        }
        return &this->flyweights.at(key);
    }
    
    void listFlyweights() {
        size_t count = this->flyweights.size();
        cout << endl << "FlyweightFactory size: " << count << endl;
        for_each(this->flyweights.begin(), this->flyweights.end(), [](auto element) {
            cout << element.first << endl;
        });
    }
};

class Car {
    friend class CombustionCarBuilder;
    friend class ElectroCarBuilder;
    
private:
    UniqueState uniqueState;
    Flyweight * flyweight;
    
public:
    Car(FlyweightFactory * flyweightFactory, string brand, string model, string color) {
        auto sharedState = SharedState(brand, model, color);
        flyweight = flyweightFactory->getFlyweight(sharedState);
    }
    
    friend ostream & operator<<(ostream & destination, const Car & source) {
        destination << source.uniqueState.owner << " " <<  source.uniqueState.plates << " " <<  source.flyweight->getSharedState()->brand << " " << source.flyweight->getSharedState()->model;
        return destination;
    }
};

class CarBuilder {
protected:
    FlyweightFactory * flyweightFactory;
    
    vector<string> buffs;
    Engine engine;
    string owner;
    string plates;
    string tripComputer;
    bool hasAutopilot;
    
    CarBuilder(FlyweightFactory * flyweightFactory): flyweightFactory(flyweightFactory) {};
    
public:
    void addBuff(string buff) {
        buffs.push_back(buff);
    }
    
    void setOwner(string owner) {
        this->owner = owner;
        auto sum = 0;
        for (char & character: owner) {
            sum += character;
        }
        this->plates = to_string(sum);
    }
    
    void setTripComputer(string tripComputer) {
        this->tripComputer = tripComputer;
    }
    
    void toggleAutopilot() {
        hasAutopilot = !hasAutopilot;
    }
    
    void reset() {
        buffs.clear();
        owner.clear();
        plates.clear();
        tripComputer.clear();
        hasAutopilot = false;
    }
    
    virtual Car * build() = 0;
};

class CombustionCarBuilder: public CarBuilder {
public:
    CombustionCarBuilder(FlyweightFactory * flyweightFactory): CarBuilder(flyweightFactory) {};
    
    Car * build() override {
        Car * car = new Car(flyweightFactory, "Toyota", "Land Cruiser Prado", "Red");
        car->uniqueState.engine = combustion;
        car->uniqueState.buffs = buffs;
        car->uniqueState.hasAutopilot = hasAutopilot;
        car->uniqueState.tripComputer = tripComputer;
        car->uniqueState.plates = plates;
        car->uniqueState.owner = owner;
        
        return car;
    };
};

class ElectroCarBuilder: public CarBuilder {
public:
    ElectroCarBuilder(FlyweightFactory * flyweightFactory): CarBuilder(flyweightFactory) {};
    
    Car * build() override {
        Car * car = new Car(flyweightFactory, "Tesla", "Model 3", "Black");
        car->uniqueState.engine = electric;
        car->uniqueState.buffs = buffs;
        car->uniqueState.hasAutopilot = hasAutopilot;
        car->uniqueState.tripComputer = tripComputer;
        car->uniqueState.plates = plates;
        car->uniqueState.owner = owner;
        
        return car;
    };
};


int main(int argc, const char * argv[]) {
    vector<SharedState> sharedStates;
    auto flyweightFactory = FlyweightFactory(sharedStates);
    auto combustionCarBuilder = CombustionCarBuilder(&flyweightFactory);
    auto electroCarBuilder = ElectroCarBuilder(&flyweightFactory);
    
    for (int i = 0; i < 6; i++) {
        combustionCarBuilder.addBuff("Buff " + to_string(i));
        combustionCarBuilder.toggleAutopilot();
        combustionCarBuilder.setOwner("Person " + to_string(i));
        combustionCarBuilder.setTripComputer("CarPlay");
        auto car = combustionCarBuilder.build();
        cout << *car << endl;
    };
    
    flyweightFactory.listFlyweights();
    
    for (int i = 0; i < 6; i++) {
        electroCarBuilder.addBuff("Buff " + to_string(i));
        electroCarBuilder.toggleAutopilot();
        electroCarBuilder.setOwner("Person " + to_string(i));
        electroCarBuilder.setTripComputer("Tesla");
        auto car = electroCarBuilder.build();
        cout << *car << endl;
    }
    
    flyweightFactory.listFlyweights();
    
    auto car = Car(&flyweightFactory, "Toyota", "Prius", "White");
    cout << car << endl;
    
    flyweightFactory.listFlyweights();
    
    return 0;
}
