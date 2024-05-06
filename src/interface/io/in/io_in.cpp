/*!
 @file io_in.h
 @author Nicolas Padron
 @brief File for performing the Input Module tasks.
 	- Responsible for reading the user command line parameters.
	- Responsible for reading the sensor data from the input file.
*/

#include <monitor/monitor.h>
#include <interface/ui/ui.h>
#include <interface/io/in/io_in.h>

#include <string.h>

using namespace std;

/* Class Instance Generation */
Input& Input::getInstance(void)
{
	static Input instance;
	return instance;
}

/* Close all opened files */
void Input::closeFiles(void)
{
	try {
		for (auto& fileHandler : cFilesHandler)
		{
			if (!fileHandler.closeFile())
			{
				string msg = "File: "; msg += fileHandler.getFilename(); msg += " cannot be closed.";
				updateDisplayOutputConsoleCpp(msg, true);
				throw MonitorException(ERROR_RETURN_FILE_CLOSE_ERROR);
			}
		}
	}
	catch (const MonitorException& monExc)
	{
		cMonitor.exitCode(monExc);
	}
	catch (...)
	{
		cMonitor.exitCode(MonitorException(ERROR_RETURN_UNKNOWN));
	}
}

/* Open all files (Input & Output) */
void Input::openIOFiles(void)
{

	// Open file
	try {
		for (int fileIndex : vector<int>{ FILE_INPUT, FILE_OUTPUT, FILE_OUTPUT_KML_GPS, FILE_OUTPUT_KML_INS, FILE_OUTPUT_KML_FUSION })
		{
			if (!cFilesHandler.at(fileIndex).openFile())
			{
				string msg = "File: "; msg += cFilesHandler.at(fileIndex).getFilename(); msg += " cannot be opened.";
				updateDisplayOutputConsoleCpp(msg, true);
				throw MonitorException(ERROR_RETURN_FILE_OPEN_ERROR);
			}
		}

		// Calculate size of input file
		(void)cFilesHandler.at(FILE_INPUT).getFileSize();
	}
	catch (const MonitorException&)
	{
		throw;
	}
}

/* Remove white space on string, needed for UI */
const string Input::removeStartingWhiteSpace(const string filename)
{
	int index = 0;
	const char* pFilename = filename.c_str();
	for (index = 0; index < strlen(pFilename); index++)
	{
		if (' ' != pFilename[index])
		{
			break;
		}
	}
	return string(&pFilename[index]);
}

/* Function to read and write the IDs of the input CSV file */
void Input::readInputCsvIds(void)
{
	// Open input file
	if (!cFilesHandler.at(FILE_INPUT).openFile())
	{
		ostringstream msg;
		msg << "Error in opening file: " << cFilesHandler.at(FILE_INPUT).getFilename() << endl;
		updateDisplayOutputConsoleCpp(msg.str(),true);
		throw MonitorException(ERROR_RETURN_FILE_OPEN_ERROR);
	}

	// Construct filename string to save the IDs and open the file
	string inputFilename = cFilesHandler.at(FILE_INPUT).getFilename();
	cFilesHandler.at(FILE_INPUT_CSVIDS).setFilename(inputFilename.substr(0, inputFilename.length() - 4) + "_INDEX.txt");
	if (!cFilesHandler.at(FILE_INPUT_CSVIDS).openFile())
	{
		ostringstream msg;
		msg << "Error in opening file: " << cFilesHandler.at(FILE_INPUT_CSVIDS).getFilename() << endl;
		updateDisplayOutputConsoleCpp(msg.str(),true);
		throw MonitorException(ERROR_RETURN_FILE_OPEN_ERROR);
	}

	// Read line and write the Index file.
	try {
		// Read the row, which in this case is the 1st which contains the field names, i.e., column names.
		if (!readline())
		{
			stringstream msg;
			msg << "Error in reading content from file: " << cFilesHandler.at(FILE_INPUT).getFilename() << endl;
			updateDisplayOutputConsoleCpp(msg.str(), true);
			throw MonitorException(ERROR_RETURN_FILE_READ_ERROR);
		}

		// Create string with IDs to write
		ostringstream idStr;
		idStr << "Index of Elements: " << endl;
		for (auto it = mapData.begin(); it != mapData.end(); it++)
		{
			idStr << "INDEX:\t" << std::to_string(it->first) << "\t-\t" << it->second.fieldname << endl; // Construct string with IDs, e.g.: "ID: 3 - accX"
		}
		// Write IDs
		(void)cFilesHandler.at(FILE_INPUT_CSVIDS).writeContent(idStr.str().c_str()); // Write IDs
		if (FILE_ACT_WRITTEN != cFilesHandler.at(FILE_INPUT_CSVIDS).getFileLastAction())
		{
			ostringstream msg;
			msg << "Error in writing content on file: " << cFilesHandler.at(FILE_INPUT_CSVIDS).getFilename() << endl;
			updateDisplayOutputConsoleCpp(msg.str(), true);
			throw MonitorException(ERROR_RETURN_FILE_WRITE_ERROR);
		}
		// Close file which holds the IDs
		cFilesHandler.at(FILE_INPUT_CSVIDS).closeFile();
	}
	catch (const MonitorException&)
	{
		throw;
	}

	// Close files
	if (!cFilesHandler.at(FILE_INPUT).closeFile() || !cFilesHandler.at(FILE_INPUT_CSVIDS).closeFile())
	{
		throw MonitorException(ERROR_RETURN_FILE_CLOSE_ERROR);
	}

};

/* Read CSV line */
bool Input::readline(bool jumpLine, int epochCounter)
{
	bool lineReadCorrectly = false;
	char* eptr;

	// Read line
	string readLineStr;
	IoFilesAction_e fileLastAction;
	cFilesHandler.at(FILE_INPUT).readLine(readLineStr);
	if (!(readLineStr.length() > 0))
	{
		lineReadCorrectly = false;
		updateDisplayOutputConsoleCpp("Empty line found on input CSV file.", true);
		if(epochCounter == 0)
		{
			throw MonitorException(ERROR_RETURN_FILE_READ_ERROR);
		}
	}
	fileLastAction = cFilesHandler.at(FILE_INPUT).getFileLastAction();
	if (FILE_ACT_READ == fileLastAction)
	{
		lineReadCorrectly = true;
	}
	else if(FILE_ACT_EOF == fileLastAction)
	{
		lineReadCorrectly = false;
		return lineReadCorrectly;
	}
	else
	{
		lineReadCorrectly = false;
		throw MonitorException(ERROR_RETURN_FILE_READ_ERROR);
	}

	if (jumpLine)
	{
		return lineReadCorrectly;
	}

	// Fill fieldvalue
	size_t delimPos = 0;
	string field;
	int fieldId = 0;
	// Since comparing against npos, can be reseted back to 0 if is not handled correctly, the following variable acts as sanity check for that.
	try {
		while (delimPos != readLineStr.npos)
		{
			// Look for when a ',' is found, this delimits the end of the field to read
			do{
				delimPos = readLineStr.find(",");
			}while (delimPos == 0);
			field = readLineStr.substr(0, delimPos); // Read field goes from start of string (index 0) until index where the ',' was found

			// Fill map at key = fieldcount with the value = field read
			// The map contans, at each index, a pair of <fieldname, fieldvalue> e.g. <"latitude", 71.34>
			if(field.length() > 0)
			{
				auto it = mapData.find(fieldId);
				if (false == isFieldnameSet) // The 1st line always contains the fieldname, and subsequent lines the fieldvalue, so if fieldname is not yet set, the 1st thing to do is to set the map with the corresponding fieldnames read.
				{
						mapData.insert({ fieldId, InputCsvFields(field, 0) });
				}
				else // If fieldname is already set, then what is read from the CSV line is the fieldvalue, so we can introduce it as a double.
				{
					try
					{
						it->second.fieldvalue = strtod(field.c_str(), &eptr);
					}
					catch(...)
					{
						lineReadCorrectly = false;
						throw MonitorException(ERROR_RETURN_FILE_READ_ERROR);
					}
				}
				readLineStr = readLineStr.substr(delimPos + 1);
				fieldId++;
			}
		}
	}
	catch (...)
	{
		lineReadCorrectly = false;
		throw MonitorException(ERROR_RETURN_FILE_READ_ERROR);
	}

	// Ideally, the 1st time this function is called, the pair <fieldname,fieldvalue = 0> should be filled, and then only the fieldvalue should be updated.
	if (false == isFieldnameSet)
	{
		isFieldnameSet = true;
	}


	return lineReadCorrectly;
}

/* Get fieldvalue for corresponding field ID */
const double Input::getFieldvalue(int id)
{
	double ret = 0;
	try {
		auto it = mapData.find(id);
		if (it != mapData.end())
		{
			ret = it->second.fieldvalue; // Need to check for the range
		}
		else
		{
			updateDisplayOutputConsoleCpp("Out of range in fieldvalue when trying to read index from CSV. Check if CSV indexes (columns) are correct.", true);
			throw MonitorException(ERROR_RETURN_OUT_RANGE);
		}
	}
	catch (const MonitorException&)
	{
		throw;
	}
	return ret;
}
