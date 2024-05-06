from helpers import *
    
# Read Input
filename = r'D:\Nico\Lab\Software\development\Navigation\navfusion\data\inputs\rest.csv'

csvHandler = HandlerCSV(',','.', filename)
dfIn = csvHandler.read()

dfInStats = pd.DataFrame(columns=dfIn.columns)
dfInStats.loc[len(dfInStats)] = dfIn.mean()
dfInStats.loc[len(dfInStats)] = dfIn.std()
dfInStats.loc[len(dfInStats)] = dfIn.mean() + dfIn.std()

print(dfInStats)
dfInStats.to_csv(filename[:-4] + "_stats.csv", sep=';', decimal=',')
print('')
print(f'Written in CSV format in: {filename[:-4] + "_stats.csv"}')

