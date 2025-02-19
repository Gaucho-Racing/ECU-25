#!/bin/bash
set -e;

# Setup method to update the CSV files here

python3 CSV-25/grCSVtoH.py CSV-25/GR-ID.csv Application/Inc/grIDs.h
python3 CSV-25/msgCSVtoH.py CSV-25/MSG-ID.csv Application/Inc/msgIDs.h
