#!/bin/bash

PROC_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && cd processing && pwd )"

math -script ${PROC_DIR}/New_ICs.m
