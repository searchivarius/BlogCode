#!/bin/bash -e

conda create -n test_accelerate python=3.8 -y

conda init bash

bash -i install_sub.sh

