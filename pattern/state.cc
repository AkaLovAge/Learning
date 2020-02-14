#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class TCPState;

class TCPConnection {
 public:
  TCPConnection(TCPState* connector);
  ~TCPConnection();
  void ActiveOpen();
  void PassiveOpen();
  void Close();
  void Send();
  void Ack();
  void Sync();
 private:
  TCPState* connector_;
  void UpdateState(TCPState* new_connector);
  friend class TCPState;
  friend class EstablishTCP;
  friend class ListenTCP;
  friend class CloseTCP;
  friend class SyncTCP;
};


class TCPState {
 public:
	virtual ~TCPState(){}

	virtual void ActiveOpen(TCPConnection* connection){std::cout << "This connection does not support active open."<< std::endl;};
	virtual void PassiveOpen(TCPConnection* connection){std::cout << "This connection does not support pasive open."<< std::endl;};
	virtual void Close(TCPConnection* connection){std::cout << "This connection does not support close."<< std::endl;};
	virtual void Send(TCPConnection* connection){std::cout << "This connection does not support send."<< std::endl;};
	virtual void Ack(TCPConnection* connection){std::cout << "This connection does not support ack."<< std::endl;};
	virtual void Sync(TCPConnection* connection){std::cout << "This connection does not support sync."<< std::endl;};
 protected:
	TCPState(){};
};

class EstablishTCP: public TCPState {
 public:
	EstablishTCP();
	~EstablishTCP();
	void ActiveOpen(TCPConnection* connection) override;
};

class ListenTCP: public TCPState {
 public:       
  ListenTCP() {};
  ~ListenTCP() {};
               
  void Send(TCPConnection* connection) override {
    std::cout<<"ListenTCP send" << std::endl;
    connection->UpdateState(this); 
  }            
};
class CloseTCP: public TCPState {
 public:       
  CloseTCP() {};
  ~CloseTCP() {};
               
  void Close(TCPConnection* connection) override {
    std::cout<<"CloseTCP close" << std::endl;
    connection->UpdateState(this); 
  }            
	void ActiveOpen(TCPConnection* connection) override {
		std::cout << "CloseTCP active open" << std::endl;
		connection->UpdateState(new EstablishTCP());
	}
};

EstablishTCP::EstablishTCP() {};
EstablishTCP::~EstablishTCP() {};

void EstablishTCP::ActiveOpen(TCPConnection* connection) {
  std::cout<<"EstablishTCP active open" << std::endl;
  connection->UpdateState(new ListenTCP());
}

TCPConnection::TCPConnection(TCPState* connector): connector_(connector) {}
TCPConnection::~TCPConnection(){
  free(connector_);
}
void TCPConnection::ActiveOpen(){
  connector_->ActiveOpen(this);
}
void TCPConnection::PassiveOpen(){
  connector_->PassiveOpen(this);
}
void TCPConnection::Close(){
  connector_->Close(this);
}
void TCPConnection::Send(){
 connector_->Send(this);
}
void TCPConnection::Ack(){
  connector_->Ack(this);
}
void TCPConnection::Sync(){
  connector_->Sync(this);
}
void TCPConnection::UpdateState(TCPState* new_connector){
  if (new_connector != connector_ && connector_ != nullptr){
    free(connector_);
    connector_ = nullptr;
  }
  connector_ = new_connector;
}

int main(int argc, char** argv) {
	TCPConnection* connection = new TCPConnection(new EstablishTCP());
	connection->ActiveOpen();
	connection->Send();
	connection->Close();
	connection->ActiveOpen();
	free(connection);	
}
