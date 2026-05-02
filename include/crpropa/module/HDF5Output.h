#ifdef CRPROPA_HAVE_HDF5

#ifndef CRPROPA_HDF5OUTPUT_H
#define CRPROPA_HDF5OUTPUT_H


#include "crpropa/module/Output.h"
#include <stdint.h>
#include <ctime>

#include <H5Ipublic.h>

namespace crpropa {

const size_t propertyBufferSize = 1024;

/**
 * \addtogroup Output
 * @{
 */

/**
 @class HDF5Output
 @brief Output to HDF5 Format.
The base class gives an overview of possible columns

HDF5 structure:
```
HDF5 "FILENAME.h5" {
GROUP "/" {
DATASET "OUTPUTTYPE" {
  DATATYPE  H5T_COMPOUND {
  ...
 }
 DATASPACE  SIMPLE { ( 1 ) / ( H5S_UNLIMITED ) }
 DATA {
  ...
 }
  ATTRIBUTE "Version" {
  DATATYPE  H5T_STRING {
      STRSIZE 100;
      STRPAD H5T_STR_NULLTERM;
      CSET H5T_CSET_ASCII;
      CTYPE H5T_C_S1;
      }
  DATASPACE  SCALAR
  DATA { (0): "VERSION" }
 }
} } }
```

 */
class HDF5Output: public Output {

	typedef struct OutputRow {
		long double D;
		long double time;
		long double z;
		uint64_t SN;
		int32_t ID;
		long double E;
		long double X;
		long double Y;
		long double Z;
		long double Px;
		long double Py;
		long double Pz;
		uint64_t SN0;
		int32_t ID0;
		long double E0;
		long double X0;
		long double Y0;
		long double Z0;
		long double P0x;
		long double P0y;
		long double P0z;
		uint64_t SN1;
		int32_t ID1;
		long double E1;
		long double X1;
		long double Y1;
		long double Z1;
		long double P1x;
		long double P1y;
		long double P1z;
		long double weight;
		std::string tag;
		unsigned char propertyBuffer[propertyBufferSize];
	} OutputRow;

	std::string filename;

	hid_t file, sid;
	hid_t dset, dataspace;
	mutable std::vector<OutputRow> buffer;

	time_t lastFlush;
	unsigned int flushLimit;
	unsigned int candidatesSinceFlush;
public:
	/** Default constructor.
	  	Does not run from scratch.
	    At least open() has to be called in addition.
		Units of energy and length are, by default, EeV and Mpc.
	 	This can be changed with setEnergyScale and setLengthScale.
	 */
	HDF5Output();
	/** Constructor with the default OutputType (everything).
	 	@param filename	string containing name of output hdf5 file
	 */
	HDF5Output(const std::string &filename);
	/** Constructor
	 	@param outputtype	type of output: Trajectory1D, Trajectory3D, Event1D, Event3D, Everything
	 	@param filename	string containing name of output hdf5 file
	 */
	HDF5Output(const std::string &filename, OutputType outputtype);
	~HDF5Output();

	void process(Candidate *candidate) const;
	herr_t insertStringAttribute(const std::string &key, const std::string &value);
	herr_t insertDoubleAttribute(const std::string &key, const long double &value);
	std::string getDescription() const;

	/// Force flush after N events. In long running applications with scarse
	/// output this can be set to 1 or 0 to avoid data corruption. In applications
	/// with frequent output this should be set to a high number (default)
	void setFlushLimit(unsigned int N);

	/** Create and prepare a file as HDF5-file.
	 */
	void open(const std::string &filename);
	void close();
	void flush() const;

};
/** @}*/

} // namespace crpropa

#endif // CRPROPA_HDF5OUTPUT_H

#endif // CRPROPA_HAVE_HDF5
