# Applied patch sets

This directory contains patch bundles that have already been applied to this fork of CRPropa.

## Available bundles

- `exact-velocity/`
  - Physics refactor that removes several hidden ultra-relativistic assumptions from the runtime and introduces exact finite-velocity kinematics where they matter physically.
- `textouput-precision/`
  - Output-only improvement that makes text precision configurable. This bundle does not change the physical model.

## Which bundle changes the physics?

The physics changes are documented in:

- `exact-velocity/README.md`
- `exact-velocity/README.es.md`

Those two files describe the physical motivation, the equations that were changed, the modules that were modified, and the intended domain of validity.

## Scope of the documentation

The documentation in `exact-velocity/` is meant to describe the final applied state of the code after the full exact-velocity patch series, including the later corrective patches and the optional exact cosmological redshift modules.
