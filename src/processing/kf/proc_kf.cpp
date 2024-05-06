/*! 
 @file proc_kf.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of proc_kf.h are implemented.
*/

#include <general/general.h>
#include <interface/ui/ui.h>
#include <monitor/monitor.h>
#include <interface/navdata/interface_navdata.h>
#include <processing/kf/proc_kf.h>
#include <processing/frames/frames.h>

template void KalmanFilter::process<DatatypesFusion_t, DatatypesGps_t>(const DatatypesFusion_t&, const DatatypesGps_t&, const bool);

/* Initialize Kalman Filter R and Q matrices */
void KalmanFilter::initialize(void)
{
	const InputValues_s& sInputValues = cInterfaceNavdata.getInputValues();

	string valuesDataStdString = sInputValues.kfStdCfg;

	if (KF_STD_LENGTH - 1 != std::count(valuesDataStdString.begin(), valuesDataStdString.end(), ','))
	{
		updateDisplayOutputConsoleCpp("KF STD length entered is != to the required 15: 3 acc bias, 3 acc noise bias, 3 gyro bias, 3 gyro noise bias, 3 DOPs", true);
		throw MonitorException(ERROR_RETURN_NUMBER_KF_STD);
	}
	
	const char* valuesDataStd = valuesDataStdString.c_str();
	string field;
	size_t prevPos, nextPos, readUntilPos;
	int startIndex = 3;
	int endIndex = KF_STATE_VECTOR_LENGTH;

	// Read Q matrix diagonal
	prevPos = 0; nextPos = 0;
	for (int i = startIndex; i < endIndex; i++) // Based on the line format, and the matrix indexing adjusts accordingly.
	{
		// 0.001,0.004,0.005,
		// ^   	^			// The 1st ^ is 0, the 2nd is what is found, no offset needed.
		//       ^    ^		// The 1st ^ is the last nextPos + 1, and the 2nd is the next nextPos, so what is found, applying an offset of previous nextPos + 1.
		prevPos = (i == startIndex) ? 0 : nextPos + 1;
		nextPos = (i == startIndex) ? valuesDataStdString.find(",") : valuesDataStdString.find(",", nextPos + 1);
		// Have to substract 1 because the find(",") is exactly where the "," is, and we want to read until the character before, but substract only if there is still another "," after.
		readUntilPos = (i < (endIndex - 1)) ? nextPos - 1 : nextPos;
		// 0.001,0.004,0.005,
		//       ^   ^ 
		//       |   nextPos = 10
		//       prevPos = 6
		// The field is "0.004", the substr(6, 5), so count is 10 - 6 + 1;
		field = valuesDataStdString.substr(prevPos, readUntilPos - prevPos + 1);
		//sData.Q(i + KF_MEASUREMENTS_VECTOR_LENGTH, i + KF_MEASUREMENTS_VECTOR_LENGTH) = atof(field.c_str());
		sData.u(i) =  atof(field.c_str());
	}

	// Read R matrix diagonal
	startIndex = KF_STATE_VECTOR_LENGTH - 3;
	endIndex =  KF_STD_LENGTH;
	for (int i = startIndex; i < endIndex; i++) // Based on the line format, and the matrix indexing adjusts accordingly.
	{
		prevPos = (i == startIndex) ? nextPos + 1 : nextPos + 1; // In this case we don�t start in index 0 but in index of previous ",", so need to add 1 all the time.
		nextPos = valuesDataStdString.find(",", nextPos + 1); // Here no double handling is needed
		// Have to substract 1 because the find(",") is exactly where the "," is, and we to read until want the character before, but substract only if there is still another "," after.
		readUntilPos = (i < endIndex - 1) ? nextPos - 1 : nextPos;
		field = valuesDataStdString.substr(prevPos,  readUntilPos - prevPos + 1);
		//sData.R(i - startIndex, i - startIndex) = pow(atof(field.c_str()), 2);
		sData.w(i - startIndex) = pow(atof(field.c_str()), 2);
	}

	// Observation Matrix
	sData.H(arma::span(0, KF_MEASUREMENTS_VECTOR_LENGTH - 1), arma::span(0, KF_MEASUREMENTS_VECTOR_LENGTH - 1)) = arma::eye(KF_MEASUREMENTS_VECTOR_LENGTH, KF_MEASUREMENTS_VECTOR_LENGTH);

	sData.S = 0.1 * arma::eye(KF_STATE_VECTOR_LENGTH,KF_STATE_VECTOR_LENGTH);

	sData.u.subvec(6,8) %= cInterfaceNavdata.getInputValues().attitudeSelector;
	sData.u.subvec(9,11) %= cInterfaceNavdata.getInputValues().bodySelector;
	sData.u.subvec(12,14) %= cInterfaceNavdata.getInputValues().attitudeSelector;
}

/* Process Kalman Filter */
template <class DatatypePrediction_s, class DatatypeObservation_s>
void KalmanFilter::process(const DatatypePrediction_s& sDataIns, const DatatypeObservation_s& sDataGps, const bool isKfUpdatable)
{
	/* KF State transition matrix */
	stateTransitionMatrix(sDataIns);

	/* Discretize State Transition Matrix F and Process Noise Matrix Q (defined above with STDs) */
	discretize();

	/* Filter columns in State Transition Matrix according to selection of Accelerometer axes and Attitude angles. */
	componentSelection();

	/* State prediction */
	predictState();
	sData.X.subvec(6,8) %= cInterfaceNavdata.getInstance().getInputValues().attitudeSelector;
	sData.X.subvec(9,11) %= cInterfaceNavdata.getInstance().getInputValues().bodySelector;
	sData.X.subvec(12,14) %= cInterfaceNavdata.getInstance().getInputValues().attitudeSelector;

	/* Update filter */
	if (isKfUpdatable)
	{
		updateFilter(sDataGps.ENU - sDataIns.ENU);
		sData.X.subvec(6,8) %= cInterfaceNavdata.getInstance().getInputValues().attitudeSelector;
		sData.X.subvec(9,11) %= cInterfaceNavdata.getInstance().getInputValues().bodySelector;
		sData.X.subvec(12,14) %= cInterfaceNavdata.getInstance().getInputValues().attitudeSelector;
	}
}

/* Filter the matrices with the selections made for angles and axes */
void KalmanFilter::componentSelection(void)
{
	arma::vec bodySelector = cInterfaceNavdata.getInputValues().bodySelector;
	arma::vec attitudeSelector = cInterfaceNavdata.getInputValues().attitudeSelector;

	/* Filter columns related to velocity rate */
	sData.Fk.cols(6, 8) %= arma::repmat(attitudeSelector.t(), sData.F.n_rows, 1);

	/* Filter columns related to accelerometer bias */
	sData.Fk.cols(9, 11) %= arma::repmat(bodySelector.t(), sData.F.n_rows, 1);

	/* Filter columns related to Gyrometer Bias */
	sData.Fk.cols(12, 14) %= arma::repmat(attitudeSelector.t(), sData.F.n_rows, 1);

	/* Filter columns related to velocity rate */
	sData.Qk.cols(6, 8) %= arma::repmat(attitudeSelector.t(), sData.F.n_rows, 1);

	/* Filter columns related to accelerometer bias */
	sData.Qk.cols(9, 11) %= arma::repmat(bodySelector.t(), sData.F.n_rows, 1);

	/* Filter columns related to Gyrometer Bias */
	sData.Qk.cols(12, 14) %= arma::repmat(attitudeSelector.t(), sData.F.n_rows, 1);

}

/* Compute state tansition matrix */
template <class Datatype_s>
void KalmanFilter::stateTransitionMatrix(const Datatype_s& sDataIns)
{
	const InputValues_t& inputValues = cInterfaceNavdata.getInputValues();

	// Get body to LTP rotation matrix	
	const arma::mat Rb2n = Frames::matrixBody2Enu(sDataIns.RPY % inputValues.attitudeSelector);
	// Get Rotation matrix depending on mechanization mode (velocity rate in LTP or Body frame): Body-to-LTP or identity, respectively.
	const arma::mat R = (inputValues.modeMechanicsLocal) ? arma::eye(3,3) : Rb2n;
	
	// Get accelerometer and gyrometer
	const arma::vec gyr = cInterfaceNavdata.getMapInputMonitor().at(KEY_GYR).inputHolder % inputValues.attitudeSelector;
	const arma::vec acc = cInterfaceNavdata.getMapInputMonitor().at(KEY_ACC).inputHolder % inputValues.bodySelector;
	
	// Get skew symmetric matrix for accelerometer in LTP plane
	const arma::mat skew_Rf = Frames::skew(Rb2n * acc);
	// Get skey symmetric matrix for gyrometer in LTP plane
	const arma::mat skew_Rw = Frames::skew(Rb2n * gyr);
	// Get skew symmetric matrix for Earth rotation with respect to inertial frame.
	const arma::mat skew_ie = Frames::skewInertialEarth(sDataIns.LLH(0));

	// Get Euler angle derivative matrix
	const arma::mat M = Frames::matrixRateAttitudeDynamics(sDataIns.RPY % inputValues.attitudeSelector);
	// Get skew symmetric matrix for attitude angles rate
	const arma::mat skew_rpy = Frames::skew(sDataIns.RPY_dot % inputValues.attitudeSelector);

	// Position rate error propagation
	sData.F(arma::span(0,2), arma::span(3,5)) = R * arma::eye(3,3);

	// Velocity rate error propagation
	sData.F(5,2) = 2 * Frames::G_EQUATOR / Frames::SEMI_MAJOR_A;
	sData.F(arma::span(3,5), arma::span(3,5)) = -R.t() * skew_ie * 2;
	sData.F(arma::span(3,5),arma::span(6,8)) = -R.t() * skew_Rf;
	sData.F(arma::span(3,5), arma::span(9,11)) = R.t() * Rb2n;

	// Attitude rate error propagation
	sData.F(arma::span(6,8), arma::span(6,8)) = skew_rpy;
	sData.F(arma::span(6,8), arma::span(12,14)) = M;
	
	// Accelerometer bias rate error propagation
	sData.F(arma::span(9,11),arma::span(9,11)) = -arma::eye(3,3) / inputValues.tau;
	
	// Gyrometer bias rate error propagation
	sData.F(arma::span(12,14),arma::span(12,14)) = -arma::eye(3,3) / inputValues.tau;

	// Fill Process Noise matrix Q, Measurement Noise matrix R, and noise control matrix G
	sData.G(arma::span(3,5),arma::span(3,5)) = R.t() * Rb2n;
	sData.G(arma::span(6,8),arma::span(6,8)) = M;

	sData.Q = arma::diagmat(arma::pow(sData.u,2));//arma::diagmat(arma::pow(u,2));

	sData.R = arma::diagmat(sData.w);
}

/* Discretize F and Q matrices */
void KalmanFilter::discretize(void)
{
	const double dtImu = 1.0 / cInterfaceNavdata.getInputValues().fsImu;
	sData.Fk = (arma::eye(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH)) + sData.F * dtImu;
	sData.Qk =  sData.G * sData.Q * sData.G.t() * dtImu;
	
}

/* Predict state */
void KalmanFilter::predictState(void)
{
	sData.X = sData.Fk * sData.X; // Xk|k-1 = Fk * Xk-1|k-1
	sData.S = sData.Fk * sData.S * sData.Fk.t() + sData.Qk; // Sk|k-1
}

/* Update State */
void KalmanFilter::updateFilter(arma::vec diffs)
{
	// Innovation
	sData.Y = diffs;
	sData.I = sData.Y - sData.H * sData.X;

	// Innovation Variance
	sData.V = sData.H * sData.S * sData.H.t() + sData.R;

	// Kalman Gain
	sData.K = sData.S * sData.H.t() * sData.V.i();

	// State Vector Update
	sData.X += sData.K * sData.I; // Xk|k-

	// State Vector Covariance Update
	sData.S = ((arma::eye(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH)) - sData.K * sData.H) * sData.S;
}
