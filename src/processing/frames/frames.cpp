/*! 
 @file frames.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of frames.h are implemented.
*/

#include <processing/frames/frames.h>

arma::vec Frames::llh2ecef(const arma::vec llh)
{
	double phi, lambda, h, N;
	double x, y, z;
	x = y = z = N = 0;
	phi = llh(0);
	lambda = llh(1);
	h = llh(2);

	/* Follwoing 2.2.3.2 Conversion from Geodetic Coordinates to Cartesian Coordinates in ECEF Frame */
	N = (double)SEMI_MAJOR_A;
	N /= sqrt(1 + (1 - pow((double)ECC, 2)) * pow(tan(phi), 2));
	x = cos(lambda) * N + h * cos(lambda) * cos(phi);
	y = sin(lambda) * N + h * sin(lambda) * cos(phi);
	z = (double)SEMI_MAJOR_A * (1 - pow((double)ECC, 2)) * sin(phi);
	z /= sqrt(1 - pow((double)ECC, 2) * pow(sin(phi), 2));
	z += h * sin(phi);

	return { x, y, z };
}

arma::vec Frames::ecef2llh(const arma::vec ecef)
{
	arma::vec LLH = arma::zeros(3, 1);
	double diff_tan_u = 1;
	double cos2u, sin2u, tan_phi, phi, lambda, h, u, N;
	cos2u = sin2u = tan_phi = phi = lambda = h = u = N = 0;
	
	/* Following Table 2.1 "Determination of Geodetic Height and Latitude in Terms of ECEF Parameters "*/
	double p = arma::norm(ecef.subvec(0,1));
	double tan_u = (p > 0) ? (ecef(2) / p) * SEMI_MAJOR_A / SEMI_MINOR_B : 0;

	while (abs(diff_tan_u) > 1e-12)
	{
		cos2u = (p > 0) ? (1 / (1 + pow(tan_u, 2))) : 0;
		sin2u = 1 - cos2u;
		tan_phi = ecef(2) + pow(ECC_SEC, 2) * SEMI_MINOR_B * pow(sqrt(sin2u), 3);
		tan_phi /= (p - pow((double)ECC, 2) * SEMI_MAJOR_A * pow(sqrt(cos2u), 3));
		diff_tan_u = tan_u;
		tan_u = (double)SEMI_MINOR_B / SEMI_MAJOR_A * tan_phi;
		diff_tan_u -= tan_u; 
	}
	phi = atan(tan_phi);
	N = (double)SEMI_MAJOR_A / sqrt(1 - pow((double)ECC, 2) * pow(sin(phi), 2));
	if (int(abs(phi)*256) != int(90.0*DEG2RAD*256))
	{
		h = p / cos(phi) - N;
	}
	else if (int(abs(phi)*256) != 0)
	{
		h = ecef(2) / sin(phi) - N + pow((double)ECC, 2) * N;
	}

	if (int(ecef(0)*256) >= 0)
	{
		lambda = atan(ecef(1) / ecef(0));
	}
	else if (int(ecef(0)*256) < 0 && int(ecef(1)*256) >= 0)
	{
		lambda = 180*DEG2RAD + atan(ecef(1) / ecef(0));
	}
	else if (int(ecef(0)*256) < 0 && int(ecef(1)*256) < 0)
	{
		lambda = -180*DEG2RAD + atan(ecef(1) / ecef(0));
	}

	LLH = { phi, lambda, h };

	return LLH;
}

arma::vec Frames::ecef2enu(const arma::vec llh, const arma::vec ecef, const arma::vec xyz0)
{
	arma::Mat<double> rEcef2Enu = matrixEcef2Enu(llh);

	return rEcef2Enu * (ecef - xyz0);
}

arma::vec Frames::enu2ecef(const arma::vec llh, const arma::vec enu, const arma::vec xyz0)
{
	arma::Mat<double> rEnu2Ecef = matrixEcef2Enu(llh).t();

	return rEnu2Ecef * enu + xyz0;
}

arma::Mat<double> Frames::genRotRx(const float angle)
{
	arma::Mat<double> Rx = arma::zeros(3, 3);
	Rx.col(0) = arma::vec({ 1, 0, 0 });
	Rx.col(1) = arma::vec({ 0, cos(angle), -sin(angle) });
	Rx.col(2) = arma::vec({ 0, sin(angle), cos(angle) });
	return Rx;
}

arma::Mat<double> Frames::matrixEcef2Enu(const arma::vec llh)
{
	const double cosLat = cos(llh(0));
	const double sinLat = sin(llh(0));
	const double cosLon = cos(llh(1));
	const double sinLon = sin(llh(1));

	arma::mat rEcef2Enu = {
							{-sinLon         ,  cosLon         ,  0     },
							{-sinLat * cosLon, -sinLat * sinLon, cosLat },
							{ cosLat*cosLon  , cosLat * sinLon , sinLat}
						  };

	return rEcef2Enu;
}

// Body 2 enu rotation matrix
arma::Mat<double> Frames::matrixBody2Enu(const arma::vec rpy)
{
	double roll = rpy(0);
	double pitch = rpy(1);
	double yaw = rpy(2);
	// Rotation is the same as doing: (ROT_RY(roll) * (ROT_RX(pitch) * ROT_RZ(-yaw))).t();
	arma::mat rBody2Enu = {
							{sin(yaw)*cos(pitch),  cos(yaw)*cos(roll) + sin(yaw)*sin(pitch)*sin(roll), -cos(yaw)*sin(roll) + sin(yaw)*sin(pitch)*cos(roll)},
							{cos(yaw)*cos(pitch), -sin(yaw)*cos(roll) + cos(yaw)*sin(pitch)*sin(roll),  cos(yaw)*sin(pitch)*cos(roll)+sin(yaw)*cos(roll)  },
							{sin(pitch)         , -cos(pitch)*sin(roll)                              , -cos(pitch)*cos(roll)                              }
						  };
	return  rBody2Enu.replace(arma::datum::nan, 0);
}

arma::mat Frames::matrixBody2H(const arma::vec rpy)
{
	return ROT_RX(-rpy(0)) * ROT_RY(-rpy(1));
}

arma::mat Frames::matrixPlatform2Body(arma::vec diagvec)
{
	arma::mat Rp2n = arma::reshape(diagvec,3,3);
	return Rp2n; 
}

arma::mat Frames::matrixRateAttitudeDynamics(const arma::vec rpy)
{
	arma::Mat<double> rpyRatesMatrix = arma::zeros(3, 3);

	rpyRatesMatrix(0, 0) = 1;
	rpyRatesMatrix(0, 1) = sin(rpy(0)) * tan(rpy(1));
	rpyRatesMatrix(0, 2) = cos(rpy(0)) * tan(rpy(1));

	rpyRatesMatrix(1, 1) = cos(rpy(0));
	rpyRatesMatrix(1, 2) = -sin(rpy(0));

	rpyRatesMatrix(2, 1) = sin(rpy(0)) / cos(rpy(1));
	rpyRatesMatrix(2, 2) = cos(rpy(0)) / cos(rpy(1));

	return rpyRatesMatrix.replace(arma::datum::nan, 0);
}

arma::mat Frames::skew(const arma::vec x)
{
	arma::mat skewMat = {
							{0    , -x(2),  x(1)},
				      		{x(2) ,  0   , -x(0)},
					  		{-x(1),  x(0),  0   }
						};
	return skewMat;
}

const arma::mat Frames::skewInertialEarth(const double lat)
{
	const arma::vec wbn = arma::vec({0, Frames::EARTH_ROTATION * cos(lat), Frames::EARTH_ROTATION * sin(lat)});
	return Frames::skew(wbn);
}

void Frames::adjustRollPitch(double& x)
{
	if (x > Frames::PI / 2) 
	{
		x = -Frames::PI / 2;
	}
	else if (x < -Frames::PI / 2)
	{
		x = -Frames::PI / 2;
	}
}

void Frames::adjustYaw(double& yaw)
{
	int yawInt = (int)(yaw * 65536);
	if (yaw > 2*Frames::PI) 
	{
		if (abs(yaw - 2*Frames::PI) < Frames::ADJUST_ANGLE_MARGIN)
		{
			yaw = 2*Frames::PI;
		}
		else
		{
			yawInt %= 2*Frames::PI_16;
			yaw = ((double)yawInt) / 65536;

		}
	}
	else if (yaw < 0)
	{
		if (abs(yaw) < Frames::ADJUST_ANGLE_MARGIN)
		{
			yaw = 0;
		}
		else
		{
			yawInt += 2*Frames::PI_16;
			yawInt %= 2*Frames::PI_16;
			yaw = ((double)yawInt) / 65536;
		}	
	}
}

double Frames::gravityCorrectionForComponentZ(double lat, double hei)
{
	double glocalCalculated = 0, A = 0, B = 0, D = 0, C = 0, G0 = 0;
	long double k = 0;

	k = (long double)((long double)Frames::SEMI_MINOR_B * Frames::G_POLES);
	k /= (long double)((long double)Frames::SEMI_MAJOR_A * Frames::G_EQUATOR);
	k -= 1;

	G0 = Frames::G_EQUATOR * (1 + k * pow(sin(lat),2)) / sqrt(1 - pow(Frames::ECC, 2) * pow(sin(lat),2));
	B = pow(Frames::SEMI_MAJOR_A / (Frames::SEMI_MAJOR_A + hei), 2);
	glocalCalculated = G0 * B;

	return glocalCalculated;
}

