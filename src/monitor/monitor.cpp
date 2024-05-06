/*! 
 @file monitor.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of monitor.h are implemented.
*/

#include <general/general.h>
#include <interface/ui/ui.h>
#include <monitor/monitor.h>
#include <interface/navdata/interface_navdata.h>

/************************************************
* Method definition for class: MonitorException *
*************************************************/

/* Get error call string */
const string& MonitorException::getErrorCall(void) const
{
	return errorCall;
}

/* Get error code */
const int MonitorException::getErrorCode(void) const
{
	return errorCode;
}

void MonitorException::setErrorCode(const int _errorCode)
{
	errorCode = _errorCode;
}

/***************************************
* Method definition for class: Monitor *
****************************************/

/* Class Instance Generation */
Monitor& Monitor::getInstance(void)
{
	static Monitor cInstance;
	return cInstance;
}

// Update input monitor
void Monitor::update(void)
{
	flagsMonitorVariables_e.set(MON_DISPLAY_DATA_CHECK, (bool)((cInterfaceNavdata.getEpochCounter() % TIME_TO_DISPLAY) == 0) );
}

const void Monitor::exitCode(const MonitorException& monExc)
{
	mainMonExc = monExc;
	const int errorCode = monExc.getErrorCode();
	ostringstream monitorStream;
	monitorStream << "Program exit with error code: " << errorCode << " - " << monExc.getErrorCall() << endl;
	updateDisplayOutputConsoleCpp(monitorStream.str(), true);
}

const int Monitor::getExitCode(void)
{
	return mainMonExc.getErrorCode();
}

Monitor::Monitor()
{
	flagsMonitorVariables_e.reset();
};
