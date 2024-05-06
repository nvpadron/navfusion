/*! 
 @file interface_navdata.h
 @author Nicolas Padron
 @brief Description: This file contains the variables holding the input arguments that play a role in navigation:
	- CSV indexes for IMU or GPS measurements and corresponding values (MapInputMonitorStruct)
	- Command line inputs for controlling/tuning the processing flow (InputValues_t)
		- NOTE: this command line entered inputs are part of ui.cpp/.h, however for centralization it is convenient to have them accessible from intetrface_navdata.cpp/.h.
*/

#ifndef _HEADER_NAVDATA_INTERFACE_
#define _HEADER_NAVDATA_INTERFACE_

#include <general/general.h>
#include <interface/ui/ui.h>


/* Input Keys */
enum MonitorInputKeys_e {
	KEY_GPS,
	KEY_ACC,
	KEY_GYR,
	KEY_MAG,
	KEY_RPY,
	KEY_HDOP,
	KEY_TOTAL
};

/*! 
 @brief Input monitor struct for each Key.
 The idea is to parse every input field with an id (user entered CSV column number) and store the corresponding value.
 */
class MapInputMonitorStruct {
public:
	/*! Constructor */
	MapInputMonitorStruct(){};

	/*! 
	@brief Constructor used to insert/create new entry in NavDataInterface::Initialize function
	@param in_inputId: ID number, i.e., column number in CSV/Excel
	@param in_inputHolder: actual value. It is entered as a 3D array: XYZ (for accelerometers, gyrometers and magnetometers), radians (for LAT/LON, or attitude angles).
	*/
	MapInputMonitorStruct(arma::Mat<int> in_inputId, arma::vec in_inputHolder = arma::vec({0, 0, 0}))
	{
		inputId = in_inputId;
		nElems = (uint8_t)inputId.n_cols;
		inputHolder = in_inputHolder;
	}

	// Variables
	arma::Mat<int> inputId;
	arma::vec inputHolder;
	uint8_t nElems = 0;
};

/* Typedef map for input monitor struct */
typedef std::map<MonitorInputKeys_e, MapInputMonitorStruct> MapInputMonitor_t;

/*!
 @brief Input data file monitor class. Its purpose is to manage the input data to be used for navigation. It is not the same as UI input or general Input class handler.
 \class NavDataInterface
*/
class NavDataInterface {
public:
	/*! Singleton class, function returns static object from private constructor */
	static NavDataInterface& getInstance();
	NavDataInterface& operator=(const NavDataInterface&) = delete;
	~NavDataInterface() {};

	/*! Initialize interface. Creates MapInputMonitorStruct entries. */
	void initialize(void);
	
	/*! Update at every new input file row, i.e., every unit of time. Updates the map field in MapInputMonitorStruct entries. */
	void update(void);

	/*! Function to retrieve private MapInputMonitorStruct */
	const MapInputMonitor_t& getMapInputMonitor(void) const;
	
	/*! Function to retrieve user entered input values with navigation purpose (different to the general UI entered, which are for example filenames or CSV columns) */
	const InputValues_t& getInputValues(void) const;
	
	/*! Function to retrieve the bool checking if GPS data is new, in which case the KF is updated.
	A getter function for a single variable might be excessive, but this way we can ensure that the value is not modified anywhere else*/
	const bool getIsGpsDataNew(void) const;
	
	/*! Function to retreive the bool checking if GPS data is valied, i.e. non-nan. */
	const bool getIsGpsDataValid(void) const;
	
	/*! Get epoch counter */
	const int getEpochCounter(void) const;
	
private:
	// Private constructor
	NavDataInterface() {};
	// Variables
	MapInputMonitor_t mapInputMonitor;
	InputValues_t sInputValues;
	int epochCounter;
	bool isGpsDataNew;
	bool isGpsDataValid;
};
extern NavDataInterface& cInterfaceNavdata;

#endif // _HEADER_NAVDATA_INTERFACE_