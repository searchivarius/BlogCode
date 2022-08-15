#!/bin/bash -e

conda activate test_accelerate

set -o pipefail

BERT_MODEL=bert-large-uncased
# CHANGE ME IF NECESSARY
MAX_TRAIN_SAMPLES=20000
MAX_EVAL_SAMPLES=10000
# CHANGE ME IF NECESSARY
BATCH_SIZE=8
SEED=0
OUTPUT_PREF=output_res

GPU_QTY=$(nvidia-smi -L|wc -l)
BASE_LR=3e-5


for grad_accum_steps in 8 32 64 128 ; do
    for adjust_coeff in 64 32 8 2 1 0.5 0.125 .03125 .015625 ; do 
        curr_lr=`python -c "print($BASE_LR*$adjust_coeff)"`
        echo "LR: $curr_lr"
        accelerate launch  run_qa_no_trainer.py \
          --max_train_samples $MAX_TRAIN_SAMPLES \
          --max_eval_samples $MAX_EVAL_SAMPLES \
          --model_name_or_path bert-large-uncased \
          --per_device_train_batch_size $BATCH_SIZE \
          --gradient_accumulation_steps $grad_accum_steps \
          --learning_rate $curr_lr \
          --seed $SEED \
          --dataset_name squad \
          --max_seq_length 384 \
          --doc_stride 128  \
          --output_dir ${OUTPUT_PREF}_accum_steps_${grad_accum_steps}_lr_${curr_lr}  2>&1|tee mult_gpus_accum_steps_${grad_accum_steps}_lr_${curr_lr}_run.log
    done
done
