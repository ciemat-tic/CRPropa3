# CRPropa exact-velocity bundle

## Contenido

- `docs/`: descripción del refactor
- `patches/applied`: parches regenerados contra `CRPropa3::master d4be0da67aae0205af4677c1f7b21d6d82f3ee0a`
- `tests/`: tests unitarios e integración
- `scripts/`: ayuda para aplicar de nuevo los patches

## Aplicación rápida al CRPropa3::master d4be0da67aae0205af4677c1f7b21d6d82f3ee0a

```bash
cd CRPropa3
bash scripts/apply_one.sh patches/0001-particle-state.patch
```

O todos de golpe:

```bash
cd CRPropa3
bash scripts/apply_all.sh patches
```


### Información de cada patch:

- `0012-propagation-diffusionsde-fixes.patch`: corrige las ramas neutras de `PropagationCK` y `PropagationBP`, reconstruye `E(p)` dentro de `PropagationBP` y ajusta el test `testDiffusionSDE.py` al uso de velocidad exacta.
