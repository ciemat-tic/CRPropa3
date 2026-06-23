# CRPropa portable bundle

Este directorio contiene:

- `CRPropa3/`: el código fuente de CRPropa 3.
- `CRPropa3-data/`: los scripts y tablas de generación de datos, incluyendo `data/`.
- `CRPropa3/install.sh`: instalador local sin acceso a red.

## Instalación

Ejecuta:

```bash
cd /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3
./install.sh
```

Por defecto se compila en `CRPropa3/build` y se instala en el propio directorio `CRPropa3/`, sin crear un prefijo externo. El `install.sh` que queda en el directorio superior solo delega en `CRPropa3/install.sh` por compatibilidad.

Si quieres indicar explícitamente el directorio de instalacion:

```bash
./install.sh /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3
```

El segundo argumento, si lo usas, es el directorio de compilacion. Para el flujo autocontenido no hace falta pasarlo:

```bash
./install.sh /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3 /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3/build
```

Si `cmake`, `swig` o `numpy` no aparecen en el entorno, el instalador intenta cargar automaticamente el stack Spack de ACME Rocky9 cascadelake. Si aun asi falla, ejecuta antes:

```bash
source /etc/profile.d/SPACK2.sh
spack load /tjciufg /w6dgctr /3tjlhpm /sqvlzkq /pbvefzl /kxsb4uw
```

No ejecutes `source activate.sh` hasta que `./install.sh` termine mostrando `CRPropa instalado en: ...`; ese archivo se crea al final de una instalacion correcta.

## Activación

Después de instalar:

```bash
source /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3/activate.sh
```

Esto define:

- `CRPROPA_DATA_PATH`
- `LD_LIBRARY_PATH`
- `PYTHONPATH`
- `PATH`

Además instala estos scripts de prueba en el raíz de `CRPropa3/`:

- `Velocity_test.py`
- `PropagationBP_test.py`

También deja una copia en `share/crpropa/examples/` por compatibilidad. Los scripts intentan usar `CRPROPA_HOME`; si no está definido, detectan el directorio `CRPropa3/` desde su propia ubicación.

## Prueba de velocidad

Después de activar el entorno:

```bash
cd /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3
python3 Velocity_test.py
```

Los outputs se escriben por defecto en `CRPropa3/velocity_test_outputs/`. Puedes sobrescribirlo con `CRPROPA_TEST_OUTDIR`, pero no hace falta para el flujo normal.

El resultado esperado es que `beta_output` coincida con `beta_expected` para todas las energías. La columna `D` sale como `0.999 Mpc` porque el observador es una esfera de radio `1 kpc` centrada en `1 Mpc`; la detección ocurre en la superficie cercana.

## Prueba PropagationBP

También puedes comprobar el propagador en campo magnético uniforme:

```bash
cd /home/ciemat-t13/CRPropa_linux-rocky9-cascadelake_Rafa/CRPropa3
python3 PropagationBP_test.py
```

Los outputs se escriben por defecto en `CRPropa3/bfield_larmor_outputs/`.

El script simula electrones en `B = 1 nG`, ajusta la trayectoria circular y compara el radio de Larmor obtenido con el esperado. La columna `relative_error` debe salir pequeña, típicamente alrededor de `1e-8` o menor en esta prueba.

## Dependencias

Necesitas una máquina Linux con:

- `cmake`
- `gcc` y `g++`
- `python3` con headers de desarrollo
- `numpy`
- `swig`
- `fftw`
- `hdf5`
- `muparser`

Si quieres regenerar datos desde `CRPropa3-data` en otro flujo, instala además las dependencias de `CRPropa3-data/requirements.txt`.

El instalador desactiva las lentes galacticas (`ENABLE_GALACTICMAGNETICLENS=OFF`) para evitar el fallo conocido de la interfaz SWIG con NumPy 2.x. El `Velocity_test.py` y `PropagationBP_test.py` no necesitan esa parte.

## Bundle

El bundle ya no depende de regenerar `calc_electromagnetic.py` ni de ejecutar `calc_all.py`. El flujo es:

1. Se toma directamente `CRPropa3-data/data/`.
2. `cmake` compila CRPropa en `CRPropa3/build`.
3. `cmake --install` instala `lib/`, `include/` y `share/crpropa/` dentro de `CRPropa3/`.
4. Los scripts `Velocity_test.py` y `PropagationBP_test.py` se copian al raíz de `CRPropa3/`.

El instalador solo copia `CRPropa3-data/data/` al árbol de compilación y después lo instala tal cual.

## Nota

Este bundle está pensado para compilar en la máquina destino. No incluye binarios de ACME para evitar problemas de compatibilidad de arquitectura y bibliotecas.
