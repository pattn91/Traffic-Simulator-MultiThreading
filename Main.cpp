/*
	Patrick Nguyen - 28586045 - patritn1
	Dennis Tran - 57725022 - dennisnt
	Bing Hui Feng - 78912993 - bhfeng
	Kevin Pham - 51044146 - kkpham1

	Team 42

	Note --	To compile in g++, type: 
			"g++ Main.cpp -lpthread"

			To run, type:
			"./a.out"

			Output file:
			"bridge.out"

	In your solution, if a car arrives while traffic 
	is currently moving in its direction of travel 
	across the bridge, but there is another car already 
	waiting to cross in the opposite direction, will the 
	new arrival cross before the car waiting on the other 
	side, after the car on the other side, or is it 
	impossible to say? Explain briefly.

	Answer: In our solution, the new arrival can only cross 
	if the bridge has less than 3 cars and is going in the 
	same direction. So if the new arrival comes after the 
	waiting car, and there is less than 3 cars on the bridge,
	the new arrival gets to cross.
	
	If the bridge has 3 cars, the new arrival has to wait. At 
	this point, it would be impossible to predict when it will
	be able to cross, as cars placed in the waiting pool gets
	randomly picked to cross (when conditions are right for it
	to cross).
*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;

struct thread_data{
	int id;
	int direc;
};

pthread_t* threads;
thread_data* td;

int numOfCars = 0;
int* rets;
int currentDir = 2;
int carsIn;
ofstream fileOut;


void ArriveBridge(int id, int direc);
void CrossBridge(int id, int direc);
void ExitBridge(int id, int direc);
void *oneVehicle(void* threadarg);
void parseFile();

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main()
{
	parseFile();

	fileOut.open("bridge.out");

	// Create independent threads each of which will execute function 
	for (int i = 0; i < numOfCars; i++){
		rets[i] = pthread_create(&threads[i], NULL, oneVehicle, (void*)&td[i]);
	}

	// Start threads
	for (int i = 0; i < numOfCars; i++)
		pthread_join(threads[i], NULL);

	fileOut.close();

	system("pause");
	return 0;
}

// make the thread and functions it will call
void * oneVehicle(void* threadarg)
{
	struct thread_data *my_data;
	my_data = (struct thread_data *) threadarg;

	ArriveBridge(my_data->id, my_data->direc);
	CrossBridge(my_data->id, my_data->direc);
	ExitBridge(my_data->id, my_data->direc);

	pthread_exit(NULL);
	return NULL;
}

// parses file and makes a list of structs for creating threads later
void parseFile()
{
	ifstream ifs;
	ifs.open("bridge.in");

	string str;
	string file_contents;
	int counter = 0;

	if (ifs.is_open())
	{
		getline(ifs, str);
		numOfCars = atoi(str.c_str());

		rets = new int[numOfCars];
		td = new thread_data[numOfCars];
		threads = new pthread_t[numOfCars];

		for (int i = 0; i < numOfCars; i++)
		{
			getline(ifs, str);
			string temp;
			istringstream  ss(str);

			ss >> temp;
			td[i].id = atoi(temp.c_str());
			ss >> temp;
			td[i].direc = atoi(temp.c_str());
		}
	}
	ifs.close();
}

void ArriveBridge(int id, int direc)
{
	// get mutex 
	pthread_mutex_lock(&mutex);
	cout << "Car " << id << " arrives at the bridge in direction " << direc << endl;
	fileOut << "Car " << id << " arrives at the bridge in direction " << direc << endl;

	// if no cars
	if (carsIn == 0)
	{
		currentDir = direc;
		cout << "\nTraffic Direction is being changed to " << currentDir << "\n";
		fileOut << "\nTraffic Direction is being changed to " << currentDir << "\n";

		carsIn++;

		return;
	}

	// if bridge is full, or there's cars on the bridge but going the wrong direction, wait
	while (carsIn == 3 || (carsIn != 0 && direc != currentDir))
	{
		cout << "Car " << id << " waits to travel in direction " << direc << endl;
		fileOut << "Car " << id << " waits to travel in direction " << direc << endl;

		pthread_cond_wait(&cond, &mutex); // check when it can get cond and mutex
	}

	// here need to reset direction as needed
	if (direc != currentDir)
	{
		currentDir = direc;
		cout << "\nTraffic Direction is being changed to " << currentDir << "\n";
		fileOut << "\nTraffic Direction is being changed to " << currentDir << "\n";
	}

	// conditions are good for the car to pass and enter the bridge.
	carsIn++;
}

void CrossBridge(int id, int direc)
{
	cout << "*Car " << id << " crossing bridge in direction " << direc << "\n";
	fileOut << "*Car " << id << " crossing bridge in direction " << direc << "\n";

	sleep(1);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	sleep(2);
}

void ExitBridge(int id, int direc)
{
	pthread_mutex_lock(&mutex);
	carsIn--;

	cout << "**Car " << id << " exiting bridge in direction " << direc << endl;
	fileOut << "**Car " << id << " exiting bridge in direction " << direc << endl;

	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}