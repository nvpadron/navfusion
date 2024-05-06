/*! 
 @file general.h
 @author Nicolas Padron
 */

#ifndef _HEADER_IO_OUT_
#define _HEADER_IO_OUT_

#include <general/general.h>
#include <processing/system/proc_system.h>

/** CONSTANTS */

const string KML_HEADER_1 =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<kml>"
"  <Document>";
// "    <name>Paths</name>\n" // This is filled in header function
const string KML_HEADER_2 =
"    <description>ROUTE</description>\n"
"    <Style id=\"yellowLineGreenPoly\">\n"
"      <LineStyle>";
// "        <color>7f00ffff</color>\n" // This is filled in header function
const string KML_HEADER_3 =
"        <width>3</width>\n"
"      </LineStyle>\n"
"    </Style>\n"
"    <Placemark>\n"
"      <name>Absolute Extruded</name>\n"
"      <description>LLH</description>\n"
"      <styleUrl>#yellowLineGreenPoly</styleUrl>\n"
"      <LineString>\n"
"        <extrude>1</extrude>\n"
"        <tessellate>1</tessellate>\n"
"        <altitudeMode>absolut</altitudeMode>\n"
"        <coordinates>";

const string KML_FOOTER =
"        </coordinates>\n"
"      </LineString>\n"
"    </Placemark>\n"
"  </Document>\n"
"</kml>\n";

// KML colors
const string KML_COLOR_RED   = "FF0000FF";
const string KML_COLOR_GREEN = "FF00FF00";
const string KML_COLOR_BLUE  = "FFFF0000";

/*! 
 @brief to add the output to the std::map to write, and read it back. 
 \class Output_c 
 */
class Output_c {
public:
	Output_c(const Output_c&) = delete;
	Output_c operator=(const Output_c&) = delete;
	~Output_c()
	{
			   kmlWriteFooter();
	};
	/*! Singleton class, function returns static object from private constructor */
	static Output_c& getInstance(void);

	/*! Write headers: for both analysis CSV and KMLs */
	void writeHeaders(void);
	/*! Write content: for both analysis CSV and KMLs */
	void writeContent(void);
	/*! Write footer for KMLs. Cannot be handled together with analysis CSV like header and content.*/
	void kmlWriteFooter(void);

private:
	Output_c()
	{
		valuesStream.precision(10);
	};
	// Functions for CSV processing
	void fillStreams(const string fieldanme, const double fieldvalue);
	void csvSetData();
	// And for KML processing
	void kmlSetHeader(const string name, const string color);
	void kmlSetFooter(void);
	void kmlSetContent(const arma::vec llh);
	// Stream variables
	ostringstream titlesStream;
	ostringstream valuesStream;
};

#endif _HEADER_IO_OUT_