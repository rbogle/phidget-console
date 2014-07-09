// phidget1014.cpp : Defines the entry point for the console application.

/*
This console application can be used to drive the relays on a Phidget InterfaceKit board (tested on 0/0/4, but should work on others)
You must have the Phidget Drivers installed on your system for this to work. See: http://www.phidgets.com/docs/OS_-_Windows#Quick_Downloads
It is driven by command line options.

Allowed options:
-? [ --help ]         produce help message and exit
-i [ --info ]         get diagnostic info from phidget and exits
-p [ --port ] arg     set port number, if no port is set then all ports are used.
-s [ --state ] arg    state to set port: 0 off, 1 on. if state is not set then 0 is used
-t [ --toggle ]       toggle specific port or all ports
-v [ --verbose ]      output state of ports as they change

Notes:
----------------------
Many options can (or should) be combined.
Default settings result in the following behaviors:

if no port or state options are given, all ports are shut off.
if only state is given (0 or 1) then all ports are set to that state.
toggling can work with a specified port, with no port specified all ports are used, supplying state has no effect.
*/

//#include "stdafx.h"
#include <iostream>
#include <phidget21.h>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

int CCONV AttachHandler(CPhidgetHandle IFK, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);

	//printf("%s %10d attached!\n", name, serialNo);
	cout << name <<": "<< serialNo << " is attached!\n";

	return 0;
}

int CCONV DetachHandler(CPhidgetHandle IFK, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);

	//printf("%s %10d detached!\n", name, serialNo);
	cout << name << ": " << serialNo << " is dettached!\n";

	return 0;
}

//callback that will run if an output changes.
//Index - Index of the output that generated the event, State - boolean (0 or 1) representing the output state (on or off)
int CCONV OutputChangeHandler(CPhidgetInterfaceKitHandle IFK, void *usrptr, int Index, int State)
{
	//printf("Digital Output: %d > State: %d\n", Index, State);
	cout << "Digital Output: " << Index << " State: " << State << "\n";
	return 0;
}

int CCONV ErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
	//printf("Error handled. %d - %s", ErrorCode, unknown);
	cout << "Error handled. " << ErrorCode << " - " << unknown << "\n";
	return 0;
}

int togglePort(CPhidgetInterfaceKitHandle ifk, int port){
	int state;
	CPhidgetInterfaceKit_getOutputState(ifk, port, &state);
	state ^= 1;
	return CPhidgetInterfaceKit_setOutputState(ifk, port, state);

}

int toggleAllPorts(CPhidgetInterfaceKitHandle ifk, int numports){
	for (int port = 0; port < numports; port++){
		togglePort(ifk, port);
	}
	return 0;
}

int setPortState(CPhidgetInterfaceKitHandle ifk, int port, int state){
	return CPhidgetInterfaceKit_setOutputState(ifk, port, state);
	
}

int setAllPortStates(CPhidgetInterfaceKitHandle ifk, int numports, int state){
	for (int port = 0; port < numports; port++){
		CPhidgetInterfaceKit_setOutputState(ifk, port, state);
	}
	return 0;

}

int probePhidget(CPhidgetInterfaceKitHandle ifk){
	int serialNo, version, numOutputs, port, state;
	const char* ptr;

	CPhidget_getDeviceName((CPhidgetHandle)ifk, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)ifk, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)ifk, &version);
	CPhidgetInterfaceKit_getOutputCount(ifk, &numOutputs);
	cout << ptr << "\n";
	cout << "Serial No: " << serialNo << "\n";
	cout << "Version: " << version << "\n";
	cout << "Number of Relays: " << numOutputs << "\n";

	for (port = 0; port < numOutputs; port++){
		CPhidgetInterfaceKit_getOutputState(ifk, port, &state);
		cout << "Port: " << port << " State: " << state << "\n";
	}
	return 0;
}


int main(int ac, char* av[])
{
	int result, numports, port, state;
	const char *err;

	try{

		port = -1; //all ports
		state = 0; //off
		numports = 4; //for the phidget1014 board

		po::options_description desc("Allowed options");
		desc.add_options()
			("help,?", "produce help message")
			("info,i", "get diagnostic info from phidget")
			("port,p", po::value<int>(&port), "set port number 1-4, if no port is set then all ports are used.")
			("state,s", po::value<int>(&state), "state to set port: 0 off, 1 on. if state is not set then 0 is used")
			("toggle,t", "toggle specific port or all ports")
			("verbose,v", "output state of ports")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cout << desc << "\n";
			return 0;
		}

		//Declare an InterfaceKit handle
		CPhidgetInterfaceKitHandle ifKit = 0;

		//create the InterfaceKit object
		CPhidgetInterfaceKit_create(&ifKit);

		if (vm.count("verbose")){
			//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
			//CPhidget_set_OnAttach_Handler((CPhidgetHandle)ifKit, AttachHandler, NULL);
			//CPhidget_set_OnDetach_Handler((CPhidgetHandle)ifKit, DetachHandler, NULL);
			//CPhidget_set_OnError_Handler((CPhidgetHandle)ifKit, ErrorHandler, NULL);
			//Registers a callback that will run if an output changes.
			CPhidgetInterfaceKit_set_OnOutputChange_Handler(ifKit, OutputChangeHandler, NULL);
		}

		//open the interfacekit for device connections
		CPhidget_open((CPhidgetHandle)ifKit, -1);

		//get the program to wait for an interface kit device to be attached
		//cout<<"Waiting for interface kit to be attached....\n";
		if ((result = CPhidget_waitForAttachment((CPhidgetHandle)ifKit, 10000)))
		{
			CPhidget_getErrorDescription(result, &err);
			cout<<"Problem waiting for attachment: "<<err<<"\n";
			cout << "Exiting....\n";
			return 0;
		}
	
		CPhidgetInterfaceKit_getOutputCount(ifKit, &numports);

		if (vm.count("info")){
			probePhidget(ifKit);
		}
		// no probe requested so we will try to toggle or set port
		else{ 

			if (vm.count("toggle")){
				if (port < 0){
					toggleAllPorts(ifKit,numports);
				}
				else{
					togglePort(ifKit, port);
				}
			}
			//we were not asked to toggle ports so we will set one
			else{
				if (port < 0){
					setAllPortStates(ifKit, numports, state);
				}
				else{
					setPortState(ifKit, port, state);
				}
			}
		}

		CPhidget_close((CPhidgetHandle)ifKit);
		CPhidget_delete((CPhidgetHandle)ifKit);

	}
	catch (exception& e) {
		cerr << "error: " << e.what() << "\n";
		return 1;
	}
	catch (...) {
		cerr << "Exception of unknown type!\n";
	}
	return 0;
}

