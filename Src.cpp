#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Process{
	int pid; // process ID
	int startTime; // Arrival time of process
	int serviceTime; // Time needed for its execution
	int tremaining; // Time till it finishes execution - to faciliate the queues
	bool cs; // Whether it needs to enter the critical section or not - Dependent on Critical section or independent
public:
	int getpid(){
		return pid;
	}
	void setpid(int x){
		pid = x;
	}
	int getstartTime(){
		return startTime;
	}
	void setstartTime(int x){
		startTime = x;
	}
	int getserviceTime(){
		return serviceTime;
	}
	void setserviceTime(int x){
		serviceTime = x;
	}
	int getcs(){
		return cs;
	}

	/* Shortcut for setting all the attributes of the process */

	void setall(int PROCESSORID, int STARTTIME, int SERVICETIME, bool CRITICALSECTION){
		pid = PROCESSORID; startTime=STARTTIME; serviceTime=SERVICETIME; cs=CRITICALSECTION;
		tremaining = serviceTime;

	}

	int gettremaining(){
		return tremaining;
	}

	void settremaining(int x){
		tremaining=x;
	}
};

class Queue{
	vector <Process> p; // A queue can facilitate a vector of processes
	int NOP; // Number of processes currently in the Queue
public:

	/* Remove a processor from the beginning of the Queue */
	void popprocess(){
		Process x = p.front(); // get direct reference to the first process in the vector
		cout <<"\n --- Removing Process #"<<x.getpid()<<" from the Queue ---";
		p.erase(p.begin()); // Removing the first element (process) from the vector
		NOP = p.size(); // Update the number of processes count of the Queue
	}

	/* Returns the first process in the Queue */
	Process getprocess(){
		if (NOP!=0) // Execute only if the Queue is filled, else runtime errors
			return (p.front());
	}

	/* Add a process to the end of the queue */
	void pushprocess(Process x){
		p.push_back(x);
		NOP = p.size(); // update number of processes count of the Queue
		cout <<"\n +++ Adding Process #"<<x.getpid()<<" to the Queue (Service Time: "<<x.getserviceTime()<<"s) +++";
	}

	int getNOP(){
		return NOP;
	}

	/* To execute the first process in the Queue */
	void process(){
		// Check if the process still needs to be executed
		if (p.front().gettremaining() != 0 ){
				p.front().settremaining((p.front().gettremaining()-1));
				Process temp = p.front();
				cout <<"\n*** Process #"<<temp.getpid()<<" executed ("<<temp.gettremaining()<<"s remaining) ***";
		}
		// Else remove it since it is finished its execution
		if (p.front().gettremaining() == 0){
			popprocess();
		}
	}

	void showprocesses(string name){
		if (NOP==0)
			cout <<"\nNo processes in "<<name<<" Queue";
		else
		{
			for (int i=0; i<NOP; ++i){
				cout <<"\nProcess #"<<p[i].getpid()<<" in "<<name<<" Queue";
			}
		}
	}

	/* Default constructor for the Queue */
	Queue() {
		NOP=0;
	}

};

int main(){
	bool semaphore=true;
	int pid, starttime, servicetime; bool criticalsection=0;
	int currentTime=0, estimatedTotalTime=0, semtime=-1, currentpid=0;
	Queue readyQueue;
	Queue blockedQueue;
	Process processes[5];


	/* Calculate the Estimated time required for all the processes to Finish */
	for (int i=0; i<5; ++i){
		cout <<endl<<" **For process #"<<i<<" **";
		cout <<endl<<"\nEnter arrival time, service time and Critical Section requirement > ";
		cin >>starttime>>servicetime>>criticalsection;
		processes[i].setall(i, starttime, servicetime, criticalsection);
		estimatedTotalTime += servicetime;
	}

	/* Check if last process come late, then update the Estimated time accordingly*/
	if (processes[4].getstartTime() > estimatedTotalTime)
		estimatedTotalTime = processes[4].getstartTime() + processes[4].getserviceTime();

		for (int j=0; j<=estimatedTotalTime; ++j){
			cout <<"\n\n--------------------------------------------------------------------------------";
			cout <<"\n\t   ***** CURRENT SYSTEM UPTIME: "<<currentTime<<" seconds *****";
			// Check if any process needs to be executed at this time
			if (processes[currentpid].getstartTime() <= currentTime && currentpid <5){
				// Check if the process needs to enter Critical Section
				if (!processes[currentpid].getcs()){
					readyQueue.pushprocess(processes[currentpid]);
					cout <<"\nProcess #"<<currentpid<<" placed in Ready Queue";
					++currentpid;
				}
				else {
					// Check if Critical section is being used by other process
					if (semaphore){
						readyQueue.pushprocess(processes[currentpid]);
						cout <<"\nProcess #"<<currentpid<<" placed in Ready Queue";
						semtime = processes[currentpid].getserviceTime();
						++currentpid;
						semaphore=0;
					}
					else {
						// If a process if done with CS, then place the new process in Ready Queue
						if (semtime == 0 ){
							readyQueue.pushprocess(processes[currentpid]);
							cout <<"\nProcess #"<<currentpid<<" placed in Ready Queue";
							semtime = processes[currentpid].getserviceTime();
							++currentpid;
						}
						else {
							// A process is in CS, thus place the new process in Blocked Queue
							blockedQueue.pushprocess(processes[currentpid]);
							cout <<"\nProcess #"<<currentpid<<" placed in Blocked Queue";
							++currentpid;
							--semtime;
						}
					}
				}
			}

			// if Blocked Queue is empty, execute the processes in Ready Queue
			if (blockedQueue.getNOP() == 0){
				// Check if the first process in Ready Queue needs to be executed
				if (readyQueue.getprocess().getstartTime() < currentTime){
					if (readyQueue.getNOP() != 0)
						readyQueue.process();
				}
			}

			// if Ready Queue is empty, execute the processes in Blocked Queue
			else if (readyQueue.getNOP() == 0){
				// Check if the first process in Blocked Queue needs to be executed
				if (blockedQueue.getprocess().getstartTime() < currentTime){
					if (blockedQueue.getNOP() != 0)
						blockedQueue.process();
				}
			}

			// If both the Queues are filled, check which Queue needs to be processed first
			else {
				if (readyQueue.getprocess().getstartTime() < currentTime || blockedQueue.getprocess().getstartTime() < currentTime){
					if (readyQueue.getprocess().getstartTime() < blockedQueue.getprocess().getstartTime()){
						// Ensure that the Ready Queue is properly initialized before processing it
						if (readyQueue.getNOP() != 0)
							readyQueue.process();
					}
					else {
						// Ensure that the Blocked Queue is properly initialized before processing it
						if (blockedQueue.getNOP() != 0)
							blockedQueue.process();
					}
				}
			}

			blockedQueue.showprocesses("Blocked");
			readyQueue.showprocesses("Ready");

			++currentTime;
			cout <<endl;
			std::system("pause");
		}

		cout <<endl<<endl;
	}