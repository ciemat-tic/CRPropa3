# Conjuntos de parches aplicados

Este directorio contiene paquetes de parches que ya han sido aplicados a este fork de CRPropa.

## Paquetes disponibles

- `exact-velocity/`
  - Refactorización física que elimina varias aproximaciones ultra-relativistas implícitas del tiempo de ejecución e introduce cinemática exacta de velocidad finita allí donde es físicamente relevante.
- `textouput-precision/`
  - Mejora sólo de salida que hace configurable la precisión del texto. Este paquete no cambia el modelo físico.

## ¿Qué paquete cambia la física?

Los cambios físicos están documentados en:

- `exact-velocity/README.md`
- `exact-velocity/README.es.md`

Esos dos archivos describen la motivación física, las ecuaciones que se cambiaron, los módulos que se modificaron y el dominio de validez pretendido.

## Alcance de la documentación

La documentación en `exact-velocity/` está pensada para describir el estado final aplicado del código después de toda la serie de parches de velocidad exacta, incluyendo los parches correctivos posteriores y los módulos cosmológicos exactos opcionales de redshift.
