import os
from helpers import *

## COMMANDS
# This are all the commands, comment out which you will not use.
# Notice that accelerometers, gyrometers, yaw angle, Frequencies and GPS coordinates are mandatory.

# Write IDX file: program will read input, write file with column indexes in CSV and stop.
cmds['WRITE_IDX_FILE']      = False         # Bool. True to write file with CSV indexes. # Comment this to run the processing after filling the CSV indexes below.

## MANDATORY: Input file and output directory
cmds['INPUT_FILE']          = ' "data/tram/input/tram.csv" '
cmds['OUTPUT_FILE']         = ' "data/tram/yaw" '

## MANDATORY: frequency, and CSV indexes of IMU measurements and GPS
cmds['FREQUENCY']           = [300, 1]      # In Hz, order as [fs_imu, fs_gps]. Default is "100,1".
cmds['ACC_CSV_INDEX']       = [1,2,3]       # Index in CSV file
cmds['GYRO_CSV_INDEX']      = [4,5,6]       # Index in CSV file
#cmds['MAG_CSV_INDEX']       = [7,8,9]    # Index in CSV file. If commented, then YAW_CSV_INDEX has to be set.
cmds['GPS_COORD_CSV_INDEX'] = [13,14]       # Index in CSV file
#cmds['HEIGHT_CSV_INDEX']    = None          # Index in CSV file. If commented or "None", then HEIGHT_VALUE below has to be set
cmds['HEIGHT_VALUE']        = 100           # Double, height value if not part of index CSV. Default is 100.

## MANDATORY: Attitude Angles
cmds['ROLL_CSV_INDEX']      = 12            # Index in CSV file. If commented, it is calculated from accelerometer.
cmds['PITCH_CSV_INDEX']     = 11            # Index in CSV file. If commented, it is calculated from accelerometer.
cmds['YAW_CSV_INDEX']       = 10            # Index in CSV file. If commented, it is calculated from accelerometer and magnetometer (MAG_CSV_INDEX becomes mandatory)

## Optional tuning values
cmds['ACC_IN_REST']         = [0.05601,  0.01959,  0.18640]       # Accelerometer values in rest, this will be subtracted to the CSV read accelerometer values. Subtraction happens in Platform frame (input frame). Default is 0s.
cmds['GYR_IN_REST']         = [0.01752,  0.03873,  0.00347]       # Gyrometer values in rest, this will be subtracted to the CSV read gyrometer values. Subtraction happens in Platform frame (input frame). Default is 0s.
cmds['PLATFORM_2_BODY']     = [0, 1, 0,     # 3x3 Platform-to-Body rotation matrix. Enter as 9x1 array, the program will reshape. Recall Body frame used in software. Default is identity.
                              -1, 0, 0,
                               0, 0,-1]
cmds['ATTITUDE_SELECTOR']   = [0,0,1]       # 1x3 array of 0s or 1s to filter (from right to left): [roll, pitch, yaw]. Default is [0,0,1]
cmds['BODY_SELECTOR']       = [1,0,0]       # 1x3 array of 0s or 1s to filter (from right to left): [x, y, z] axes in Body frame. Default is [1,0,0].
cmds['INPUTS_IN_RADIANS']   = False        # Bool. True for attitude angles in radians, False for degrees. Default is True.
cmds['PLATFORM_ALIGNMENT']  = False         # Bool. True to perform platform alignment to horizontal plane, depends on roll and pitch. 
                                            # If these are zero, or filtered by ATTITUDE_SELECTOR, then PLATFORM_ALIGNMENT is useless. Default is False.
cmds['FEEDBACK_BIAS']       = False         # Bool. True to compensate IMU measurements with KF accelerometer and gyrometer bias estimation. Default is False.
cmds['MODE_MECH_LOCAL']     = False         # Bool. True to perform mechanization of velocity in local plane, false to do in body frame.  Default is True.
cmds['PROGRESS_ANGLES']     = False          # Bool. True to use 1st attitude angles from entered (CSV indexes) or calculated (from acc, gyr, mag), and then continues with Euler derivatives.
                                            # False to use entered/calculated all the time, no euler derivatives used. Default is True.
cmds['KF_TAU']              = 100           # Scalar. Correlation time to be used in State Transition Matrix 1st order Markov processes for accelerometer and gyrometer drift. Default is 1.
cmds['INTERVAL_GPS_OFF']    = [-1,-1]       # Scalar. Interval in seconds to turn GPS off in GPS-INS fusion. Default is [-1,-1] which means don't turn off.
cmds['QUANT_FACTOR']        = 1000          # Scalar. Quantization factor to apply to input IMU values to remove small variations. Criteria is floor(x * QF) / QF. Default is 10000.
# 
## MANDATORY: IMU BIASES (to be filled as process noise in KF).
# Enter as (in order from left to right):
# [1x3 acc bias, 1x3 gyr bias, 1x3 acc drift bias, 1x3 gyr drift bias].
kfconfig['ACCELEROMETER_BIAS_XYZ']  = [0.05601,  0.01959,  0.18640]
kfconfig['GYROMETER_BIAS_XYZ']      = [0.01752,  0.03873,  0.0347]
kfconfig['ACCELEROMETER_DRIFT_XYZ'] = [0.01,0.01,0.01]
kfconfig['GYROMETER_DRIFT_RATE']    = [0.01,0.01,0.01]
   
# GPS DOPs
kfconfig['GPS_DOP'] = [3,3,3]

# END OF USER INPUTS

cmdstr = formCmdStr(cmds,kfconfig)
#os.system(".\\build\\src\\Release\\navfusion.exe" + cmdstr)
os.system(".\\out\\navfusion.exe" + cmdstr)

print('End of file')
