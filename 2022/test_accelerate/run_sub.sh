#!/bin/bash -e

conda activate test_accelerate

export CUDA_VERSION=11.3
conda install pytorch cudatoolkit=$CUDA_VERSION -c pytorch -y

pip install "transformers==4.15.0" 
pip install "datasets==1.14.0" 
pip install "wrapt" "flatbuffers"
# This must go last or else we install a very old incompatible version of the hub
pip install "huggingface_hub==0.4.0"
#pip install "accelerate==0.9.0" 
pip install "accelerate"

echo "Installation finished!"

BERT_MODEL=bert-large-uncased
MAX_TRAIN_SAMPLES=20000
MAX_EVAL_SAMPLES=10000
BATCH_SIZE=8
SEED=0
OUTPUT_PREF=output_res

rm -f run_qa_no_trainer.py* utils_qa.py*

wget https://raw.githubusercontent.com/huggingface/transformers/v4.15.0/examples/pytorch/question-answering/run_qa_no_trainer.py
wget https://raw.githubusercontent.com/huggingface/transformers/v4.15.0/examples/pytorch/question-answering/utils_qa.py

# The first run is on a single GPU and without gradient accumulation
python run_qa_no_trainer.py \
  --max_train_samples $MAX_TRAIN_SAMPLES \
  --max_eval_samples $MAX_EVAL_SAMPLES \
  --model_name_or_path bert-large-uncased \
  --per_device_train_batch_size $BATCH_SIZE \
  --gradient_accumulation_steps 1 \
  --learning_rate 3e-5 \
  --seed $SEED \
  --dataset_name squad \
  --max_seq_length 384 \
  --doc_stride 128  \
  --output_dir ${OUTPUT_PREF}_1gpu 2>&1|tee 1gpu_run.log

for grad_accum_steps in 1 2 4 8 16 ; do
  accelerate launch  run_qa_no_trainer.py \
  --max_train_samples $MAX_TRAIN_SAMPLES \
  --max_eval_samples $MAX_EVAL_SAMPLES \
  --model_name_or_path bert-large-uncased \
  --per_device_train_batch_size $BATCH_SIZE \
  --gradient_accumulation_steps $grad_accum_steps \
  --learning_rate 3e-5 \
  --seed $SEED \
  --dataset_name squad \
  --max_seq_length 384 \
  --doc_stride 128  \
  --output_dir ${OUTPUT_PREF}_accum_steps_${grad_accum_steps} 2>&1|tee mult_gpus_accum_steps_${grad_accum_steps}_run.log
done
