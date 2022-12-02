#!/bin/bash -e

conda activate test_accelerate

export CUDA_VERSION=11.3
conda install pytorch cudatoolkit=$CUDA_VERSION -c pytorch -c nvidia -y

pip install "transformers==4.21.1" 
pip install "datasets==2.6.1"
pip install "evaluate"
pip install "scipy"
pip install "scikit-learn"
pip install "wrapt" "flatbuffers"
pip install "huggingface_hub"
pip install "accelerate"

echo "Installation finished!"
