#include "stdio.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include<unordered_map> 

using namespace std;



class Mouse;
class Monitor;
class Computer;

class ElementVisitor {
 public:
	virtual ~ElementVisitor(){};

  virtual void VisitMouse(Mouse*) =0;
	virtual void VisitMonitor(Monitor*)=0;
	virtual void VisitComputer(Computer*)=0;
 protected:
	ElementVisitor(){};
};

class Element {
 public:
	virtual ~Element(){};
	const string MyName() {return name_;};
	virtual void Accept(ElementVisitor* visitor) = 0;

	virtual void Power() {std::cout << "This device " << name_ <<" does not have Power!" << std::endl;};
	virtual void NetPrice() {std::cout << "This device " << name_ << " is free!" << std::endl;};
	virtual void Discount() {std::cout << "This device " << name_ << " does not have discount " <<std::endl;};
	virtual void Move() {std::cout << "This device " << name_ << " can not be moved!" << std::endl;};
 
 protected:
	Element(const string& name): name_(name){};
 private:
	std::string name_;
};

class Mouse : public Element {
 public:
	Mouse(std::string name, int power) : Element(name), power_(power) {}
	~Mouse(){}

  void Power() override {
		std::cout << "Mouse Power: " << power_ << std::endl;
	}
	
	void NetPrice() override {
		std::cout << "Mouse net price is 19$" << std::endl;
	}

	void Move() override {
		std::cout << "Mouse move 1, use 1 power" << std::endl;
		power_--;
		Power();
	}

  void Accept(ElementVisitor* visitor) override {
		visitor->VisitMouse(this);
	}
 private:
	int power_;
};

class Monitor: public Element {
 public:
	Monitor(std::string name) : Element(name){}
	~Monitor(){}

	void NetPrice() override {
		std::cout <<"This Monitor is 100$" << std::endl;
	}

	void Discount() override {
		std::cout << "This Monitor has discount 20$"<<std::endl;
	}

	void Accept(ElementVisitor* visitor) override{
		visitor->VisitMonitor(this);
	}
};

class Computer: public Element {
 public:
  Computer(std::string name, std::unordered_map<string, string> device_names): Element(name), power_(100){
		for (auto kv : device_names) {
			if (kv.first == "Mouse"){
				devices_.push_back(new Mouse(kv.second, 10));
			}else if (kv.first == "Monitor"){
				devices_.push_back(new Monitor(kv.second));
			}else{
				std::cout << "No suching device!" << std::endl;
			} 
		}
	}
	~Computer(){
		for (Element* x : devices_){
			free(x);
		}
	};

	void Accept(ElementVisitor* visitor) override {
		visitor->VisitComputer(this);
	}
  void Power() override{
		std::cout << "Computer has Power: " << power_;
		for (auto device : devices_){
			device->Power();
		}
	}

	void NetPrice() override{
		std::cout << "This computer is 500$" << std::endl;
		for (auto device : devices_){
			device->NetPrice();
		}
	}

	void Discount() override{
		Element::Discount();
		for (auto d: devices_) {
			d->Discount();
		}
	}
	
	void Move() override {
		Element::Move();
		for (auto d: devices_) {
			d->Move();
		}
	}

 private:
	std::vector<Element*> devices_;
	int power_;
};

class ComputerVisitor: public ElementVisitor{
 public:
	ComputerVisitor(){};
	~ComputerVisitor(){};
	void VisitMouse(Mouse* mouse) override {
		for (int i= 0; i < 10; i++){
			mouse->Move();
		}
		std::cout << "Mouse price: " << std::endl;
		mouse->NetPrice();
	};
	void VisitMonitor(Monitor* monitor) override {
		monitor->Discount();
	};
	void VisitComputer(Computer* computer) override {
		computer->Power();
		computer->NetPrice();
		computer->Move();
		computer->Discount();
	};
};
int main(int argc, char** argv){
	ComputerVisitor visitor;
	Mouse mouse("mouse1", 10);
	Monitor monitor("Monitoer1");
	Computer computer("computer1", {{"Mouse", "mouse2"}, {"Monitor", "monitor2"}, {"keyboard", "keyboard"}});

	mouse.Accept(&visitor);
	monitor.Accept(&visitor);
	computer.Accept(&visitor);
	return 0;
}
