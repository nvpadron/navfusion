import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os

from third.plotWindow import plotWindow

cmds = {}

chars = {}
chars['INPUT_FILE']          = "-I"
chars['OUTPUT_FILE']         = "-O"
chars['FREQUENCY']           = "-F"      
chars['INPUTS_IN_RADIANS']   = "-r"     
chars['ACC_CSV_INDEX']       = "-A"   
chars['GYRO_CSV_INDEX']      = "-W"  
chars['MAG_CSV_INDEX']       = "-M"  
chars['GPS_COORD_CSV_INDEX'] = "-C"    
chars['HEIGHT_CSV_INDEX']    = "-H"        
chars['ROLL_CSV_INDEX']      = "-R"     
chars['PITCH_CSV_INDEX']     = "-P"     
chars['YAW_CSV_INDEX']       = "-Y"       
chars['KF_STDS']             = "-K"
chars['ACC_IN_REST']         = "-a"
chars['GYR_IN_REST']         = "-w"
chars['HEIGHT_VALUE']        = "-h"        
chars['ATTITUDE_SELECTOR']   = "-z"  
chars['BODY_SELECTOR']       = "-x" 
chars['PLATFORM_2_BODY']     = "-p"
chars['PLATFORM_ALIGNMENT']  = "-l"      
chars['FEEDBACK_BIAS']       = "-f"      
chars['MODE_MECH_LOCAL']     = "-m"      
chars['PROGRESS_ANGLES']     = "-y"      
chars['KF_TAU']              = "-t"
chars['INTERVAL_GPS_OFF']    = "-T"
chars['QUANT_FACTOR']        = "-q" 
chars['WRITE_IDX_FILE']      = "--idx"

kfconfig = {}
kfconfig['ACCELEROMETER_BIAS_XYZ']  = [0.1,0.1,0.1]
kfconfig['GYROMETER_BIAS_XYZ']      = [0.1,0.1,0.1]
kfconfig['ACCELEROMETER_DRIFT_XYZ'] = [0.05,0.05,0.05]
kfconfig['GYROMETER_DRIFT_RATE']    = [0.05,0.05,0.05]
kfconfig['GPS_DOP'] = [3,3,3]

# Utils
class HandlerCSV:
    delim = ','

    def __init__(self, delim_=',', decimal_='.', filename_=None):
        self.decimal = decimal_
        self.delim = delim_
        self.filename = filename_
        return
    
    def read(self):
        file = self.filename
        df = None
        try:
            df = pd.read_csv(file, delimiter=self.delim, decimal=self.decimal)
        except:
            print(f'ERROR: could not open {file}')
        return df
    

def formCmdStr(cmds, kfconfig):
    cmdstr = ''
    for cmdkey in cmds.keys():
        if cmds[cmdkey] is not None:
            cmdstr += ' ' + chars[cmdkey] + ' ' + f"{cmds[cmdkey]}".replace('[','"').replace(']','"').replace("True","1").replace("False","0")
    cmdstr = cmdstr.replace("--idx 1", "--idx").replace("--idx 0", '')
    
    kfstr = ''
    for kfkeys in kfconfig.keys():
        kfstr += f"{kfconfig[kfkeys]}".replace('[',' ').replace(']',',')
    cmdstr += ' ' + chars['KF_STDS'] + ' "' + kfstr[:-1] + '"'

    return cmdstr

def dfLook(df):
    # Initialize plot window
    pwin = plotWindow()
    # Plot Inputs
    try:
        for ll in df.columns:
            f = plt.figure()
            plt.plot(df[ll], label=ll)
            plt.grid()
            pwin.addPlot(ll, f)
            plt.close()
    except:
        print("Error: No columns in dataframe")
    return pwin


def addPlotCompare(df, labelArg, pwin):
    lbl = 'INS_' + labelArg
    f = plt.figure()
    plt.plot(df[lbl], label='INS')
    plt.grid()
    lbl = 'FUS_' + labelArg
    plt.plot(df[lbl], label='FUSION')
    plt.title(labelArg)
    plt.legend()
    pwin.addPlot(labelArg, f)
    plt.close()
    return pwin
