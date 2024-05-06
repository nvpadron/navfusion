/*!
 @file io_in.h
 @author Nicolas Padron
 @brief File to handle the input files opening and reading.
 */

#ifndef _HEADER_IO_IN_
#define _HEADER_IO_IN_

#include <unordered_map>
#include <string>
#include <array>
#include <interface/io/files/io_files.h>

/* Types of files to handle (open, read/write, close): input file, output file, google earth */
enum FileTypes_e {
	FILE_INPUT,
	FILE_INPUT_CSVIDS,
	FILE_OUTPUT,
	FILE_OUTPUT_KML_GPS,
	FILE_OUTPUT_KML_INS,
	FILE_OUTPUT_KML_FUSION,
	FILE_TOTAL
};

/*!
 @brief Class to handle the Input CSV fields
 */
class InputCsvFields{
public:
	/*! Default constructor */
	InputCsvFields()
	{
		fieldvalue = 0;
	};
	/*! Constructor used when inserting the CSV into the input map in Input::readLine */
	InputCsvFields(std::string fieldname_, double fieldvalue_) : fieldname(fieldname_), fieldvalue(fieldvalue_) {};

	/*! Variable to hold field name in input CSV */
	std::string fieldname;

	/*! Variable to hold field value in input CSV */
	double fieldvalue;
};

/*!
 @brief Read/Write Files Interface class
*/
class Input {
public:
	Input(const Input&) = delete;
	Input operator=(const Input&) = delete;
	~Input() {};
	/*! Singleton class, function returns static object from private constructor */
	static Input& getInstance(void);

	/*! Open files entered as Input */
	void openIOFiles(void);
	/*! Close files */
	void closeFiles(void);
	/*! Remove white space on string, needed for UI */
	static const string removeStartingWhiteSpace(const string filename);
	/*! Read input line */
	bool readline(bool jumpLine = false, int epochCounter = 0);
	/*! Function to read the IDs of the input CSV file */
	void readInputCsvIds(void);	
	/*! Get the fieldvalues */
	const double getFieldvalue(int id);

	// Handle the files
	std::array<FileHandler, FILE_TOTAL> cFilesHandler;

private:
	Input()
	{
		totalfields = 0;
		isFieldnameSet = false;
		cFilesHandler.at(FILE_INPUT).setOpenOption(FSTREAM_IN);
	};
	int totalfields;
	bool isFieldnameSet;
	std::unordered_map<int, InputCsvFields> mapData;
};


#endif // !_HEADER_IO_IN_
