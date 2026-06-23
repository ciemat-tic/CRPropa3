#!/usr/bin/env bash
export CRPROPA_HOME="/home/ciemat-t13/CRPropa_bundle/CRPropa3"
export CRPROPA_DATA_PATH="${CRPROPA_HOME}/share/crpropa"
export LD_LIBRARY_PATH="${CRPROPA_HOME}/lib:${LD_LIBRARY_PATH:-}"
export PYTHONPATH="${CRPROPA_HOME}/lib/python3.12/site-packages:${PYTHONPATH:-}"
export PATH="${CRPROPA_HOME}/bin:${PATH:-}"
