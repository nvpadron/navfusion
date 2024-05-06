/*! 
 @file io_files.h
 @author Nicolas Padron
 */

#ifndef _HEADER_IO_FILES_
#define _HEADER_IO_FILES_

#include <iostream>
#include <fstream>
#include <string>

enum FstreamOption_e {
	FSTREAM_IN,
	FSTREAM_OUT
};

enum IoFilesAction_e{
	FILE_ACT_INIT,
	FILE_ACT_OPEN,
	FILE_ACT_EOF,
	FILE_ACT_CLOSED,
	FILE_ACT_READ,
	FILE_ACT_WRITTEN
};

/*!
 \class FileHandler
 @brief Class to handle files for read/write
*/
class FileHandler {
public:
	/*! Default constructor */
	FileHandler()
	{
		sizeFile = -1;
		fileLastAction = FILE_ACT_INIT;
		openOption = FSTREAM_OUT;
	};
	~FileHandler(){};

	/*! Open individual file with status checkings */
	bool openFile(void);
	/*! Close individual fole with status checkings */
	bool closeFile(void);
	/*! Read file line */
	int readLine(std::string& line);
	/*! Write into file */
	int writeContent(const char* str);
	/*! Set open mode */
	void setOpenOption(int opt);
	/*! Set filename */
	void setFilename(const std::string& filename_);
	/*! Get filename */
	const std::string& getFilename(void) const;
	/*! Get file size in bytes */
	long getFileSize(void);
	/*! Get number of bytes already read */
	long getReadBytes(void);
	/*! Get file last action */
	const IoFilesAction_e getFileLastAction(void);
private:
	long sizeFile;
	std::string filename;
	std::fstream fs;
	IoFilesAction_e fileLastAction;
	int openOption;
};
#endif// _HEADER_IO_FILES_
