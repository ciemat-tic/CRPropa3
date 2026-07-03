# CRPropa portable bundle

El nivel superior del bundle contiene solo:

- `CRPropa3/`: codigo fuente, build local, instalador y scripts de prueba.
- `CRPropa3-data/`: scripts de generacion y tablas de datos, incluyendo `data/`.

Las tablas electromagneticas en `CRPropa3-data/data/EM*` estan extendidas hasta
`1 eV`. En particular, `EMInverseComptonScattering` se regenero con una malla
de energia primaria `10^0..10^23 eV` y una malla `s_kin` baja para cubrir el
regimen Thomson.

## Instalacion

Desde este directorio:

```bash
./install.sh
```

Por defecto se compila en `CRPropa3/build` y se instala en el propio directorio
`CRPropa3/`, sin crear un prefijo externo.

Si quieres indicar explicitamente el directorio de instalacion:

```bash
./install.sh /ruta/al/bundle/CRPropa3
```

El segundo argumento, si lo usas, es el directorio de compilacion:

```bash
./install.sh /ruta/al/bundle/CRPropa3 /ruta/al/bundle/CRPropa3/build
```

Si `cmake`, `swig` o `numpy` no aparecen en el entorno, el instalador intenta
cargar automaticamente el stack Spack de ACME Rocky9 cascadelake. Si aun asi
falla, ejecuta antes:

```bash
source /etc/profile.d/SPACK2.sh
spack load /tjciufg /w6dgctr /3tjlhpm /lnsshyr /ovya43b /sqvlzkq /pbvefzl /kxsb4uw
```

No ejecutes `source activate.sh` hasta que `./install.sh` termine mostrando
`CRPropa instalado en: ...`; ese archivo se crea al final de una instalacion
correcta.

## Activacion

Despues de instalar:

```bash
source /ruta/al/bundle/CRPropa3/activate.sh
```

Esto define:

- `CRPROPA_DATA_PATH`
- `LD_LIBRARY_PATH`
- `PYTHONPATH`
- `PATH`

## Pruebas Python

Los scripts de comprobacion estan en `CRPropa3/build/`:

- `Velocity_test.py`
- `PropagationBP_test.py`
- `LarmorLowEnergy_test.py`
- `ICSThomsonMFP_test.py`
- `TravelTime_test.py`

Ejecutalos desde `build` despues de activar el entorno:

```bash
cd /ruta/al/bundle/CRPropa3/build
python3 Velocity_test.py
python3 PropagationBP_test.py
python3 LarmorLowEnergy_test.py
python3 ICSThomsonMFP_test.py
python3 TravelTime_test.py
```

Todos incluyen `1 eV` en sus barridos de comprobacion. Las pruebas de Larmor
usan `B = 1e-3 nG` para que el radio a `1 eV` sea numericamente resoluble.

## Flujo del bundle

1. `install.sh` copia `../CRPropa3-data/data/` a `CRPropa3/build/data/`.
2. `cmake` compila CRPropa en `CRPropa3/build`.
3. `cmake --install` instala `lib/`, `include/` y `share/crpropa/` dentro de
   `CRPropa3/`.
4. Los scripts de prueba se mantienen en `CRPropa3/build/`.

El bundle esta pensado para compilar en la maquina destino. No incluye binarios
externos de ACME para evitar problemas de compatibilidad de arquitectura y
bibliotecas.
