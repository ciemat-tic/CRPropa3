#!/usr/bin/env python3
import crpropa as crp

def run(module, name, energy, redshift, step):
    c = crp.Candidate(crp.nucleusId(1, 1), 0.)
    c.current.setEnergy(energy)
    c.setRedshift(redshift)
    c.setCurrentStep(step)
    module.process(c)
    print(f"{name}: z -> {c.getRedshift():.12g}, E -> {c.current.getEnergy()/crp.EeV:.12g} EeV")

def main():
    redshift = 0.024
    step = 1 * crp.Mpc

    print("Ultra-relativistic example")
    run(crp.Redshift(), "legacy", 100 * crp.EeV, redshift, step)
    run(crp.ExactRedshift(), "exact ", 100 * crp.EeV, redshift, step)

    print()
    print("Mildly relativistic proton example")
    ctmp = crp.Candidate(crp.nucleusId(1, 1), 0.)
    m = ctmp.current.getMass()
    energy = 2.0 * m * crp.c_squared
    run(crp.Redshift(), "legacy", energy, redshift, step)
    run(crp.ExactRedshift(), "exact ", energy, redshift, step)

if __name__ == "__main__":
    main()
