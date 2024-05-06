/*! 
 @file monitor.h
 @author Nicolas Padron
 @brief Description: This file contains the monitor control functions and variables
*/

#ifndef MONITOR_HEADER
#define MONITOR_HEADER

#include <bitset>
#include <exception>
#include <general/general.h>
#include <processing/system/proc_system.h>

// Error codes checks
enum ErrorCodes_e {
	ERROR_RETURN_NOERROR,
	ERROR_RETURN_HELPCMD,
	ERROR_RETURN_FILE_FEOF,
	ERROR_RETURN_FILE_OPEN_ERROR,
	ERROR_RETURN_FILE_CLOSE_ERROR,
	ERROR_RETURN_FILE_WRITE_ERROR,
	ERROR_RETURN_FILE_READ_ERROR,
	ERROR_RETURN_OUT_RANGE,
	ERROR_RETURN_NUMBER_INPUTS,
	ERROR_RETURN_INCONSISTENT_INPUTS,
	ERROR_RETURN_NUMBER_KF_STD,
	ERROR_RETURN_IDX_HANDLED,
	ERROR_RETURN_UNKNOWN,
	ERROR_RETURN_TOTALERRORCODES
};

// Monitor variables
enum MonitorVariables_e {
	MON_IS_GPS_ECEF_REF_SET,
	MON_DISPLAY_DATA_CHECK,
	MON_TOTAL_MON_VARIABLES
};


class MonitorExceptionMap
{
	public:
	MonitorExceptionMap()
	{
		excMap.insert(std::pair<int, string>(ERROR_RETURN_NOERROR,"No Error."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_HELPCMD,"Help command called."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_FILE_FEOF,"End of file."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_FILE_OPEN_ERROR,"File open error."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_FILE_CLOSE_ERROR,"File close error."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_FILE_WRITE_ERROR,"File write error."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_OUT_RANGE,"Out of range value."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_NUMBER_INPUTS,"Error in number of inputs."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_INCONSISTENT_INPUTS,"Inconsistent input argument."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_NUMBER_KF_STD,"Error in number KF noises entries."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_IDX_HANDLED,"IDX written in file."));
		excMap.insert(std::pair<int, string>(ERROR_RETURN_UNKNOWN,"Error unknown."));
	}
	
	std::map<int, std::string> excMap;
};

// Exception monitor class
class MonitorException : public std::exception {
private:
	int errorCode;
	string errorCall;
public:
	MonitorException()
	{
		errorCode = ERROR_RETURN_NOERROR;
		errorCall = MonitorExceptionMap().excMap.at(errorCode);
	};
	MonitorException(const int _errorCode)
	{
		errorCode = _errorCode;
		errorCall = MonitorExceptionMap().excMap.at(errorCode);
	};
	MonitorException(MonitorException&&) = default;
	MonitorException(const MonitorException&) = default;
	MonitorException& operator=(const MonitorException&) = default;

	void setErrorCode(const int _errorCode);
	const string& getErrorCall(void) const;
	const int getErrorCode(void) const;
};

/* General Monitor class */
class Monitor {
public:
	static Monitor& getInstance(void);
	Monitor(const Monitor&) = delete;
	Monitor& operator=(const Monitor&) = delete;
	~Monitor(){};

	void update(void);
	const int getExitCode(void);
	const void exitCode(const MonitorException& monExc);
	std::bitset<MON_TOTAL_MON_VARIABLES> flagsMonitorVariables_e;
private:
	MonitorException mainMonExc;

	Monitor();
};
extern Monitor& cMonitor;

#endif // MONITOR_HEADER
