#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Context {
	public:
		Context() = default;
		virtual ~Context();
		virtual LookUp(const char*) const;
		virtual Assign(BoolExpInterface*, bool);
};

class BoolExpInterface {
	public:
		virtual ~BoolExpInterface();
		
		virtual bool Eval() = 0;
		virtual BoolExpInterface* Replace(const char*, BoolExpInterface&) = 0;
		virtual BoolExpInterface* Copy() const = 0;
	protected:
		BoolExpInterface() = default;
};

class VariableBool : public BoolExpInterface {
	public:
		VariableBool(const string& name):name_(name){};
		virtual ~VariableBool();
	protected:
		string name_;
};

int main(int argc, char **argv) {
	string bool_str = "(true and X) or (Y and (not X))";
	BoolExpInterface* x = new VariableBool("X");
	BoolExpINterface* y = new VariableBool("Y");

}
