# Conjunto de parches físicos de velocidad exacta

## Propósito

Este conjunto de parches mejora el tiempo de ejecución de CRPropa pasando de una interpretación estrictamente ultra-relativista a un tratamiento de velocidad finita de partículas masivas siempre que ese cambio sea físicamente significativo.

Históricamente, varias partes del tiempo de ejecución asumían implícitamente una o varias de las siguientes relaciones:

- `v = c`
- `p = E / c`
- `R = E / |q|`
- `dt = ds / c`

Esas aproximaciones son excelentes en el régimen UHE estándar, pero no son exactas para partículas masivas fuera del límite estrictamente ultra-relativista. La serie de parches aplicada introduce las relaciones exactas correspondientes en los módulos de tiempo de ejecución donde importan:

- `beta = sqrt(1 - m^2 c^4 / E^2)`
- `v = beta c`
- `p = sqrt(E^2 - m^2 c^4) / c`
- `R = p c / |q|`
- `dt = ds / v`

Para partículas sin masa se conservan los límites esperados:

- `beta = 1`
- `p = E / c`

El objetivo práctico de la serie no es convertir CRPropa en un código genérico de transporte a baja energía, sino eliminar del runtime las suposiciones implícitas de `v = c` en el tiempo, la rigidez, el momento, la longitud de decaimiento y otras magnitudes cinemáticas relacionadas.

**Parches aplicados para esta sección**

- Alcance global de la serie, no un único parche
- El propósito se implementa de forma colectiva por los parches `0001` a `0017`

## Estrategia general de diseño

Las modificaciones se implementaron de forma conservadora.

1. La API legacy histórica se conservó siempre que fue posible.
2. Primero se introdujeron métodos auxiliares exactos en `ParticleState`.
3. Los módulos de tiempo de ejecución se actualizaron sólo allí donde la cinemática exacta de velocidad finita es físicamente relevante.
4. El comportamiento ultra-relativista estándar sigue recuperándose en el límite apropiado.
5. El estado final se validó con tests unitarios en C++, tests en Python y comparaciones de tipo regresión.

El diseño evita deliberadamente cambiar silenciosamente todos los módulos históricos. Cuando un nuevo comportamiento físico podía afectar a la compatibilidad hacia atrás, se prefirió una alternativa optativa en lugar de sustituir el módulo legacy de forma incondicional.

**Parches aplicados para esta sección**

- Estrategia global de toda la serie de velocidad exacta
- Validación y archivos de soporte en el directorio de parches aplicados
- Esta sección resume cómo se aplicaron conjuntamente los parches `0001` a `0017`

## Cambios físicos principales

### 1. Cinemática exacta de partículas en `ParticleState`

`ParticleState` proporciona ahora métodos auxiliares cinemáticos exactos para partículas masivas además de los métodos auxiliares ultra-relativistas históricos.

Los nuevos auxiliares exactos incluyen:

- `getBeta()`
- `getVelocityExact()`
- `getMomentumExact()`
- `getRigidityExact()`
- `getProperVelocity()` como helper para partículas masivas

Los auxiliares legacy siguen presentes por compatibilidad, pero ahora deben interpretarse como aproximaciones históricas y no como física exacta del runtime.

Físicamente, ésta es la base de toda la refactorización. El resto de la serie usa estos helpers para reconstruir la velocidad finita de la partícula, el momento exacto y la rigidez exacta a partir de la energía total y de la masa en reposo.

**Parches aplicados para esta sección**

- `0001-particle-state.patch`

### 2. Tiempo de vuelo exacto en `Candidate`

`Candidate` usa ahora la velocidad exacta de la partícula al convertir la longitud geométrica recorrida en tiempo transcurrido.

Esto sustituye la conversión implícita histórica

- `dt = ds / c`

por

- `dt = ds / v`

Esto afecta tanto al tiempo actual del candidato como al tiempo retrotraído asignado a secundarias creadas en posiciones desplazadas.

Físicamente, esto importa siempre que la partícula sea masiva y no estrictamente ultra-relativista. Es uno de los lugares más claros donde la antigua aproximación `v = c` se propagaba directamente hacia magnitudes observables.

**Parches aplicados para esta sección**

- `0002-candidate.patch`

### 3. Propagación exacta basada en momento en `PropagationCK`

`PropagationCK` se refactorizó para que la integración en espacio de fases de Cash–Karp use dinámica exacta de velocidad finita.

En su forma final, este módulo es el lugar natural de la refactorización donde se usa propagación exacta basada en momento. La variable de espacio de fases se trata de forma consistente con la energía total exacta y con el momento exacto de la partícula, y el movimiento geométrico se deriva de la velocidad exacta de la partícula en lugar de la aproximación histórica basada en `c`.

Físicamente, éste es el propagador de cinemática exacta más limpio de toda la serie.

**Parches aplicados para esta sección**

- `0007-propagation-ck.patch`

### 4. Corrección con momento exacto del propagador de Boris en `PropagationBP`

`PropagationBP` requirió un diseño final diferente de `PropagationCK`.

Una reformulación directa en espacio completo de momento no conservó el comportamiento pretendido de la implementación histórica de Boris. En particular, condujo a problemas en el control adaptativo del paso y en la ruta de optimización de paso fijo.

La solución final aplicada conserva por ello la estructura geométrica de Boris:

- propagación en dirección unitaria
- longitud geométrica como variable principal de paso

pero sustituye la aproximación ultra-relativista implícita en el coeficiente de rotación magnética por el módulo exacto del momento.

En otras palabras, `PropagationBP` sigue siendo un propagador de tipo Boris en espacio de direcciones, mientras que el coeficiente que históricamente se comportaba como si `p = E / c` se construye ahora a partir del momento exacto.

Esto es físicamente importante porque conserva el carácter geométrico robusto de la implementación original de Boris y al mismo tiempo elimina la aproximación de momento no exacta.

**Parches aplicados para esta sección**

- `0008-propagation-bp.patch`
- `0014-propagation-bp-step-units.patch`
- `0015-propagation-bp-boris-direction-exact-momentum.patch`

### 5. Reconstrucción exacta del momento en `MomentumDiffusion`

`MomentumDiffusion` ya no depende internamente de la identidad ultra-relativista `E = p c`.

En su lugar, el módulo evoluciona el momento exacto y reconstruye la energía total mediante la relación relativista exacta. Esto importa principalmente en el régimen moderadamente relativista. En el límite estrictamente ultra-relativista, el código modificado converge de forma natural hacia el comportamiento histórico.

Físicamente, este parche es importante porque la difusión en momento es uno de los lugares donde usar `E/c` como si fuera siempre el momento puede sesgar silenciosamente el resultado fuera del límite UHE.

**Parches aplicados para esta sección**

- `0003-momentum-diffusion.patch`

### 6. Velocidad y rigidez exactas en `DiffusionSDE`

`DiffusionSDE` se actualizó para que la conversión temporal y las magnitudes dependientes de la rigidez usen cinemática exacta de velocidad finita cuando corresponde.

El punto clave es evitar mezclar:

- longitud geométrica de paso
- paso temporal interno
- conversiones históricas basadas en `c`

Esta parte de la serie requirió varios ajustes posteriores, porque el módulo mezcla geometría de transporte, propagación estocástica y física dependiente de la rigidez de una forma sensible a las aproximaciones ultra-relativistas implícitas.

**Parches aplicados para esta sección**

- `0009-diffusion-sde.patch`
- `0012-propagation-diffusionsde-fixes.patch`
- `0013-followup-cleanups.patch`

### 7. Condición exacta de corte por rigidez mínima

La condición de corte por rigidez mínima usa ahora la rigidez exacta

- `R = p c / |q|`

en lugar de la aproximación ultra-relativista histórica

- `R = E / |q|`

Esto es físicamente relevante porque la rigidez es una de las variables de control más importantes en transporte y difusión. Usar `E / |q|` fuera del régimen estrictamente ultra-relativista sobreestima sistemáticamente la rigidez física de una partícula masiva.

**Parches aplicados para esta sección**

- `0013-followup-cleanups.patch`

### 8. Momento exacto y velocidad exacta en pérdidas sincrotrón y adiabáticas

Donde el runtime usaba históricamente la aproximación ultra-relativista del momento, el código modificado usa ahora el momento exacto o la velocidad exacta siempre que la fórmula física lo requiere.

Esto mejora la consistencia de:

- magnitudes relacionadas con sincrotrón
- enfriamiento adiabático dependiente del tiempo

El punto físico clave es que los términos de pérdida que dependen explícitamente del momento o del tiempo transcurrido no deberían heredar una aproximación implícita `v = c` salvo que el modelo esté pensado explícitamente como ultra-relativista.

**Parches aplicados para esta sección**

- `0004-synchrotron-radiation.patch`
- `0005-adiabatic-cooling.patch`

### 9. Momento exacto y rigidez exacta en `Acceleration`

`Acceleration` se actualizó para usar momento exacto y rigidez exacta en los lugares donde el runtime se apoyaba antes en métodos auxiliares ultra-relativistas.

Físicamente, esto importa porque los operadores de scattering y aceleración dependen a menudo de forma más natural del momento o de la rigidez que de la energía total, y la aproximación `p = E / c` no es exacta para partículas masivas fuera del límite estrictamente ultra-relativista.

**Parches aplicados para esta sección**

- `0006-acceleration.patch`

### 10. Longitud de decaimiento exacta de núcleos y partículas inestables

La longitud de decaimiento de núcleos inestables ya no se trata como si la partícula propagara siempre a `c`.

La escala espacial relevante se basa ahora en

- `gamma beta c tau0 = gamma v tau0`

que es la expresión físicamente correcta para una partícula masiva con velocidad finita.

Las tablas de decaimiento en sí siguen basándose en vidas medias propias y datos de branching. El cambio está en la conversión en tiempo de ejecución de vida media propia a escala espacial de decaimiento.

Ésta es una de las correcciones físicamente más directas de toda la serie, porque una longitud de decaimiento incorrecta es una consecuencia explícita de asumir que toda partícula viaja a la velocidad de la luz.

**Parches aplicados para esta sección**

- `0011-nuclear-decay.patch`

### 11. Aclaración semántica en las salidas para dirección, velocidad y momento

Las salidas de texto y las salidas estructuradas se actualizaron para que dirección, velocidad y momento no queden implícitamente confundidos cuando la cinemática exacta de velocidad finita está activada.

Esto es principalmente un cambio de diagnóstico y de reproducibilidad más que un cambio del propio modelo físico, pero es importante para interpretar correctamente los resultados después de la refactorización.

**Parches aplicados para esta sección**

- `0010-output.patch`

### 12. Módulos cosmológicos opcionales de redshift exacto

Los módulos históricos `Redshift` y `FutureRedshift` se conservan por compatibilidad hacia atrás.

Además, esta serie de parches introduce módulos opcionales exactos de redshift cosmológico:

- `ExactRedshift`
- `ExactFutureRedshift`

Estos módulos están pensados para partículas masivas fuera del régimen estrictamente ultra-relativista.

La idea clave es que el redshift cosmológico se aplique primero al momento físico y que la energía total se reconstruya después con la relación exacta. En la implementación aplicada esto se maneja mediante una actualización basada en momento dentro de los módulos exactos de redshift, en lugar de asumir que la energía total escala siempre como si la partícula fuera efectivamente sin masa.

Se trata deliberadamente de una extensión optativa. No sustituye silenciosamente al tratamiento cosmológico histórico.

**Parches aplicados para esta sección**

- `0016-exact-redshift-and-future.patch`
- `0017-exactredshift-test-include-cosmology.patch`

## Qué no se cambió de forma intencionada

Este conjunto de parches no pretende reescribir todo CRPropa como un código genérico de transporte a baja energía.

En particular:

- el módulo legacy `Redshift` se conservó
- los métodos auxiliares ultra-relativistas legacy de `ParticleState` se conservaron
- el conjunto de parches no certifica por sí mismo que todas las tablas de interacción precalculadas en `CRPropa3-data` sean válidas hasta energías arbitrariamente bajas

El objetivo principal del trabajo es la consistencia en tiempo de ejecución del transporte, del tiempo, de la rigidez, del momento, de la longitud de decaimiento y de otras magnitudes cinemáticas relacionadas.

**Parches aplicados para esta sección**

- No hay parche dedicado
- Esta sección documenta no-objetivos explícitos y comportamiento legacy preservado de toda la serie

## Impacto físico esperado

### En el régimen ultra-relativista

Se espera que el código modificado reproduzca el comportamiento histórico de CRPropa con gran precisión.

Esto incluye los casos de uso UHE estándar para los que las aproximaciones originales ya eran muy buenas.

### En el régimen moderadamente relativista o de velocidad finita

Se esperan diferencias principalmente en:

- tiempo de vuelo
- rigidez exacta
- longitudes de decaimiento de partículas y núcleos inestables
- módulos que usan magnitudes dependientes del momento o del tiempo
- redshift cosmológico de partículas masivas cuando se usan los módulos exactos opcionales

Estas diferencias suelen ser pequeñas en ejecuciones fuertemente ultra-relativistas y se vuelven más relevantes cuando la energía cinética de la partícula es comparable con la escala de su masa en reposo.

Físicamente, esto significa que el conjunto de parches es más relevante en la región de transición donde una partícula masiva no es ni no relativista ni tan energética como para que `beta` sea prácticamente indistinguible de uno.

**Parches aplicados para esta sección**

- Consecuencia de toda la serie de parches de velocidad exacta
- Parches aplicados especialmente relevantes:
  - `0001-particle-state.patch`
  - `0002-candidate.patch`
  - `0003-momentum-diffusion.patch`
  - `0007-propagation-ck.patch`
  - `0008-propagation-bp.patch`
  - `0009-diffusion-sde.patch`
  - `0011-nuclear-decay.patch`
  - `0016-exact-redshift-and-future.patch`

## Interpretación práctica final

Este conjunto de parches debe entenderse como una mejora de consistencia de velocidad finita del tiempo de ejecución de CRPropa.

No cambia la imagen física principal de CRPropa en el dominio ultra-relativista histórico, pero elimina varias suposiciones implícitas de `v = c` del runtime y hace que el tratamiento de partículas masivas sea más físicamente correcto siempre que importen las velocidades finitas de las partículas.

El punto más importante es que la serie mejora la consistencia física sin abandonar la compatibilidad hacia atrás ni el caso de uso UHE estándar que motivó originalmente el framework.

**Parches aplicados para esta sección**

- Consecuencia de toda la serie de parches de velocidad exacta
- Esta sección es un resumen práctico de todo el conjunto aplicado
