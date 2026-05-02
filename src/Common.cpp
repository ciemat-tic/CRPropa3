#include "crpropa/Common.h"

#include "kiss/path.h"
#include "kiss/logger.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

#define index(i,j) ((j)+(i)*Y.size())

namespace crpropa {

std::string removeNullCharacter(std::string path) {
	// check for null character in data path and remove it
	if (path.find('\x00') != std::string::npos)
		path.erase(std::remove(path.begin(), path.end(), '\x00'), path.end());
	return path;
}

std::string getDataPath(std::string filename) {
	static std::string dataPath;

	if (dataPath.size())
		return removeNullCharacter(concat_path(dataPath, filename));

	const char *env_path = getenv("CRPROPA_DATA_PATH");
	if (env_path) {
		if (is_directory(env_path)) {
			dataPath = removeNullCharacter(env_path);
			KISS_LOG_INFO << "getDataPath: use environment variable, "
					<< dataPath << std::endl;
			return concat_path(dataPath, filename);
		}
	}

#ifdef CRPROPA_INSTALL_PREFIX
	{
		std::string _path = CRPROPA_INSTALL_PREFIX "/share/crpropa";
		if (is_directory(_path)) {
			dataPath = removeNullCharacter(_path);
			KISS_LOG_INFO
			<< "getDataPath: use install prefix, " << dataPath << std::endl;
			return concat_path(dataPath, filename);
		}
	}
#endif

	{
		std::string _path = removeNullCharacter(executable_path() + "../data");
		if (is_directory(_path)) {
			dataPath = _path;
			KISS_LOG_INFO << "getDataPath: use executable path, " << dataPath
					<< std::endl;
			return concat_path(dataPath, filename);
		}
	}

	dataPath = "data";
	KISS_LOG_INFO << "getDataPath: use default, " << dataPath << std::endl;
	return removeNullCharacter(concat_path(dataPath, filename));
}


std::string getInstallPrefix()
{
  std::string _path = "";
  #ifdef CRPROPA_INSTALL_PREFIX
    _path += CRPROPA_INSTALL_PREFIX;
  #endif
  return _path;
};

long double interpolate(long double x, const std::vector<long double> &X,
		const std::vector<long double> &Y) {
	std::vector<long double>::const_iterator it = std::upper_bound(X.begin(),
			X.end(), x);
	if (it == X.begin())
		return Y.front();
	if (it == X.end())
		return Y.back();

	size_t i = it - X.begin() - 1;
	return Y[i] + (x - X[i]) * (Y[i + 1] - Y[i]) / (X[i + 1] - X[i]);
}

long double interpolate2d(long double x, long double y, const std::vector<long double> &X,
		const std::vector<long double> &Y, const std::vector<long double> &Z) {

	std::vector<long double>::const_iterator itx = std::upper_bound(X.begin(), X.end(), x);
	std::vector<long double>::const_iterator ity = std::upper_bound(Y.begin(), Y.end(), y);

	if (x > X.back() || x < X.front())
		return 0;
	if (y > Y.back() || y < Y.front())
		return 0;

	if (itx == X.begin() && ity == Y.begin())
		return Z.front();
	if (itx == X.end() && ity == Y.end())
		return Z.back();

	size_t i = itx - X.begin() - 1;
	size_t j = ity - Y.begin() - 1;

	long double Q11 = Z[index(i,j)];
	long double Q12 = Z[index(i,j+1)];
	long double Q21 = Z[index(i+1,j)];
	long double Q22 = Z[index(i+1,j+1)];

	long double R1 = ((X[i+1]-x)/(X[i+1]-X[i]))*Q11+((x-X[i])/(X[i+1]-X[i]))*Q21;
	long double R2 = ((X[i+1]-x)/(X[i+1]-X[i]))*Q12+((x-X[i])/(X[i+1]-X[i]))*Q22;

	return ((Y[j+1]-y)/(Y[j+1]-Y[j]))*R1+((y-Y[j])/(Y[j+1]-Y[j]))*R2;
}

long double interpolateEquidistant(long double x, long double lo, long double hi,
		const std::vector<long double> &Y) {
	if (x <= lo)
		return Y.front();
	if (x >= hi)
		return Y.back();

	long double dx = (hi - lo) / (Y.size() - 1);
	long double p = (x - lo) / dx;
	size_t i = floor(p);
	return Y[i] + (p - i) * (Y[i + 1] - Y[i]);
}

size_t closestIndex(long double x, const std::vector<long double> &X) {
	size_t i1 = std::lower_bound(X.begin(), X.end(), x) - X.begin();
	if (i1 == 0)
		return i1;
	size_t i0 = i1 - 1;
	if (std::fabs(X[i0] - x) < std::fabs(X[i1] - x))
		return i0;
	else
		return i1;
}

} // namespace crpropa

