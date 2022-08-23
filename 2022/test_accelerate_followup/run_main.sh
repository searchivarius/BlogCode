#!/bin/bash -e
set -o pipefail

conda create -n test_accelerate python=3.8 -y

conda init bash

bash -i run_sub.sh


