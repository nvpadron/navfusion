/*! 
 @file main.cpp
 @author Nicolas Padron
 @brief Description: Main file.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <armadillo>

#include <general/general.h>
#include <monitor/monitor.h>
#include <interface/ui/ui.h>
#include <interface/io/in/io_in.h>
#include <interface/io/out/io_out.h>
#include <interface/navdata/interface_navdata.h>


Monitor& cMonitor = Monitor::getInstance();
NavDataInterface& cInterfaceNavdata = NavDataInterface::getInstance();

int main(int argc, char* argv[])
{
	updateDisplayOutputConsoleCpp("SOFTWARE STARTED", true);

    /* OBJECT CREATION */
	UI& ui = UI::getInstance();
	Input& cInput = Input::getInstance();
	Output_c& cOutputInterface = Output_c::getInstance();
	Systems& cSystems = Systems::getInstance();

   // Read inputs from cmd line, parse into structs and initialize Systems.
   try {
	   ui.start(argc, argv);

	   /* Start the Input Interface:
	   * - Load the default user-entered raw parameters, read from the command line and not from the CSV.
	   * - Read the input parameters from the command line. Here the default parameters will be updated from the user-entered command line.
	   */
	   ui.loadParams();

	   /* Open INPUT file to read and OUTPUT file to create. */
	   cInput.openIOFiles();

	   /*
	   * - Analysis file with each module results (CSV file)
	   * - Google Earth (KML files)
	   */
	   cOutputInterface.writeHeaders();

	   /* Initialize input values */
	   cInterfaceNavdata.initialize();

   		/* Initialize systems */
   		cSystems.initialize();

		/* Jump 1st row of data
		* This is done because the 1st row in the input data file is the column description:
		* row 1: timeStamp,accX,accY,...
		* * row 2: 0,0.05,0.09,...
		*/
		cInput.readline();

   }
   catch (const MonitorException& monExc)
   {
	   cMonitor.exitCode(monExc);
	   cInput.closeFiles();
	   return cMonitor.getExitCode();
   }
   catch (...)
   {
	   cMonitor.exitCode(MonitorException(ERROR_RETURN_UNKNOWN));
	   cInput.closeFiles();
	   return cMonitor.getExitCode();
   }
  
  /* Start loop processing */
  updateDisplayOutputConsoleCpp("PROCESSING STARTING", true);
  
  /* Loop along the file */
  while (cInput.readline(false, cInterfaceNavdata.getEpochCounter()))  /* Read the row and put into Fields. */
  {
	/* Update monitor, not part of processing but contorls when to show display information */
	cMonitor.update();

	/* Update the input navdata interface with the inputs read at every epoch */
	try
	{
		cInterfaceNavdata.update();
	}
   catch (const MonitorException& monExc)
   {
	   cMonitor.exitCode(monExc);
	   cInput.closeFiles();
	   return cMonitor.getExitCode();
   }
   catch (...)
   {
	   cMonitor.exitCode(MonitorException(ERROR_RETURN_UNKNOWN));
	   cInput.closeFiles();
	   return cMonitor.getExitCode();
   }
   

	/* Process systems: GNSS, INS and FUSION */
	cSystems.process();

	/* Write output files */
	cOutputInterface.writeContent();
	
	// Display some results on screen
	if (cMonitor.flagsMonitorVariables_e.test(MON_DISPLAY_DATA_CHECK))
	{
		// Variable to hold the percentage of bytes read on the input file.
		double ReadBytesPercentage = (double)cInput.cFilesHandler.at(FILE_INPUT).getReadBytes() / cInput.cFilesHandler.at(FILE_INPUT).getFileSize();
		updateProgressBarCpp((int)(ReadBytesPercentage * 100));
	}
 }
	
	/* Write eKML output footer and close input files */
	cOutputInterface.kmlWriteFooter();
	updateDisplayOutputConsoleCpp("PROCESSING COMPLETED!", true);
	cInput.closeFiles();
 
	return cMonitor.getExitCode();
}

