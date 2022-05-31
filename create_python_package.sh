#!/bin/bash

deactivate
rm -rf .venv

virtualenv .venv
source .venv/bin/activate
pip install pandas numpy
cd tools/pythonpkg || exit
./clean.sh

python3 setup.py install
