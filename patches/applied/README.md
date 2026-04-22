# TextOutput precision bundle

Includes: everything in a single zip:

- `0001-textoutput-precision.patch`
  - adds `TextOutput::setPrecision(int)`
  - adds `TextOutput::getPrecision() const`
  - keeps the default precision at `5`
  - allows printing with higher precision in text output, for example `17`

- `0002-textoutput-python-test.patch`
  - adds a Python test that checks that `setPrecision/getPrecision` are available from Python

## Note on Python bindings

There is no need to patch `python/2_headers.i` to expose these methods.  
`TextOutput.h` is already included by SWIG through:

`%include "crpropa/module/TextOutput.h"`

so, after rebuilding the Python extension, the new public methods become available automatically.

## Applying the patches

```bash
git apply patches/0001-textoutput-precision.patch
git apply patches/0002-textoutput-python-test.patch
```

## Usage in C++

```cpp
crpropa::TextOutput out("output.txt");
out.setPrecision(17);
```

## Usage in Python

```python
import crpropa as crp
out = crp.TextOutput("output.txt")
out.setPrecision(17)
```

## Verification

```bash
ctest -R testOutput --output-on-failure
ctest -R testPythonExtension --output-on-failure
```
