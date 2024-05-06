/*! 
 @file ui.h
 @author Nicolas Padron
 @brief Description:  In this file the processes of ui.h are implemented.
*/

#include <general/general.h>
#include <monitor/monitor.h>
#include <interface/ui/ui.h>

#ifndef WFUI_INTERFACE

/* Convert std::array to arma::vec */
template<class type_t, std::size_t nsize>
static arma::vec stdArray3ToArmaVec(std::array<type_t, nsize> arr)
{
    arma::vec vec = arma::zeros(nsize, 1);
    uint32_t idx = 0;
    for (auto it = arr.begin(); it != arr.end(); it++)
    {
        vec.at(idx) = *it;
        idx++;
    }
    return vec;
}
template arma::vec stdArray3ToArmaVec<uint8_t, 3U>(std::array<uint8_t, 3U> arr);
template arma::vec stdArray3ToArmaVec<double, 3U>(std::array<double, 3U> arr);


/* Convert string vector to std::array */
template <typename InType, size_t arrSize>
 static void strvecToArray(const string& str, array<InType, arrSize> &arr)
{
	InType elem = 0;
	uint16_t counter = 0;
	size_t prevPos, nextPos;
	prevPos = nextPos = 0;
	string substr;
	while (nextPos != str.npos)
	{
		nextPos = str.find(",", prevPos);
		substr = str.substr(prevPos, nextPos - prevPos);
		prevPos = nextPos + 1;

		elem = (InType)atof(substr.c_str());
		arr.at(counter++) = elem;
	}
}
template void strvecToArray<int, 2>(const string& str, array<int, 2>& arr);
template void strvecToArray<uint16_t, 3>(const string& str, array<uint16_t, 3>& arr);
template void strvecToArray<double, 3>(const string& str, array<double, 3>& arr);

/* Check if input is within the acceptable range */
static int checkInputScalar(const int input, const int inMin, const int inMax, string msg)
{
	int ret = ERROR_RETURN_NOERROR;
	bool check = (input >= inMin) && (input <= inMax);
	if (!check)
	{
		updateDisplayOutputConsoleCpp(msg.append(": value entered out of range").c_str(), true);
		ret = ERROR_RETURN_OUT_RANGE;
	}
	return ret;
}

/*! Display progress percentage */
void updateProgressBarCpp(int value)
{
	cout << "Processing completed: " << value << "%" << endl;
}

/*! Display output console */
void updateDisplayOutputConsoleCpp(const std::string str, const bool forceDisplay)
{
	if (cMonitor.flagsMonitorVariables_e.test(MON_DISPLAY_DATA_CHECK) || forceDisplay)
	{
		time_t now = time(0);
		string tmpnowstr = string(ctime(&now));
		cout << "[" << tmpnowstr.substr(0, tmpnowstr.length() - 1) << "] - " << str << endl;
	}
}

#endif // WFUI_INTERFACE

// Singleton class, return static constructor
InputCmdLine& InputCmdLine::getInstance(void)
{
	static InputCmdLine instance;
	return instance;
}

// Check if the required argument is within the entered arguments
void InputCmdLine::checkInputCmdLine(const std::vector<std::string> inputCmdLineStr, bool* cmdArgRequiredEntered)
{
	// Consistency Check
	bool check = (inputCmdLineStr.size() <= INPUT_ARGS_NUM + 1);
	if (!check)
	{
		updateDisplayOutputConsoleCpp("ERROR: the count of args is bigger than the max number of accepted input cmd line args", true);
		throw MonitorException(ERROR_RETURN_INCONSISTENT_INPUTS);
	}

	string cmdArgLabel;
	try {
		for (auto str : inputCmdLineStr)
		{
			auto barpos = str.substr(0, 1).find("-");
			if (barpos != str.npos)
			{
				auto barpossub = str.substr(barpos + 1, 1).find("-");
				if (barpossub != str.npos)
				{
					barpossub += 1; // To consider also that we have jumped 1 position, since the previous line is refernced to the position we have jumped counting it as the 0 for that substring.
					cmdArgLabel = str.substr(barpossub + 1);
				}
				else
				{
					cmdArgLabel = str.substr(barpos + 1);
					// Look for accepted commands
					if (INPUT_ARGS_LABELS.end() != std::find(INPUT_ARGS_LABELS.begin(), INPUT_ARGS_LABELS.end(), *cmdArgLabel.c_str()))
					{
						// Look for required command
						if (INPUT_ARGS_INFILE == *cmdArgLabel.c_str())
						{
							*cmdArgRequiredEntered = true;
						}
						else if (INPUT_ARGS_HELP == *cmdArgLabel.c_str())
						{
							throw MonitorException(ERROR_RETURN_HELPCMD);
						}
					}
				}
			}
		}
	}
	catch (const MonitorException&)
	{
		UI::ui_usage();
		throw;
	}
}

// Read input command line
void InputCmdLine::readInputCmdLine(const std::vector<std::string> inputCmdLineStr, MapInArgs_t& mapInputArgs)
{
	string cmdArg;
	string cmdArgLabel;
	for (auto str : inputCmdLineStr)
	{
		auto barpos = str.substr(0,1).find("-");
		if (barpos != str.npos)
		{
			auto barpossub = str.substr(barpos + 1,1).find("-");
			if (barpossub != str.npos)
			{
				barpossub += 1; // To consider also that we have jumped 1 position, since the previous line is refernced to the position we have jumped counting it as the 0 for that substring.
				cmdArgLabel = str.substr(barpossub + 1);
				cmdArg = "";
			}
			else
			{
				/* map stores, i.e.: <"-f", "argLabelCmdfilename"> */
				cmdArgLabel = str.substr(barpos + 1, 1);
				// Next position after the '-' ------------^
				cmdArg = str.substr(str.substr(barpos + 1).find(" ", barpos + 1) + 1);
			}
			// Next after the " ", after the cmdArgLabel offset ---^
			MapInArgs_t::iterator it = mapInputArgs.find(cmdArgLabel);
			if (it == mapInputArgs.end())
			{
				mapInputArgs.insert(std::pair<string, string>(cmdArgLabel, cmdArg));
			}
			else
			{
				it->second = cmdArg;
			}
		}
	}
}

// Singleton class, return static constructor
UI& UI::getInstance(void)
{
	static UI instance;
	return instance;
}

// Display usage of software.
void UI::ui_usage(void)
{
	fprintf(stderr, 
		"******************************************\n"
		"*  SENSOR FUSION: GPS + INERTIAL SENSORS *\n"
		"*  Nicolas Padron. Copyright 2024.       *\n"
		"*  spacewayfinder.com                    *\n"
		"******************************************\n"
		"\n"
		"Commands ( * = mandatory ):\n"
		"  -?     HELP, show this menu again\n"
		"  --idx  If this flag is entered, the software will read the input CSV file and write a .txt indicating each column number. Program finishes after this.\n"
		"  -I *   Input CSV file. NOTE: must be comma separated, not Excel type.\n"
		"  -O *   Output directory\n"
		"  -K *   Contains Process Noise and Measurement Noises in the order: [1x3 acc bias, 1x3 gyr bias, 1x3 acc drift bias, 1x3 gyr drift bias, 1x3 GPS DOPs].\n"
		"  -F *   Sampling Rate in Hz, order as \"fs_imu, fs_gps\"\n"
		"  -A *   CSV Column for ACCELEROMETER\n"
		"  -W *   CSV Column for GYROMETER\n"
		"  -M     CSV Column for MAGNETOMETER\n"
		"  -C *   CSV Column for GPS LAT & LON (degrees)\n"
		"  -H *   CSV Column for HEIGHT (meters). If not entered, then -h must be specified.\n"
		"  -h     Fix height value if not part of index CSV. Default is 100.\n"
		"  -R     CSV Column for ROLL. If not entered, it is calculated from accelerometer.\n"
		"  -P     CSV Column for PITCH. If not entered, it is calculated from accelerometer.\n"
		"  -Y *   CSV Column for YAW. If not entered, it is calculated from accelerometer and magnetometer (-M must be set).\n"
		"\n"
		"Other optional parameters:\n"
		"  -a     Accelerometer values in rest, this will be subtracted to the CSV read values. Enter as \"x,y,z\".\n" 
		"         Subtraction happens in Platform frame (input frame). Default is 1x3 0s.\n"
		"  -w     Gyrometer values in rest, this will be subtracted to the CSV read values. Enter as \"x,y,z\".\n" 
		"         Subtraction happens in Platform frame (input frame). Default is 1x3 0s.\n"
		"  -p     3x3 Platform-to-Body rotation matrix. Enter as 9x1 array, the program will reshape. Enter as \"a,b,c,...,h,i\"\n"
		"         Recall Body frame used in software. Default is identity.\n"
		"  -x     1x3 array of 0s or 1s to filter (from right to left): \"roll, pitch, yaw\". Default is \"0,0,1\".\n"
		"  -z     1x3 array of 0s or 1s to filter (from right to left): \"x, y, z\" axes in Body frame. Default is \"1,0,0\".\n"
		"  -r     Entered attitude angles in Radians (set to 1) or degrees (set to 0). Default is 1.\n"
		"  -l     Perform platform alignment to horizontal plane, depends on roll and pitch. Align (set to 1), not align (set to 0). Default is 0.\n"
		"  -f     Feedback Kalman Filter estimated accelerometer and gyrometer biases into IMU measurements to compensate. Enable (set to 1), disable (set to 0). Default is 0.\n"
		"  -m     Set to 1 to perform mechanization of velocity in local plane, set to 0 to do in body frame.  Default is 0.\n"
		"  -g     Gravity correction for Z component: correct = 1, do not correct = 0\n"
		"  -y     Progress attitude angles or take input ones. Set to 1 to 1st use attitude angles from entered (CSV indexes) or calculated (from acc, gyr, mag) \n"
	    "         and then continues with Euler derivatives. Set to 0 to use entered/calculated all the time, no euler derivatives used. Default is 1.\n"
		"  -t     Correlation time to be used in State Transition Matrix 1st order Markov processes for accelerometer and gyrometer drift. Default is 1.\n"
		"  -T     Interval in seconds to turn GPS off in GPS-INS fusion. Enter as \"min,max\" both > 0. Default is \"-1,-1\" which means don't turn off.\n"
		"  -q     Quantization factor to apply to input IMU values to remove small variations. Criteria is floor(x * QF) / QF. Default is 10000.\n"
	);
}

// Call method for function to read the input command line parameters
void UI::handleInputCmdLine(void)
{
	bool cmdArgRequiredEntered = false;
	try
	{
		/*
		cInputCmdLine.checkInputCmdLine(inputCmdLineStr, &cmdArgRequiredEntered);
		if (false != cmdArgRequiredEntered)
		{
			cInputCmdLine.readInputCmdLine(inputCmdLineStr, mapInputArgs);
		}
		else
		{
			updateDisplayOutputConsoleCpp("ERROR: missing required input command (-i: input file)", true);
			ui_usage();
			throw MonitorException(ERROR_RETURN_INCONSISTENT_INPUTS);
		}
		*/
		cInputCmdLine.checkInputCmdLine(inputCmdLineStr, &cmdArgRequiredEntered);
		cInputCmdLine.readInputCmdLine(inputCmdLineStr, mapInputArgs);
		
		// Check if mandatory fields are entered:
		bool checkMandatory = 
		(mapInputArgs.find("I") != mapInputArgs.end()) && 
		(mapInputArgs.find("K") != mapInputArgs.end()) &&
		(mapInputArgs.find("O") != mapInputArgs.end()) &&
		(mapInputArgs.find("F") != mapInputArgs.end()) &&
		(mapInputArgs.find("A") != mapInputArgs.end()) &&
		(mapInputArgs.find("W") != mapInputArgs.end()) &&
		(mapInputArgs.find("C") != mapInputArgs.end()) &&
		((mapInputArgs.find("Y") != mapInputArgs.end()) || (mapInputArgs.find("M") != mapInputArgs.end())) &&
		((mapInputArgs.find("H") != mapInputArgs.end()) || (mapInputArgs.find("h") != mapInputArgs.end()));

		if (!checkMandatory)
		{
			updateDisplayOutputConsoleCpp("ERROR: one or more mandatory arguments are not present. Mandatory (I, K, O, F, A, W, C, H or h, and Y or M)", true);
			throw MonitorException(ERROR_RETURN_INCONSISTENT_INPUTS);
		}
		// Consistency check
		// Number of arguments is: program-name + (-cmdLabel1 cmdArg1) + (-cmdLabel2 cmdArg2) + ...
		//						         1              2        3             4        5
		bool checkSize = mapInputArgs.size() <= INPUT_ARGS_NUM + 1;
		if (!checkSize)
		{
			updateDisplayOutputConsoleCpp("ERROR: the count of args is bigger than the max number of accepted input cmd line args", true);
			throw MonitorException(ERROR_RETURN_INCONSISTENT_INPUTS);
		}

	}
	catch (const MonitorException&)
	{
		throw;
	}
}

// Function to convert input command line to string
void UI::convertInputCmdLineToString(void)
{
	inputCmdLineStr.clear();
	string str;
	//int argc = cMonitor.getProgInputArgs().argc;
	//char** argv = cMonitor.getProgInputArgs().argv;
	inputCmdLineStr.push_back(string(*argv));
	try {
		while (*++argv != nullptr && argc > inputCmdLineStr.size())
		{
			str = string(*argv);
			// Check if the string read is a valid
			bool isStrCmdArgValid, isStrCmdSubArgValid;
			isStrCmdArgValid = isStrCmdSubArgValid = false;
			bool isStrCmdArgLabel, isStrCmdArgSubLabel;
			isStrCmdArgLabel = isStrCmdArgSubLabel = false;
			// Check if the string read is within the input argument or subargument label lists predefined
			auto itIsStrLabel = str.substr(0, 1).find("-"); // Check if the string read is label or sublabel syntax
			if (itIsStrLabel != str.npos)
			{
				isStrCmdArgLabel = true;
				// Here we check if it is a label argument, i.e. start with "-" or subargument e.g., "--idx".
				auto itIsStrLabelSub = str.substr(itIsStrLabel + 1, 1).find("-");
				if(itIsStrLabelSub == str.npos)
				{ // Here we check if it is a label argument, i.e., the "i" on "-i" 
					auto itIsStrLabelWithinArgLabels = std::find(INPUT_ARGS_LABELS.begin(), INPUT_ARGS_LABELS.end(), *str.substr(itIsStrLabel + 1, 1).c_str());
					if (itIsStrLabelWithinArgLabels != INPUT_ARGS_LABELS.end())
					{ // Here we validate the label argument, i.e. "-i" will fail if "i" is not an accepted argument
						isStrCmdArgValid = true;
					}
					else
					{
						if (str.length() == 2) // This ensures that e.g. "-g -0.04, 0.05 0.02" ignores the "-0.04" as command and instead enters as argument of "-g".
						{
							throw MonitorException(ERROR_RETURN_INCONSISTENT_INPUTS);
						}
					}
				}
				else
				{ // Here we check if it is a label subargument, since it turns that it wasn�t a label argument, e.g., "--idx"
					itIsStrLabelSub += 1; // To consider also that we have jumped 1 position, since the previous line is refernced to the position we have jumped counting it as the 0 for that substring.
					if (itIsStrLabelSub != str.npos)
					{ // Here we check if check if the label subargument is a valid one within the accepted subarguments
						isStrCmdArgSubLabel = true;
						vector<string> subargsStringVector; // Since std::find will not work with N dimensional char arrays, e.g. std:array<char[N],num-elem>, then it is parsed to a std::vector<std::string>, we don't have this problem when we have a single char.
						for_each(INPUT_SUBARGS_LABELS.begin(), INPUT_SUBARGS_LABELS.end(), [&subargsStringVector](const char* chrArray)
							{
								subargsStringVector.push_back(string(chrArray));
							});
						auto itIsStrLabelSubWithinSubargLabels = std::find(subargsStringVector.begin(), subargsStringVector.end(), str.substr(itIsStrLabelSub + 1)); // Now we can use the std::find after handling a vector.
						if (itIsStrLabelSubWithinSubargLabels != subargsStringVector.end())
						{ // Here we valiate the label subargument
							isStrCmdSubArgValid = true;
						}
						else
						{
							throw MonitorException(ERROR_RETURN_INCONSISTENT_INPUTS);
						}
					}
				}
			}

			if (isStrCmdArgValid || isStrCmdSubArgValid) // Its a sublabel (complete), or label (complete): add new entry, e.g: "-i", "-i file.csv", "--index"
			{
				// the valid argLabel was entered, so advance one more
				inputCmdLineStr.push_back(str);
				str.clear();
			}
			else // Its an incomplete label, so append the 2nd part: "file.csv"
			{
				inputCmdLineStr.rbegin()->append(" ");
				inputCmdLineStr.rbegin()->append(str);
			}
		}
	}
	catch (const MonitorException&)
	{
		updateDisplayOutputConsoleCpp("Entered cmd arg or subarg label not valid.", true);
		throw;
	}
}

// Load default UI values
void UI::loadDefaultValues(void)
{
	inputCmdLineStr.push_back("-h 100"); 				// [meters]
	inputCmdLineStr.push_back("-F 100,1"); 				// [Hz]
	
	inputCmdLineStr.push_back("-a 0,0,0"); 				// {X, Y, Z}
	inputCmdLineStr.push_back("-w 0,0,0"); 				// {X, Y, Z}
	inputCmdLineStr.push_back("-p 1,0,0,0,1,0,0,0,1"); 	// {Roll, Pitch, Yaw}
	inputCmdLineStr.push_back("-z 0,0,1"); 				// {Roll, Pitch, Yaw}
	inputCmdLineStr.push_back("-x 1,0,0"); 				// {X, Y, Z}
	inputCmdLineStr.push_back("-r 1"); 					// [bool]

	inputCmdLineStr.push_back("-l 0"); 					// [bool]
	inputCmdLineStr.push_back("-f 0");					// [bool]
	inputCmdLineStr.push_back("-m 0"); 					// [bool]
	inputCmdLineStr.push_back("-g 0"); 					// [bool]
	inputCmdLineStr.push_back("-y 1");					// [bool]

	inputCmdLineStr.push_back("-t 1"); 					// [scalar]
	inputCmdLineStr.push_back("-T -1,-1"); 				// [s]
	inputCmdLineStr.push_back("-q 10000"); 				// [scalar]

	// Load default values
	cInputCmdLine.readInputCmdLine(inputCmdLineStr, mapInputArgs);
}

// Start UI, set default values and read command line
void UI::start(const int argc_, char* argv_[])
{
	// Set Input arguments
	argc = argc_;
	argv = argv_;
	// Load default configuration before reading all input cmd line
	loadDefaultValues();
	try {
		// Convert input command line, in char(*)[argc], to string array
		convertInputCmdLineToString();
		// Read all input cmd line
		handleInputCmdLine();
	}
	catch (const MonitorException&)
	{
		throw;
	}
}

// Get the map contianing the input arguments
MapInArgs_t UI::getMapInputArgs(void)
{
	return mapInputArgs;
}


// Get IDs of the input CSV file
InputIds& UI::getInputIds(void)
{
	return cInputIds;
}

// Get user entered input values
const InputValues_t& UI::getInputValues(void) const
{
	return sInputValues;
}

// Load the input parameters entered through the command line
void UI::loadParams()
{
	Input& cInput = Input::getInstance();
	int ret = ERROR_RETURN_NOERROR;

	// Read all input data from command line
	string cmdArgLabel;
	string cmdArg;
	string filename;
	bool flagIndexHandled, inputFilenameHandled;
	flagIndexHandled = inputFilenameHandled = false;
	try
	{
		for (auto mapEntry : mapInputArgs)
		{
			cmdArgLabel = mapEntry.first;
			cmdArg = mapEntry.second;
			
			if (INPUT_SUBARGS_LABELS.end() != std::find(INPUT_SUBARGS_LABELS.begin(), INPUT_SUBARGS_LABELS.end(), cmdArgLabel))
			{
				if (string(INPUT_SUBARGS_INDEX) == cmdArgLabel)
				{
					flagIndexHandled = true;
				}
			}
			else
			{
				switch (*cmdArgLabel.c_str())
				{
				case INPUT_ARGS_INFILE:
					cInput.cFilesHandler.at(FILE_INPUT).setFilename(Input::removeStartingWhiteSpace(cmdArg));
					inputFilenameHandled = true;
					break;
				case INPUT_ARGS_OUTFILE:
					sInputValues.outputDir = cmdArg.c_str();
					// Set output filename
					filename = Input::removeStartingWhiteSpace(sInputValues.outputDir + "/" + OUTPUT_FILENAME);
					cInput.cFilesHandler.at(FILE_OUTPUT).setFilename(filename);
					// Set KML filenames
					filename = Input::removeStartingWhiteSpace(sInputValues.outputDir + "/" + OUTPUT_FILENAME_GPS);
					cInput.cFilesHandler.at(FILE_OUTPUT_KML_GPS).setFilename(filename);
					filename = Input::removeStartingWhiteSpace(sInputValues.outputDir + "/" + OUTPUT_FILENAME_IRS);
					cInput.cFilesHandler.at(FILE_OUTPUT_KML_INS).setFilename(filename);
					filename = Input::removeStartingWhiteSpace(sInputValues.outputDir + "/" + OUTPUT_FILENAME_FUSION);
					cInput.cFilesHandler.at(FILE_OUTPUT_KML_FUSION).setFilename(filename);
					break;
				case INPUT_ARGS_INTERVAL_GPS_OFF:
					std::array<int16_t, 2> intervalGpsOff;
					strvecToArray(cmdArg, intervalGpsOff);
					sInputValues.intervalGpsOff = intervalGpsOff;
					break;
				case INPUT_ARGS_KFCFG:
					sInputValues.kfStdCfg = cmdArg;
					break;
				case INPUT_ARGS_TAU:
					sInputValues.tau = atof(cmdArg.c_str());
					break;
				case INPUT_QUANTIZATION_FACTOR:
					sInputValues.quantFactor = atoi(cmdArg.c_str());
					break;
				case INPUT_ARGS_HEIGHT_VAL:
					sInputValues.heightVal = atof(cmdArg.c_str());
					break;
				case INPUT_ARGS_FS:
					sInputValues.fsImu = atoi(cmdArg.substr(0, cmdArg.find(",")).c_str());
					sInputValues.fsGps = atoi(cmdArg.substr(cmdArg.find(",") + 1).c_str());
					break;
				case INPUT_ARGS_LATLON:
					strvecToArray(cmdArg, cInputIds.GPS);
					break;
				case INPUT_ARGS_ACC:
					strvecToArray(cmdArg, cInputIds.ACC);
					break;
				case INPUT_ARGS_ACC_REST:
					std::array<double, 3> accRest;
					strvecToArray(cmdArg, accRest);
					sInputValues.accRest = stdArray3ToArmaVec(accRest);
					break;
				case INPUT_ARGS_GYR:
					strvecToArray(cmdArg, cInputIds.GYR);
					break;
				case INPUT_ARGS_GYR_REST:
					std::array<double, 3> gyrRest;
					strvecToArray(cmdArg, gyrRest);
					sInputValues.gyrRest = stdArray3ToArmaVec(gyrRest);
					break;
				case INPUT_ARGS_MAG:
					strvecToArray(cmdArg, cInputIds.MAG);
					break;
				case INPUT_ARGS_ROLL:
					cInputIds.ROLL = atoi(cmdArg.c_str());
					break;
				case INPUT_ARGS_PITCH:
					cInputIds.PITCH = atoi(cmdArg.c_str());
					break;
				case INPUT_ARGS_YAW:
					cInputIds.YAW = atoi(cmdArg.c_str());
					break;
				case INPUT_ARGS_HEIGHT:
					cInputIds.HEIGHT = atoi(cmdArg.c_str());
					break;
				case INPUT_ARGS_ANGLES:
					sInputValues.inputAnglesInRadians = atoi(cmdArg.c_str());
					ret = checkInputScalar(sInputValues.inputAnglesInRadians, 0, 1, "Angles in Radians");
					sInputValues.inputAnglesInRadians = (bool)sInputValues.inputAnglesInRadians;
					break;
				case INPUT_ARGS_PROGRESS_ANGLES:
					sInputValues.progressAngles = atoi(cmdArg.c_str());
					ret = checkInputScalar(sInputValues.progressAngles, 0, 1, "Progress Angles");
					sInputValues.progressAngles = (bool)sInputValues.progressAngles;
					break;
				case INPUT_ARGS_GRAVITYCORR:
					sInputValues.correctForGravity = atoi(cmdArg.c_str());
					ret = checkInputScalar(sInputValues.correctForGravity, 0, 1, "Gravity Correction");
					sInputValues.correctForGravity = (bool)sInputValues.correctForGravity;
					break;
				case INPUT_ARGS_ALIGNMENT:
					sInputValues.doPlatformAlignment = atoi(cmdArg.c_str());
					ret = checkInputScalar(sInputValues.doPlatformAlignment, 0, 1, "Platform Alignment");
					sInputValues.doPlatformAlignment = (bool)sInputValues.doPlatformAlignment;
					break;
				case INPUT_ARGS_FEEDBACK_BIAS:
					sInputValues.feedbackBias = atoi(cmdArg.c_str());
					ret = checkInputScalar(sInputValues.feedbackBias, 0, 1, "Feedback Bias");
					sInputValues.feedbackBias = (bool)sInputValues.feedbackBias;
					break;
				case INPUT_MECHANICS_LOCAL:
					sInputValues.modeMechanicsLocal = atoi(cmdArg.c_str());
					ret = checkInputScalar(sInputValues.modeMechanicsLocal, 0, 1, "INS Mechanics in Local");
					sInputValues.modeMechanicsLocal = (bool)sInputValues.modeMechanicsLocal;
					break;
				case INPUT_ARGS_BODYSELECTION:
					std::array<uint16_t, 3> bodyArray;
					strvecToArray(cmdArg, bodyArray);
					sInputValues.bodySelector = stdArray3ToArmaVec(bodyArray);
					for (uint8_t elem : arma::conv_to<std::vector<uint8_t>>::from(sInputValues.bodySelector))
					{
						ret = checkInputScalar(elem, 0, 1, "Body Selection element");
						if (ret != ERROR_RETURN_NOERROR)
						{
							break;
						}
					}
					break;
				case INPUT_ARGS_ATTITUDESELECTION:
					std::array<uint16_t, 3> attitudeArray;
					strvecToArray(cmdArg, attitudeArray);
					sInputValues.attitudeSelector = stdArray3ToArmaVec(attitudeArray);
					for (uint8_t elem : arma::conv_to<std::vector<uint8_t>>::from(sInputValues.attitudeSelector))
					{
						ret = checkInputScalar(elem, 0, 1, "Attitude Selection element");
						if (ret != ERROR_RETURN_NOERROR)
						{
							break;
						}
					}
					break;
				case INPUT_ARGS_PLAT2BODY:
					std::array<int8_t, 9> p2bArray;
					strvecToArray(cmdArg, p2bArray);
					sInputValues.diagPlat2Body = stdArray3ToArmaVec(p2bArray);
					for (int8_t elem : arma::conv_to<std::vector<int8_t>>::from(sInputValues.diagPlat2Body))
					{
						ret = checkInputScalar(elem, -1, 1, "Platform to Body element");
						if (ret != ERROR_RETURN_NOERROR)
						{
							break;
						}
					}
					break;
				case INPUT_ARGS_HELP:
					ret = ERROR_RETURN_HELPCMD;
					break;
				default:
					UI::ui_usage();
					ret = ERROR_RETURN_HELPCMD;
					break;
				}
				if (ret != ERROR_RETURN_NOERROR)
				{
					throw MonitorException(ret);
				}
			}
		}

		// If flag --idx is set, then read the 1st CSV line and write the inputs
		if (flagIndexHandled && inputFilenameHandled)
		{
			cInput.readInputCsvIds();
			ret = ERROR_RETURN_IDX_HANDLED;
			updateDisplayOutputConsoleCpp("File with indexes written in input folder.", true);
		}

		// Return if there was an error in any of the called functions
		if (ret != ERROR_RETURN_NOERROR)
		{
			throw MonitorException(ret);
		}

	}
	catch (const MonitorException&)
	{
		throw;
	}
}
