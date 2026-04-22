# Redshift vs ExactRedshift

## Legacy `Redshift`

The legacy module assumes an ultra-relativistic relation between comoving
distance and redshift step and applies adiabatic cooling directly to the total
energy:

- suitable for photons and UHE particles
- backwards compatible with existing CRPropa workflows

## `ExactRedshift`

`ExactRedshift` uses the exact particle speed from `ParticleState::getVelocityExact()`
and applies cosmological redshifting to the **physical momentum** first, then
reconstructs the total energy:

\[
p_\text{new} = p \left(1 - \frac{dz}{1+z}\right)
\]

\[
E_\text{new} = \sqrt{(p_\text{new} c)^2 + m^2 c^4}
\]

This is the correct extension to massive non-ultrarelativistic particles.

## `ExactFutureRedshift`

This module is the inverse companion to `ExactRedshift` for propagation toward
larger redshift.

## Expected behaviour

- In the ultra-relativistic limit, `ExactRedshift` should match legacy
  `Redshift` to high precision.
- For mildly relativistic or non-ultrarelativistic massive particles,
  `ExactRedshift` should decrease the total energy **less** than the legacy
  module, because the cosmological redshifting is applied to momentum rather
  than imposing `E \propto 1/(1+z)`.

## Recommendation

Keep `Redshift` as default legacy behaviour and use `ExactRedshift` explicitly
when exact subluminal kinematics matter.
