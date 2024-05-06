/*! 
 @file general.h
 @author Nicolas Padron
 @brief Description: This file contains the PNT datatypes of the navigation systems that are being used.
*/

#ifndef NAVDATA_DATATYPES_HEADER
#define NAVDATA_DATATYPES_HEADER

/* GPS data structure */
typedef struct DatatypesGps_s {
	arma::vec ECEF = arma::vec(3, arma::fill::zeros);
	arma::vec ENU = arma::vec(3, arma::fill::zeros);
	arma::vec LLH = arma::vec(3, arma::fill::value(arma::datum::nan));
	arma::vec ECEF_REF = arma::vec(3, arma::fill::value(arma::datum::nan));
} DatatypesGps_t;

/* INS data structure */
typedef struct DatatypesIns_s : DatatypesGps_s {
	arma::vec V = arma::vec(3, arma::fill::zeros);
	arma::vec V_dot  = arma::vec(3, arma::fill::zeros);
	arma::vec RPY = arma::vec(3, arma::fill::zeros);
	arma::vec RPY_dot = arma::vec(3, arma::fill::zeros);
} DatatypesIns_t;

/* Fusion data structure */
typedef struct DatatypesFusion_s : DatatypesIns_s {} DatatypesFusion_t;

#endif // NAVDATA_DATATYPES_HEADER
