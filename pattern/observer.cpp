#include "stdio.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;

class Observer{
 public:
	virtual ~Observer(){};
	virtual void Update() = 0;
 protected:
	Observer(){};
};

class Subject {
 public:
	virtual ~Subject(){};
	virtual void Register(Observer* observer){
		observers_.push_back(observer);
	};
	virtual bool Remove(Observer* observer) {
		bool found = false;
		for (int i=0; i < observers_.size(); i++){
			if (observers_[i] == observer){
				found = true;
				observers_.erase(observers_.begin() + i);
			}
		}
		return found;
	};
	virtual void Notify(){
		for (Observer* pt : observers_){
			pt->Update();
		}
	};
 protected:
	Subject(){};
 private:
	std::vector<Observer*> observers_;
};

class Timer : public Subject {
 public:
	Timer(): timestamp_(0){};
	int GetHour(){
		return timestamp_ / 3600;
	};
	int GetMinute(){
		return (timestamp_ % 3600) / 60;
	};
	int GetSecond() {
		return timestamp_ % 60;
	};
	
	void Tick(){
		timestamp_ ++;
		Subject::Notify();
	};
 private:
	int timestamp_;
};

class DigitClock : public Observer {
 public:
	DigitClock(Timer* timer){
		timer_ = timer;
		timer_->Register(this);
	};
	virtual ~DigitClock(){
		timer_->Remove(this);
	};

	void Update() override {
		int hour = timer_->GetHour();
		int minute = timer_->GetMinute();
		int second = timer_->GetSecond();
		std::cout << "Hour: " << hour << " Minute: " << minute << " Second: " << second << std::endl;
	}
 private:
	Timer* timer_;
};
class AnalogClock : public Observer{
 public:
	AnalogClock(Timer* timer){
		timer_ = timer;
		timer_->Register(this);
	};
	virtual ~AnalogClock(){
		timer_->Remove(this);
	};
	void Update() override {
		int hour = timer_->GetHour();
		int minute = timer_->GetMinute();
		int second = timer_->GetSecond();
		int angle = hour * 30;
		std::cout << "Angle: " << angle << std::endl;
	}
 private:
	Timer* timer_;
};
int main(int argc, char** argv) {
	Timer* timer = new Timer();
	DigitClock* dclock = new DigitClock(timer);
	AnalogClock* aclock = new AnalogClock(timer);
	timer->Tick();
	free(dclock);
	free(aclock);
	free(timer);
	return 0;
}


