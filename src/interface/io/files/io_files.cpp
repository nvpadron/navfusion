/*! 
 @file io_files.cpp
 @author Nicolas Padron
 @brief: File for handling the reading/writing of files.
*/

#include <interface/io/files/io_files.h>
using namespace std;

/* Get last action that took place in the file */
const IoFilesAction_e FileHandler::getFileLastAction()
{
	return fileLastAction;
}

/* Open file */
bool FileHandler::openFile(void)
{
	// Open if not open already
	if (!fs.is_open())
	{
		fs.open(filename, (openOption == FSTREAM_IN) ? std::fstream::in : std::fstream::out);
		fileLastAction = FILE_ACT_OPEN;
	}

	// return if it opened or not
	return fs.is_open();
}

/* Set file option to open */
void FileHandler::setOpenOption(int opt)
{
	openOption = opt;
}

/* Set filename */
void FileHandler::setFilename(const std::string& filename_)
{
	filename = filename_;
}

/* Close file */
bool FileHandler::closeFile(void)
{
	if (fs.is_open())
	{
		fs.close();
		fileLastAction = FILE_ACT_CLOSED;
	}
	return !fs.is_open();
}

/* Read file line */
int FileHandler::readLine(string& line)
{
	if (fs.is_open())
	{
		if (!fs.eof())
		{
			std::getline(fs, line);
			fileLastAction = FILE_ACT_READ;
		}
		else
		{
			fileLastAction = FILE_ACT_EOF;
		}
	}
	return fileLastAction;
}

/* Get filename */
const std::string& FileHandler::getFilename(void) const
{
	return filename;
}

/* Get file size */
long FileHandler::getFileSize(void)
{
	if (fs.is_open() && sizeFile == -1)
	{
		// Look for end of the file
		fs.seekg(0, ios::end);
		// Count number of bytes
		sizeFile = (long)fs.tellg();
		// Go back to starting point of the file
		fs.clear();
		fs.seekg(0, ios::beg);
		sizeFile -= (long)fs.tellg();
	}
	return sizeFile;
}

/* Calculate how many bytes have bean read so far */
long FileHandler::getReadBytes(void)
{
	long readBytes = 0;
	if (fs.is_open())
	{
		readBytes = (long)fs.tellg();
	}
	return readBytes;
}

/* Write conent in file */
int FileHandler::writeContent(const char* str)
{
	if (fs.is_open())
	{
		if (!fs.eof())
		{
			fs.write(str, string(str).size());
			fileLastAction = FILE_ACT_WRITTEN;
		}
	}
	return fileLastAction;
}
