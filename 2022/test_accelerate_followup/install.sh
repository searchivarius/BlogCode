#!/bin/bash -e

export CUDA_VERSION=11.3
conda install pytorch cudatoolkit=$CUDA_VERSION -c pytorch -y

pip install "transformers==4.21.1" 
pip install "datasets"
pip install "evaluate"
pip install "wrapt" "flatbuffers"
pip install "huggingface_hub"
pip install "accelerate"

echo "Installation finished!"
