# Exact-velocity refactor for CRPropa

## Objetivo físico

Sustituir, donde corresponda:

- `v = c`
- `p = E / c`
- `R = E / |q|`
- `dt = ds / c`

por:

- `beta = sqrt(1 - m^2 c^4 / E^2)`
- `v = beta c`
- `p = sqrt(E^2 - m^2 c^4) / c`
- `R = p c / |q|`
- `dt = ds / v`

## Notas

- Los parches se han separado por bloques lógicos.
- Los parches de `PropagationBP`, `PropagationCK`, `DiffusionSDE`, `Output` y `NuclearDecay`
  son los que más probablemente requieran revisión funcional adicional.
