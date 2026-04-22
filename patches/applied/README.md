# TextOutput precision bundle

Incluye todo en un mismo zip:

- `0001-textoutput-precision.patch`
  - añade `TextOutput::setPrecision(int)`
  - añade `TextOutput::getPrecision() const`
  - mantiene la precisión por defecto en `5`
  - permite imprimir con mayor precisión en texto, por ejemplo `17`

- `0002-textoutput-python-test.patch`
  - añade un test Python que comprueba que `setPrecision/getPrecision` están disponibles desde Python

## Nota sobre Python bindings

No hace falta parchear `python/2_headers.i` para exponer estos métodos.
`TextOutput.h` ya está incluido desde SWIG con:

`%include "crpropa/module/TextOutput.h"`

por lo que, tras recompilar la extensión Python, los nuevos métodos públicos quedan disponibles automáticamente.

## Aplicación

```bash
git apply patches/0001-textoutput-precision.patch
git apply patches/0002-textoutput-python-test.patch
```

## Uso en C++

```cpp
crpropa::TextOutput out("output.txt");
out.setPrecision(17);
```

## Uso en Python

```python
import crpropa as crp
out = crp.TextOutput("output.txt")
out.setPrecision(17)
```

## Verificación

```bash
ctest -R testOutput --output-on-failure
ctest -R testPythonExtension --output-on-failure
```
