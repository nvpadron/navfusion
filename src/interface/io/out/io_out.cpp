/*! 
 @file io_out.cpp
 @author Nicolas Padron
 @brief Description: This file performs output writing as CSV for analysis as well as GPS, INS and Fusion KML files.
*/

#include <string.h>

#include <processing/system/proc_system.h>
#include <processing/frames/frames.h>
#include <interface/io/in/io_in.h>
#include <interface/io/out/io_out.h>

using namespace std;
using namespace Frames;

Output_c& Output_c::getInstance(void)
{
	static Output_c instance;
	return instance;
}

void Output_c::kmlSetFooter(void)
{
	// Clear streams
	titlesStream.str("");
	titlesStream.clear();
	// Form streams
	titlesStream << KML_FOOTER << endl;
}

void Output_c::kmlSetHeader(const string label, const string color)
{
	// Clear Streams
	titlesStream.str("");
	titlesStream.clear();
	// Form Streams
	string kmlStream = KML_HEADER_1 + 
		               " \n   <name>" + label + "</name>\n" +
					   KML_HEADER_2 + 
					   " \n        <color>" + color + "</color>\n" + 
					   KML_HEADER_3;
	titlesStream << kmlStream << endl;
}

void Output_c::kmlSetContent(arma::vec llh)
{
	// Clear streams
	valuesStream.str("");
	valuesStream.clear();
	// Form streams
	valuesStream << "        " << llh[1] * RAD2DEG << "," << llh[0] * RAD2DEG << "," << llh[2] << "\n";
}

void Output_c::kmlWriteFooter(void)
{
	Input& cInput = Input::getInstance();

	// Clear and set stream (header)
	kmlSetFooter();
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_GPS).writeContent(titlesStream.str().c_str());
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_INS).writeContent(titlesStream.str().c_str());
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_FUSION).writeContent(titlesStream.str().c_str());
}

void Output_c::fillStreams(const string fieldname, const double fieldvalue)
{
	titlesStream << fieldname << ",";
	valuesStream << fieldvalue << ",";
}

void Output_c::csvSetData()
{
	const NavsystemsHolder& sNavSystems = NavsystemsHolder::getInstance();

	// Clear streams
	titlesStream.str("");
	titlesStream.clear();
	valuesStream.str("");
	valuesStream.clear();

	// Fill and write CSV
	fillStreams("GPS_LAT", sNavSystems.getPtrGps().LLH[0] * RAD2DEG);
	fillStreams("GPS_LON", sNavSystems.getPtrGps().LLH[1] * RAD2DEG);
	
	fillStreams("INS_LAT", sNavSystems.getPtrIns().LLH[0] * RAD2DEG);
	fillStreams("INS_LON", sNavSystems.getPtrIns().LLH[1] * RAD2DEG);
	fillStreams("INS_V", arma::norm(sNavSystems.getPtrIns().V,2));
	fillStreams("INS_ROLL", sNavSystems.getPtrIns().RPY(0) * RAD2DEG);
	fillStreams("INS_PITCH", sNavSystems.getPtrIns().RPY(1) * RAD2DEG);
	fillStreams("INS_YAW", sNavSystems.getPtrIns().RPY(2) * RAD2DEG);

	fillStreams("FUS_LAT", sNavSystems.getPtrFusion().LLH[0] * RAD2DEG);
	fillStreams("FUS_LON", sNavSystems.getPtrFusion().LLH[1] * RAD2DEG);
	fillStreams("FUS_V", arma::norm(sNavSystems.getPtrFusion().V,2));
	fillStreams("FUS_ROLL", sNavSystems.getPtrFusion().RPY(0) * RAD2DEG);
	fillStreams("FUS_PITCH", sNavSystems.getPtrFusion().RPY(1) * RAD2DEG);
	fillStreams("FUS_YAW", sNavSystems.getPtrFusion().RPY(2) * RAD2DEG);

	titlesStream << endl;
	valuesStream << endl;
}

void Output_c::writeHeaders()
{
	Input& cInput = Input::getInstance();
	
	// Fill and write CSV
	csvSetData();
	cInput.cFilesHandler.at(FILE_OUTPUT).writeContent(titlesStream.str().c_str());
	
	// Fill and write KMLs
	// Clear and set stream (header)
	kmlSetHeader("GPS", KML_COLOR_BLUE);
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_GPS).writeContent(titlesStream.str().c_str());
	// Clear and set stream (header)
	kmlSetHeader("INS", KML_COLOR_RED);
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_INS).writeContent(titlesStream.str().c_str());
	// Clear and set stream (header)
	kmlSetHeader("FUSION", KML_COLOR_GREEN);
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_FUSION).writeContent(titlesStream.str().c_str());
}

void Output_c::writeContent()
{
	Input& cInput = Input::getInstance();
	const NavsystemsHolder& sNavSystems = NavsystemsHolder::getInstance();

	// Fill and write CSV
	csvSetData();
	cInput.cFilesHandler.at(FILE_OUTPUT).writeContent(valuesStream.str().c_str());

	// Fill and write KMLs
	// Clear and set stream (values)
	kmlSetContent(sNavSystems.getPtrGps().LLH);
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_GPS).writeContent(valuesStream.str().c_str());
	// Clear and set stream (values)
	kmlSetContent(sNavSystems.getPtrIns().LLH);
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_INS).writeContent(valuesStream.str().c_str());
	// Clear and set stream (values)
	kmlSetContent(sNavSystems.getPtrFusion().LLH);
	// Write streams
	cInput.cFilesHandler.at(FILE_OUTPUT_KML_FUSION).writeContent(valuesStream.str().c_str());
}
