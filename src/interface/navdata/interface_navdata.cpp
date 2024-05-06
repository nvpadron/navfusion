/*! 
 @file interface_navdata.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of monitor.h are implemented.
*/

#include <general/general.h>
#include <interface/ui/ui.h>
#include <monitor/monitor.h>
#include <interface/navdata/interface_navdata.h>
#include <interface/io/in/io_in.h>
#include <processing/frames/frames.h>
#include <processing/system/proc_system.h>

using namespace Frames;

/********************************************
* Method definition for class: InputMonitor *
*********************************************/

// Sigleton class, return static constructor
NavDataInterface& NavDataInterface::getInstance()
{
	static NavDataInterface instance;
	return instance;
}

// Initialize input monitor on defined KEYS
void NavDataInterface::initialize(void)
{
	InputIds sInputIds =  UI::getInstance().getInputIds();
	sInputValues = UI::getInstance().getInputValues();
	epochCounter = 0;

	// Check input GPS
	mapInputMonitor.insert({ KEY_GPS, MapInputMonitorStruct(arma::Mat<int>({sInputIds.GPS.at(0), sInputIds.GPS.at(1), sInputIds.HEIGHT})) });

	// Check input ACC
	mapInputMonitor.insert({ KEY_ACC, MapInputMonitorStruct(arma::Mat<int>({sInputIds.ACC.at(0), sInputIds.ACC.at(1), sInputIds.ACC.at(2)})) });

	// Check input GYR
	mapInputMonitor.insert({ KEY_GYR, MapInputMonitorStruct(arma::Mat<int>({sInputIds.GYR.at(0), sInputIds.GYR.at(1), sInputIds.GYR.at(2)})) });

	// Check input MAG
	mapInputMonitor.insert({ KEY_MAG, MapInputMonitorStruct(arma::Mat<int>({sInputIds.MAG.at(0), sInputIds.MAG.at(1), sInputIds.MAG.at(2)})) });

	// Check input RPY
	mapInputMonitor.insert({ KEY_RPY, MapInputMonitorStruct(arma::Mat<int>({sInputIds.ROLL, sInputIds.PITCH, sInputIds.YAW})) });

	// Check input HDOP
	mapInputMonitor.insert({ KEY_HDOP, MapInputMonitorStruct(arma::Mat<int>({sInputIds.HDOP})) });

	isGpsDataNew = false;
	isGpsDataValid = false;
}

/* Update input monitor on defined KEYS */
void NavDataInterface::update(void)
{
	Input& cInput = Input::getInstance();
	arma::vec rpyIns = NavsystemsHolder::getInstance().getPtrIns().RPY;
	arma::vec oldGpsData = mapInputMonitor.at(KEY_GPS).inputHolder;
	arma::vec oldAcc = mapInputMonitor.at(KEY_ACC).inputHolder; 
	arma::vec oldGyr = mapInputMonitor.at(KEY_GYR).inputHolder;
	arma::vec gl = arma::vec({0,0,0});
	std::vector<int> quant;
	double fieldvalue = 0;
	
	// Increase epoch counter
	epochCounter += 1;

	try
	{
		// For each Key and for each inputCheck != false, then fill the corresponding input value
		for (MapInputMonitor_t::iterator it = mapInputMonitor.begin(); it != mapInputMonitor.end(); it++)
		{
			for (int i = 0; i < it->second.nElems; i++)
			{
				if (it->second.inputId.at(i) != -1)
				{
					fieldvalue = cInput.getFieldvalue(it->second.inputId[i]);
					it->second.inputHolder.at(i) = fieldvalue;
				}
			}
		}
	}
	catch (const MonitorException&)
	{
		throw;
	}

	// Assign default/entered height if not part of CSV
	if (mapInputMonitor.at(KEY_GPS).inputId.at(2) == -1)
	{
		mapInputMonitor.at(KEY_GPS).inputHolder.at(2) = sInputValues.heightVal;
	}

	// Apply bias correction if entered, otherwise take 1st value.
	mapInputMonitor.at(KEY_ACC).inputHolder -= sInputValues.accRest;
	mapInputMonitor.at(KEY_GYR).inputHolder -= sInputValues.gyrRest;

 	quant = arma::conv_to<std::vector<int>>::from(mapInputMonitor.at(KEY_ACC).inputHolder * sInputValues.quantFactor);
	mapInputMonitor.at(KEY_ACC).inputHolder = arma::conv_to<arma::vec>::from(quant) / sInputValues.quantFactor;
 	quant = arma::conv_to<std::vector<int>>::from(mapInputMonitor.at(KEY_GYR).inputHolder * sInputValues.quantFactor);
	mapInputMonitor.at(KEY_GYR).inputHolder = arma::conv_to<arma::vec>::from(quant) / sInputValues.quantFactor;
 	quant = arma::conv_to<std::vector<int>>::from(mapInputMonitor.at(KEY_MAG).inputHolder * sInputValues.quantFactor);
	mapInputMonitor.at(KEY_MAG).inputHolder = arma::conv_to<arma::vec>::from(quant) / sInputValues.quantFactor;
 	quant = arma::conv_to<std::vector<int>>::from(mapInputMonitor.at(KEY_RPY).inputHolder * sInputValues.quantFactor);
	mapInputMonitor.at(KEY_RPY).inputHolder = arma::conv_to<arma::vec>::from(quant) / sInputValues.quantFactor;
	
	// Platform to body 
	mapInputMonitor.at(KEY_ACC).inputHolder = Frames::matrixPlatform2Body(sInputValues.diagPlat2Body) * mapInputMonitor.at(KEY_ACC).inputHolder;
	//mapInputMonitor.at(KEY_GYR).inputHolder = Frames::matrixPlatform2Body(sInputValues.diagPlat2Body) * mapInputMonitor.at(KEY_GYR).inputHolder;
	
	if(sInputValues.feedbackBias)
	{
		mapInputMonitor.at(KEY_ACC).inputHolder += NavsystemsHolder::getInstance().getPtrKf().X.subvec(9,11);
		mapInputMonitor.at(KEY_GYR).inputHolder += NavsystemsHolder::getInstance().getPtrKf().X.subvec(12,14);
	}

	/* Input angles covnerted to radians to avoid unecessary conversions in processing functions */
	if (!sInputValues.inputAnglesInRadians)
	{
		mapInputMonitor.at(KEY_RPY).inputHolder %= arma::repmat(arma::vec({ DEG2RAD }), 3, 1);
		//mapInputMonitor.at(KEY_GYR).inputHolder %= arma::repmat(arma::vec({ DEG2RAD }), 3, 1);
	}
	mapInputMonitor.at(KEY_GPS).inputHolder.subvec(0, 1) %= arma::repmat(arma::vec({ DEG2RAD }), 2, 1);
	isGpsDataNew = arma::sum(arma::abs(oldGpsData - mapInputMonitor.at(KEY_GPS).inputHolder)) > 0;
	isGpsDataValid = !mapInputMonitor.at(KEY_GPS).inputHolder.has_nan();

	mapInputMonitor.at(KEY_ACC).inputHolder %= sInputValues.bodySelector;
	mapInputMonitor.at(KEY_GYR).inputHolder %= sInputValues.attitudeSelector;
	mapInputMonitor.at(KEY_MAG).inputHolder %= sInputValues.bodySelector;

	if(sInputValues.doPlatformAlignment)
	{
		mapInputMonitor.at(KEY_ACC).inputHolder = Frames::matrixBody2H(rpyIns) * mapInputMonitor.at(KEY_ACC).inputHolder;
		mapInputMonitor.at(KEY_GYR).inputHolder = Frames::matrixBody2H(rpyIns) * mapInputMonitor.at(KEY_GYR).inputHolder;
	}

	if(sInputValues.correctForGravity)
	{
		gl(2) = Frames::gravityCorrectionForComponentZ(mapInputMonitor.at(KEY_GPS).inputHolder(2),mapInputMonitor.at(KEY_GPS).inputHolder(0));
		mapInputMonitor.at(KEY_ACC).inputHolder -= Frames::matrixBody2Enu(rpyIns) * gl;
	}
}

/* Get map holding inputs organized in KEYS */
const MapInputMonitor_t& NavDataInterface::getMapInputMonitor(void) const
{
	return mapInputMonitor;
}

/* Get user entered input values */
const InputValues_s& NavDataInterface::getInputValues(void) const
{
	return sInputValues;
}

const bool NavDataInterface::getIsGpsDataNew(void) const
{
	return isGpsDataNew;
}

const bool NavDataInterface::getIsGpsDataValid(void) const
{
	return isGpsDataValid;
}

const int NavDataInterface::getEpochCounter(void) const
{
	return epochCounter;
}
