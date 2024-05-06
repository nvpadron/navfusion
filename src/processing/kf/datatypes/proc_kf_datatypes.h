/*! 
 @file proc_kf_datatypes.cpp
 @author Nicolas Padron
 @brief Description: This file contains the datatypes and parameters of the Kalman Filter.
*/

#ifndef PROC_KF_DATATYPES_HEADER
#define PROC_KF_DATATYPES_HEADER

#include <armadillo>

// Matrix sizes
enum ConstantsKF_e {
	KF_STATE_VECTOR_LENGTH = 15, // 3x1 position rate, 3x1 velocity rate, 3x1 angles rate, 3x1 accelerometer bias drift, 3x1 gyrometer bias drift
	KF_MEASUREMENTS_VECTOR_LENGTH = 3, // 3x1 position from GPS
	KF_STD_LENGTH = 15, // 3 zeros, 3 acc bias, 3 acc noise bias, 3 gyro bias, 3 gyro noise bias, 3 DOPs
};

// Kalman Filter variables data structure
typedef struct DatatypesKF_t {
	arma::Mat<double> F = arma::zeros(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> G = arma::eye(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> Fk = arma::zeros(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> Q = arma::zeros(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> Qk = arma::zeros(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> S = arma::zeros(KF_STATE_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> K = arma::zeros(KF_STATE_VECTOR_LENGTH, KF_MEASUREMENTS_VECTOR_LENGTH);
	arma::Mat<double> H = arma::zeros(KF_MEASUREMENTS_VECTOR_LENGTH, KF_STATE_VECTOR_LENGTH);
	arma::Mat<double> R = arma::zeros(KF_MEASUREMENTS_VECTOR_LENGTH, KF_MEASUREMENTS_VECTOR_LENGTH);
	arma::Mat<double> V = arma::zeros(KF_MEASUREMENTS_VECTOR_LENGTH, KF_MEASUREMENTS_VECTOR_LENGTH);
	arma::vec		  X = arma::zeros(KF_STATE_VECTOR_LENGTH, 1);
	arma::vec         Y = arma::zeros(KF_MEASUREMENTS_VECTOR_LENGTH, 1);
	arma::vec         I = arma::zeros(KF_MEASUREMENTS_VECTOR_LENGTH, 1);
	arma::vec		  u = arma::zeros(KF_STATE_VECTOR_LENGTH,1);
	arma::vec		  w = arma::zeros(KF_MEASUREMENTS_VECTOR_LENGTH,1);
} DatatypesKF_t;

#endif // PROC_KF_DATATYPES_HEADER