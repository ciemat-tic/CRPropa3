# Exact-velocity physics patch set

## Purpose

This patch set upgrades the CRPropa runtime from a strictly ultra-relativistic interpretation toward a finite-velocity treatment of massive particles wherever that change is physically meaningful.

Historically, several parts of the runtime assumed one or more of the following relations implicitly:

- `v = c`
- `p = E / c`
- `R = E / |q|`
- `dt = ds / c`

Those approximations are excellent in the standard UHE regime, but they are not exact for massive particles outside the strict ultra-relativistic limit. The applied patch series introduces the corresponding exact relations in the runtime modules where they matter:

- `beta = sqrt(1 - m^2 c^4 / E^2)`
- `v = beta c`
- `p = sqrt(E^2 - m^2 c^4) / c`
- `R = p c / |q|`
- `dt = ds / v`

For massless particles the expected limits are preserved:

- `beta = 1`
- `p = E / c`

The practical aim of the series is not to turn CRPropa into a generic low-energy transport code, but to remove hidden `v = c` assumptions from timing, rigidity, momentum, decay length, and related kinematic quantities in the existing runtime.

**Applied patches for this section**

- Global scope of the series, not a single patch
- The purpose is implemented collectively by patches `0001` through `0017`

## General design strategy

The modifications were implemented conservatively.

1. The historical legacy API was preserved where possible.
2. Exact helper methods were introduced first in `ParticleState`.
3. Runtime modules were updated only where exact finite-velocity kinematics are physically relevant.
4. The standard ultra-relativistic behaviour is still recovered in the appropriate limit.
5. The final state was validated with C++ unit tests, Python tests, and regression-style comparisons.

The design intentionally avoids silently changing every historical module. Where a new physical behaviour could affect backwards compatibility, an opt-in alternative was preferred over replacing the legacy module unconditionally.

**Applied patches for this section**

- Global strategy of the full exact-velocity series
- Validation and support files in the applied patch directory
- This section summarizes how patches `0001` through `0017` were applied together

## Main physical changes

### 1. Exact particle kinematics in `ParticleState`

`ParticleState` now provides exact kinematic helpers for massive particles in addition to the historical ultra-relativistic helpers.

New exact helpers include:

- `getBeta()`
- `getVelocityExact()`
- `getMomentumExact()`
- `getRigidityExact()`
- `getProperVelocity()` as a helper for massive particles

The legacy helpers are still present for compatibility, but they should now be read as historical approximations rather than exact runtime physics.

Physically, this is the foundation of the entire refactor. The rest of the series uses these helpers to reconstruct finite particle speed, exact momentum, and exact rigidity from total energy and rest mass.

**Applied patches for this section**

- `0001-particle-state.patch`

### 2. Exact time of flight in `Candidate`

`Candidate` now uses the exact particle speed when converting geometric path length into elapsed time.

This replaces the historical implicit conversion

- `dt = ds / c`

with

- `dt = ds / v`

This affects both the current candidate time and the backtracked time assigned to secondaries created at displaced positions.

Physically, this matters whenever the particle is massive and not strictly ultra-relativistic. It is one of the cleanest places where the old `v = c` approximation propagated directly into observable quantities.

**Applied patches for this section**

- `0002-candidate.patch`

### 3. Exact momentum-based propagation in `PropagationCK`

`PropagationCK` was refactored so that the Cash–Karp phase-space integration uses exact finite-velocity dynamics.

In its final form, this module is the natural place where the refactor uses exact momentum-based propagation. The phase-space variable is treated consistently with the exact total energy and the exact particle momentum, and the geometric motion is derived from the exact particle speed instead of the historical `c` approximation.

Physically, this is the cleanest exact-kinematics propagator of the series.

**Applied patches for this section**

- `0007-propagation-ck.patch`

### 4. Exact-momentum correction of the Boris propagator in `PropagationBP`

`PropagationBP` required a different final design from `PropagationCK`.

A direct reformulation in full momentum space did not preserve the intended behaviour of the historical Boris implementation. In particular, it led to problems in adaptive step control and in the fixed-step optimisation path.

The final applied solution therefore keeps the geometric Boris structure:

- propagation in unit direction
- geometric path length as the main step variable

but replaces the hidden ultra-relativistic assumption in the magnetic rotation coefficient with the exact momentum magnitude.

In other words, `PropagationBP` remains a Boris-like propagator in direction space, while the coefficient that historically behaved as if `p = E / c` is now built from the exact momentum.

This is physically important because it preserves the robust geometric character of the original Boris implementation while removing the non-exact momentum approximation.

**Applied patches for this section**

- `0008-propagation-bp.patch`
- `0014-propagation-bp-step-units.patch`
- `0015-propagation-bp-boris-direction-exact-momentum.patch`

### 5. Exact momentum reconstruction in `MomentumDiffusion`

`MomentumDiffusion` no longer relies internally on the ultra-relativistic identity `E = p c`.

Instead, the module evolves the exact momentum and reconstructs the total energy through the exact relativistic relation. This matters mainly in the mildly relativistic regime. In the strict ultra-relativistic limit, the modified code naturally converges back to the historical behaviour.

Physically, this patch is important because momentum diffusion is one of the places where using `E/c` as if it were always the momentum can silently bias the result outside the UHE limit.

**Applied patches for this section**

- `0003-momentum-diffusion.patch`

### 6. Exact speed and rigidity in `DiffusionSDE`

`DiffusionSDE` was updated so that time conversion and rigidity-dependent quantities use exact finite-velocity kinematics where appropriate.

The key point is to avoid mixing:

- geometric step length
- internal time step
- historical `c`-based conversions

This part of the series required several follow-up cleanups, because the module mixes transport geometry, stochastic propagation, and rigidity-dependent physics in a way that is sensitive to hidden ultra-relativistic assumptions.

**Applied patches for this section**

- `0009-diffusion-sde.patch`
- `0012-propagation-diffusionsde-fixes.patch`
- `0013-followup-cleanups.patch`

### 7. Exact minimum-rigidity break condition

The minimum-rigidity break condition now uses the exact rigidity

- `R = p c / |q|`

instead of the historical ultra-relativistic approximation

- `R = E / |q|`

This is physically relevant because rigidity is one of the most important control variables in transport and diffusion. Using `E / |q|` outside the strict ultra-relativistic regime systematically over-identifies the physical rigidity of a massive particle.

**Applied patches for this section**

- `0013-followup-cleanups.patch`

### 8. Exact momentum and exact speed in synchrotron and adiabatic losses

Where the runtime historically used the ultra-relativistic momentum approximation, the modified code now uses the exact momentum or the exact speed whenever the physical formula requires it.

This improves the consistency of:

- synchrotron-related quantities
- time-based adiabatic cooling

The key physical point is that loss terms that depend explicitly on momentum or elapsed time should not inherit a hidden `v = c` approximation unless the model is explicitly meant to be ultra-relativistic.

**Applied patches for this section**

- `0004-synchrotron-radiation.patch`
- `0005-adiabatic-cooling.patch`

### 9. Exact momentum and exact rigidity in `Acceleration`

`Acceleration` was updated to use exact momentum and exact rigidity in the places where the runtime previously relied on ultra-relativistic helper methods.

Physically, this matters because scattering and acceleration operators often depend more naturally on momentum or rigidity than on total energy, and the approximation `p = E / c` is not exact for massive particles outside the strict ultra-relativistic limit.

**Applied patches for this section**

- `0006-acceleration.patch`

### 10. Exact decay length of unstable nuclei and particles

The decay length of unstable nuclei is no longer treated as if the particle always propagated at `c`.

The relevant spatial scale is now based on

- `gamma beta c tau0 = gamma v tau0`

which is the physically correct expression for a massive particle with finite speed.

The decay tables themselves are still based on proper lifetimes and branching data. The change is in the runtime conversion from proper lifetime to spatial decay scale.

This is one of the most physically direct corrections of the whole series, because the wrong decay length is an explicit consequence of assuming that every particle travels at the speed of light.

**Applied patches for this section**

- `0011-nuclear-decay.patch`

### 11. Clarified output semantics for direction, velocity, and momentum

The text and structured outputs were updated so that direction, velocity, and momentum are no longer implicitly conflated when exact finite-velocity kinematics are enabled.

This is mainly a diagnostic and reproducibility change rather than a change of the physical model itself, but it is important for interpreting results correctly after the refactor.

**Applied patches for this section**

- `0010-output.patch`

### 12. Optional exact cosmological redshift modules

The historical `Redshift` and `FutureRedshift` modules are preserved for backward compatibility.

In addition, this patch series introduces optional exact cosmological redshift modules:

- `ExactRedshift`
- `ExactFutureRedshift`

These modules are intended for massive particles outside the strict ultra-relativistic regime.

The key idea is that cosmological redshifting is applied to the physical momentum first, and the total energy is then reconstructed from the exact relation. In the applied implementation this is handled through a momentum-based update in the exact redshift modules, rather than by assuming that the total energy always scales as if the particle were effectively massless.

This is a deliberately opt-in extension. It does not silently replace the historical cosmological treatment.

**Applied patches for this section**

- `0016-exact-redshift-and-future.patch`
- `0017-exactredshift-test-include-cosmology.patch`

## What was not intentionally changed

This patch set does not attempt to rewrite all of CRPropa into a generic low-energy transport code.

In particular:

- the legacy `Redshift` module was preserved
- the legacy ultra-relativistic helper methods in `ParticleState` were preserved
- the patch set does not by itself certify that every precomputed interaction table in `CRPropa3-data` is valid down to arbitrarily low energies

The main target of the work is runtime consistency of transport, timing, rigidity, momentum, decay length, and related kinematic quantities.

**Applied patches for this section**

- No dedicated patch
- This section documents explicit non-goals and preserved legacy behaviour of the whole series

## Expected physical impact

### In the ultra-relativistic regime

The modified code is expected to reproduce the historical behaviour of CRPropa to high accuracy.

This includes the standard UHE use cases for which the original approximations were already very good.

### In the mildly relativistic or finite-velocity regime

Differences are expected mainly in:

- time of flight
- exact rigidity
- decay lengths of unstable particles and nuclei
- modules that use momentum-dependent or time-dependent quantities
- cosmological redshift of massive particles when the optional exact modules are used

These differences are generally small in strongly ultra-relativistic runs and become more relevant when the particle kinetic energy is comparable to the rest-mass scale.

Physically, this means that the patch set is most relevant in the transition region where a massive particle is neither non-relativistic nor so energetic that `beta` is effectively indistinguishable from one.

**Applied patches for this section**

- Consequence of the full exact-velocity patch series
- Especially relevant applied patches:
  - `0001-particle-state.patch`
  - `0002-candidate.patch`
  - `0003-momentum-diffusion.patch`
  - `0007-propagation-ck.patch`
  - `0008-propagation-bp.patch`
  - `0009-diffusion-sde.patch`
  - `0011-nuclear-decay.patch`
  - `0016-exact-redshift-and-future.patch`

## Final practical interpretation

This patch set should be understood as a finite-velocity consistency upgrade of the CRPropa runtime.

It does not change the main physical picture of CRPropa in the historical ultra-relativistic domain, but it removes several hidden `v = c` assumptions from the runtime and makes the treatment of massive particles more physically correct whenever finite particle velocities matter.

The most important point is that the series improves physical consistency without abandoning backward compatibility or the standard UHE use case that originally motivated the framework.

**Applied patches for this section**

- Consequence of the full exact-velocity patch series
- This section is a practical summary of the whole applied set
