
from helpers import *
    
# Read Input
filename = r'D:\Nico\Lab\Software\development\Navigation\navfusion\data\tram\input\tram.csv'
csvHandler = HandlerCSV(',','.',filename)
dfIn = csvHandler.read()

# Read Output
filename = r'D:\Nico\Lab\Software\development\Navigation\navfusion\data\tram\yaw\output.csv'
csvHandler = HandlerCSV(',','.',filename)
dfOut = csvHandler.read()

# Quick look on data
pwinIn = dfLook(dfIn)
pwinOut = dfLook(dfOut)

# Plot comparison between INS and Fusion
# Initialize plot window
pwin = plotWindow()
pwin = addPlotCompare(dfOut, 'ROLL', pwin)
pwin = addPlotCompare(dfOut, 'PITCH', pwin)
pwin = addPlotCompare(dfOut, 'YAW', pwin)
pwin = addPlotCompare(dfOut, 'V', pwin)

plt.show()
input("Press Enter to close.")
print("End")
