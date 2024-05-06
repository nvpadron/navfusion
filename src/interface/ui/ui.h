/*! 
 @file ui.h
 @author Nicolas Padron
 */

#ifndef _HEADER_UI_
#define _HEADER_UI_

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <interface/io/files/io_files.h>
#include <interface/io/in/io_in.h>

/** UI related defines */
#define TIME_TO_DISPLAY 2000
#define DISPLAY_ALL_MSGS true

#ifdef WFUI_INTERFACE
__declspec(dllexport) void updateProgressBarCpp(int value);
__declspec(dllexport) void updateDisplayOutputConsoleCpp(const Type str, const bool forceDisplay = false);
#else

void updateProgressBarCpp(int value);
void updateDisplayOutputConsoleCpp(const std::string str, const bool forceDisplay = false);
#endif // WFUI_INTERFACE

/** Constants related to input arguments */
constexpr int INPUT_ARGS_NUM = 29;

constexpr char INPUT_ARGS_INFILE 			= 'I';
constexpr char INPUT_ARGS_OUTFILE 			= 'O';
constexpr char INPUT_ARGS_INTERVAL_GPS_OFF 	= 'T';
constexpr char INPUT_ARGS_KFCFG 			= 'K';
constexpr char INPUT_ARGS_FS 				= 'F';
constexpr char INPUT_ARGS_LATLON 			= 'C';
constexpr char INPUT_ARGS_HEIGHT 			= 'H';
constexpr char INPUT_ARGS_HEIGHT_VAL		= 'h';
constexpr char INPUT_ARGS_ACC 				= 'A';
constexpr char INPUT_ARGS_ACC_REST 		    = 'a';
constexpr char INPUT_ARGS_GYR 				= 'W';
constexpr char INPUT_ARGS_GYR_REST 		    = 'w';
constexpr char INPUT_ARGS_MAG 				= 'M';
constexpr char INPUT_ARGS_ROLL 				= 'R';
constexpr char INPUT_ARGS_PITCH 			= 'P';
constexpr char INPUT_ARGS_YAW 				= 'Y';
constexpr char INPUT_ARGS_GRAVITYCORR 		= 'g';
constexpr char INPUT_ARGS_ANGLES 			= 'r';
constexpr char INPUT_ARGS_PROGRESS_ANGLES 	= 'y';
constexpr char INPUT_ARGS_BODYSELECTION 	= 'x';
constexpr char INPUT_ARGS_ATTITUDESELECTION = 'z';
constexpr char INPUT_ARGS_PLAT2BODY 		= 'p';
constexpr char INPUT_ARGS_ALIGNMENT         = 'l';
constexpr char INPUT_ARGS_FEEDBACK_BIAS     = 'f';
constexpr char INPUT_ARGS_TAU				= 't';
constexpr char INPUT_MECHANICS_LOCAL		= 'm';
constexpr char INPUT_QUANTIZATION_FACTOR	= 'q';
constexpr char INPUT_ARGS_INDEX				= 'i';
constexpr char INPUT_ARGS_HELP 				= '?';

constexpr std::array<char, INPUT_ARGS_NUM> INPUT_ARGS_LABELS{
	INPUT_ARGS_INFILE,
	INPUT_ARGS_OUTFILE,
	INPUT_ARGS_INTERVAL_GPS_OFF,
	INPUT_ARGS_KFCFG,
	INPUT_ARGS_FS,
	INPUT_ARGS_LATLON,
	INPUT_ARGS_HEIGHT,
	INPUT_ARGS_HEIGHT_VAL,
	INPUT_ARGS_ACC,
	INPUT_ARGS_ACC_REST,
	INPUT_ARGS_GYR,
	INPUT_ARGS_GYR_REST,
	INPUT_ARGS_MAG,
	INPUT_ARGS_ROLL,
	INPUT_ARGS_PITCH,
	INPUT_ARGS_YAW,
	INPUT_ARGS_GRAVITYCORR,
	INPUT_ARGS_ANGLES,
	INPUT_ARGS_PROGRESS_ANGLES,
	INPUT_ARGS_BODYSELECTION,
	INPUT_ARGS_ATTITUDESELECTION,
	INPUT_ARGS_PLAT2BODY,
	INPUT_ARGS_ALIGNMENT,
	INPUT_ARGS_FEEDBACK_BIAS,
	INPUT_MECHANICS_LOCAL,
	INPUT_QUANTIZATION_FACTOR,
	INPUT_ARGS_TAU,
	INPUT_ARGS_HELP
};

constexpr int INPUT_SUBARGS_NUM = 1;
constexpr char INPUT_SUBARGS_INDEX[] = "idx";
constexpr std::array<char[4], INPUT_SUBARGS_NUM> INPUT_SUBARGS_LABELS{
	"idx"
};

const string OUTPUT_FILENAME = "output.csv";
const string OUTPUT_FILENAME_GPS = "kml_gps.kml";
const string OUTPUT_FILENAME_IRS = "kml_irs.kml";
const string OUTPUT_FILENAME_FUSION = "kml_fusion.kml";

/* Fileanames entered at input command */
enum FilenamesInput_e {
	FILENAMES_INPUT_INPUT,
	FILENAMES_INPUT_OUTPUT,
	FILENAMES_INPUT_TOTAL
};

/* Typedefs */
typedef std::map<std::string, std::string> MapInArgs_t;

typedef struct InputValues_s
{
	std::array<int16_t,2> intervalGpsOff;
	uint32_t quantFactor;
	uint8_t fsImu, fsGps;
	double tau;
	double heightVal;
	bool inputAnglesInRadians;
	bool correctForGravity;
	bool doPlatformAlignment;
	bool modeMechanicsLocal;
	bool progressAngles;
	bool feedbackBias;
	arma::vec attitudeSelector, bodySelector;
	arma::vec diagPlat2Body;
	arma::vec accRest, gyrRest;
	std::string kfStdCfg;
	std::string outputDir;
}InputValues_t;

/* Classes */

/*! 
 @brief Class stores the indexes corresponding to the CSV columns where data is stored.
 \class InputIds
*/
class InputIds {
public:
	InputIds() {
		GPS.fill(-1);
		ACC.fill(-1);
		GYR.fill(-1);
		MAG.fill(-1);
		ROLL = PITCH = YAW = TIMESTAMP = HDOP = HEIGHT = -1;
	}

	std::array<int8_t, 2> GPS;
	std::array<int8_t, 3> ACC, GYR, MAG;
	int8_t ROLL, PITCH, YAW, TIMESTAMP, HDOP, HEIGHT;
};


/*!
 @brief Class to handle the input command line
 @class InputCmdLine
 */
class InputCmdLine {
public:
	/*! Singleton class, function returns static object from private constructor */
	static InputCmdLine& getInstance(void);
	/*! Singleton class, delete copy constructor */
	InputCmdLine& operator=(const InputCmdLine&) = delete;
	~InputCmdLine() {};
	
	/*!
	@brief Check if the required argument is within the entered arguments
	@param inputCmdLineStr: Input comand line to be parsed to the map.
	@param cmdArgRequiredEntered: Bool indicating whether the input arguments are valid or not.
	*/
	void checkInputCmdLine(const std::vector<std::string> inputCmdLineStr, bool* cmdArgRequiredEntered);

	/*!
	@brief Function for reading the input command line parameters
	@param inputCmdLineStr: Input comand line to be parsed to the map.
	@param mapInputArgs: Output map with input arguments.
	*/
	void readInputCmdLine(const std::vector<std::string> inputCmdLineStr, MapInArgs_t& mapInputArgs);

private:
	// Private constructor
	InputCmdLine() : numInputArgs(0) {};

	// Variables
	int numInputArgs;
};

/*! 
 @brief Class to handle the Input Interface with the user. In charge of managing handling the command line and parsing into an internal map.
 Mainly store the filenames, config input variables used in navigation data, described in InputValues_t, and columns descrived in InputIds to  be used in NavDataInterface.
 */
class UI {
public:
	/*! Singleton class, function returns static object from private constructor */
	static UI& getInstance(void);
	/*! Singleton class, delete copy constructor */
	UI& operator=(const UI&) = delete;
	~UI(){};

	/*!
	@brief UI starting point. In charge of loading the default parameters and reading the input command line to the input argument map
	@param argc_ Number of input arguments.
	@param argv_ Array of input arguments.
	*/
	void start(const int argc_, char* argv_[]);

	/*! Call method for function to get the mapInputArgs variable */
	MapInArgs_t getMapInputArgs(void);

	/*! Explain how to use the software */
	static void ui_usage(void);

	/*! Loops through the map containing the input command line arguments, and fill the internal data before starting processing */
	void loadParams();

	/* Getters */
	/*! Get the IDs (= CSV column indexes) of the data in the CSV entered in command line */
	InputIds& getInputIds(void);
	
	/*! Get the input values entered in the command line, these are values which are not read from the CSV but directly entered by the user 
	These values are then copied to NavDatInterface clasas to centralize all navigation-related data from there.
	*/
	const InputValues_t& getInputValues(void) const;
private:
	// Private constructor
	UI(void){};

	// Private variables
	int argc;
	char** argv;
	MapInArgs_t mapInputArgs; // Variable to hold the input command parameters.
	InputCmdLine& cInputCmdLine = InputCmdLine::getInstance();
	std::vector<std::string> inputCmdLineStr;
	InputIds cInputIds;
	InputValues_t sInputValues;

	// Private functions
	
	// Convert input command line, in char(*)[argc], to string array
	void convertInputCmdLineToString(void);
	// Load the default configuration, called before reading the input command, access to private members is done directly and not through setters declared above
	void loadDefaultValues(void);
	// Call method for function to handle the input command line parameters
	void handleInputCmdLine(void);
};

#endif // _HEADER_UI_
