#include "HepPID/ParticleIDMethods.hh"
#include "crpropa/Random.h"
#include "crpropa/magneticLens/ParticleMapsContainer.h"
#include "crpropa/Units.h"

#include <iostream>
#include <fstream>

namespace crpropa  {

ParticleMapsContainer::~ParticleMapsContainer() {
	for(std::map<int, std::map<int, long double*> >::iterator pid_iter = _data.begin();
			pid_iter != _data.end(); ++pid_iter) {
		for(std::map<int, long double*>::iterator energy_iter = pid_iter->second.begin();
			energy_iter != pid_iter->second.end(); ++energy_iter) {
			delete[] (energy_iter->second);
		}
	}
}

int ParticleMapsContainer::energy2Idx(long double energy) const {
	long double lE = log10(energy / eV);
	return int((lE - _bin0lowerEdge) / _deltaLogE);
}

long double ParticleMapsContainer::idx2Energy(int idx) const {
	return pow(10, idx * _deltaLogE + _bin0lowerEdge + _deltaLogE / 2) * eV;
}

		
long double* ParticleMapsContainer::getMap(const int particleId, long double energy) {
	_weightsUpToDate = false;
	if (_data.find(particleId) == _data.end()) {
		std::cerr << "No map for ParticleID " << particleId << std::endl;
		return NULL;
	}
	int energyIdx	= energy2Idx(energy);
	if (_data[particleId].find(energyIdx) == _data[particleId].end()) {
		std::cerr << "No map for ParticleID and energy" << energy / eV << " eV" << std::endl;
		return NULL;
	}
	return _data[particleId][energy2Idx(energy)];
}
			
			
void ParticleMapsContainer::addParticle(const int particleId, long double energy, long double galacticLongitude, long double galacticLatitude, long double weight) {
	_weightsUpToDate = false;
	if (_data.find(particleId) == _data.end()) {
		map<int, long double*> M;
		_data[particleId] = M;
	}

	int energyIdx	= energy2Idx(energy);
	if (_data[particleId].find(energyIdx) == _data[particleId].end()) {
		_data[particleId][energyIdx] = new long double[_pixelization.getNumberOfPixels()];
		std::fill(_data[particleId][energyIdx], _data[particleId][energyIdx] + _pixelization.getNumberOfPixels(), 0);
	}

	uint32_t pixel = _pixelization.direction2Pix(galacticLongitude, galacticLatitude);
	_data[particleId][energyIdx][pixel] += weight;
}


void ParticleMapsContainer::addParticle(const int particleId, long double energy, const Vector3d &p, long double weight) {
	long double galacticLongitude = atan2(-p.y, -p.x);
	long double galacticLatitude =	M_PI / 2 - acos(-p.z / p.getR());
	addParticle(particleId, energy, galacticLongitude, galacticLatitude, weight);
}


std::vector<int> ParticleMapsContainer::getParticleIds() {
	std::vector<int> ids;
	for(std::map<int, std::map<int, long double*> >::iterator pid_iter = _data.begin();
			pid_iter != _data.end(); ++pid_iter) {
		ids.push_back(pid_iter->first);
	}
	return ids;
}


std::vector<long double> ParticleMapsContainer::getEnergies(int pid) {
	std::vector<long double> energies;
	if (_data.find(pid) != _data.end()) {
		for(std::map<int, long double*>::iterator iter = _data[pid].begin();
			iter != _data[pid].end(); ++iter) {
			energies.push_back( idx2Energy(iter->first) / eV );
		}
	}
	return energies;
}


void ParticleMapsContainer::applyLens(MagneticLens &lens) {
	// if lens is normalized, this should not be necessary.
	_weightsUpToDate = false;

	for(std::map<int, std::map<int, long double*> >::iterator pid_iter = _data.begin();
			pid_iter != _data.end(); ++pid_iter) {
		for(std::map<int, long double*>::iterator energy_iter = pid_iter->second.begin();
			energy_iter != pid_iter->second.end(); ++energy_iter) {
			// transform only nuclei
			long double energy = idx2Energy(energy_iter->first);
			int chargeNumber = HepPID::Z(pid_iter->first);
			if (chargeNumber != 0 && lens.rigidityCovered(energy / chargeNumber)) {
				lens.transformModelVector(energy_iter->second, energy / chargeNumber);
			} else { // still normalize the vectors 
				for(size_t j=0; j< _pixelization.getNumberOfPixels() ; j++) {
					energy_iter->second[j] /= lens.getNorm();
				}
			}
		}
	}
}


void ParticleMapsContainer::_updateWeights() {
	if (_weightsUpToDate)
		return;

	for(std::map<int, std::map<int, long double*> >::iterator pid_iter = _data.begin();
			pid_iter != _data.end(); ++pid_iter) {
		_weightsPID[pid_iter->first] = 0;

		for(std::map<int, long double*>::iterator energy_iter = pid_iter->second.begin();
			energy_iter != pid_iter->second.end(); ++energy_iter)  {

			_weights_pidEnergy[pid_iter->first][energy_iter->first] = 0;
			for(size_t j = 0; j< _pixelization.getNumberOfPixels() ; j++) {
				_weights_pidEnergy[pid_iter->first][energy_iter->first] +=energy_iter->second[j];
				_weightsPID[pid_iter->first]+=energy_iter->second[j];
			}
			_sumOfWeights+=_weights_pidEnergy[pid_iter->first][energy_iter->first];
		}
	}
	_weightsUpToDate = true;
}


void ParticleMapsContainer::getRandomParticles(size_t N, vector<int> &particleId, 
	vector<long double> &energy, vector<long double> &galacticLongitudes,
	vector<long double> &galacticLatitudes) {
	_updateWeights();

	particleId.resize(N);
	energy.resize(N);
	galacticLongitudes.resize(N);
	galacticLatitudes.resize(N);

	for(size_t i=0; i< N; i++) {
		//get particle
		long double r = Random::instance().rand() * _sumOfWeights;
		std::map<int, long double>::iterator iter = _weightsPID.begin();
		while ((r-= iter->second) > 0) {
			++iter; 
		}
		particleId[i] = iter->first;
	
		//get energy
		r = Random::instance().rand() * iter->second;
		iter = _weights_pidEnergy[particleId[i]].begin();
		while ((r-= iter->second) > 0) {
		 ++iter; 
		}
		energy[i] = idx2Energy(iter->first) / eV;

		placeOnMap(particleId[i], energy[i] * eV, galacticLongitudes[i], galacticLatitudes[i]);
	}
}


bool ParticleMapsContainer::placeOnMap(int pid, long double energy, long double &galacticLongitude, long double &galacticLatitude) {
	_updateWeights();

	if (_data.find(pid) == _data.end()) {
		return false;
	}
	int energyIdx	= energy2Idx(energy);
	if (_data[pid].find(energyIdx) == _data[pid].end()) {
		return false;
	}

	long double r = Random::instance().rand() * _weights_pidEnergy[pid][energyIdx];

	for(size_t j = 0; j< _pixelization.getNumberOfPixels(); j++) {
		r -= _data[pid][energyIdx][j];
		if (r <= 0) {
			_pixelization.getRandomDirectionInPixel(j, galacticLongitude, galacticLatitude);
			return true;
		}
	}
	return false;
}


void ParticleMapsContainer::forceWeightUpdate() {
	_weightsUpToDate = false;
}

} // namespace crpropa
