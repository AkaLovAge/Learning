#include "stdio.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

// This is a fake class to represent the graph in the page
struct Graphic{
	int x;
	int y;
};

class SolverMemento{
 public:
	~SolverMemento(){}

 private:
	friend class ConstraintSolver;
	SolverMemento(){}
};

class ConstraintSolver{
 public:
	static ConstraintSolver* GetInstance(){
		static ConstraintSolver* solver = new ConstraintSolver();
		return solver;
	}

	void Solve(){
		std::cout << "Will solve the constraint between two targets." <<std::endl;
	}
	void AddConstraint(Graphic* start, Graphic* end){
		std::cout << "Add a constraint between two target start and end"<<std::endl;
	}
	void RemoveConstraint(Graphic* start, Graphic* end){
		std::cout << "Remove the constraint between the targets" << std::endl;
	}
	SolverMemento* CreateMeme(){
		return new SolverMemento();
	}
	void SetMeme(SolverMemento* meme){
		std::cout << "Change inner state based on meme" << std::endl;
	};

 private:
	ConstraintSolver(){}
};

class MoveCommand {
 public:
	~MoveCommand(){
		free(state_);
	}
	MoveCommand(Graphic* target, const int delta): target_(target), delta_(delta){};
	void Execute(){
		ConstraintSolver* solver = ConstraintSolver::GetInstance();
		state_ = solver->CreateMeme();
	  std::cout << "This is the step to do target->move(delta)" << std::endl;
		solver->Solve(); //Solve the process
	}
	void Undo(){
		ConstraintSolver* solver = ConstraintSolver::GetInstance();
		std::cout << "This is the step to do target->undo(delta)" << std::endl;
		solver->SetMeme(state_);
		solver->Solve();
	}

 private:
	SolverMemento* state_;
	Graphic* target_;
	int delta_;	
};

int main(int argc, char** argv){
	Graphic g;
	MoveCommand command(&g, 10);
	command.Execute();
	command.Undo();
	return 1;
}
