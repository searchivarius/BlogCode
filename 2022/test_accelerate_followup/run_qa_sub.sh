#!/bin/bash -e

set -o pipefail

conda activate test_accelerate

BERT_MODEL=bert-large-uncased
BATCH_SIZE=8

BASE_LR=3e-5
gpu_qty=$(nvidia-smi -L|wc -l)
gpu_adjust_lr=`python -c "print($BASE_LR*$gpu_qty)"`

if [ ! -d "results_qa" ] ; then
    mkdir -p "results_qa"
fi

for MAX_TRAIN_SAMPLES in 4000 40000 ; do
    OUTPUT_PREF=results_qa/output_res_${MAX_TRAIN_SAMPLES}

    for SEED in 0 1 2 ; do
        out_dir=${OUTPUT_PREF}_1gpu/$SEED/
        rm -r -f $out_dir
        mkdir -p $out_dir
        python run_qa_no_trainer.py \
          --force_bf16 \
          --max_train_samples $MAX_TRAIN_SAMPLES \
          --model_name_or_path bert-large-uncased \
          --per_device_train_batch_size $BATCH_SIZE \
          --gradient_accumulation_steps 1 \
          --learning_rate $BASE_LR \
          --seed $SEED \
          --dataset_name squad \
          --max_seq_length 384 \
          --doc_stride 128  \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log
    done
    
    for SEED in 0 1 2 ; do

        # These runs for non-synchronous gradient descent
        for local_sgd_steps in 1 2 4 8 16 32 64 128 256 ; do
            out_dir=${OUTPUT_PREF}_nosync_steps_${local_sgd_steps}/$SEED
            rm -r -f $out_dir
            mkdir -p $out_dir
    
            # The first run is on a single GPU and without gradient accumulation
            accelerate launch run_qa_no_trainer_local_sgd.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --local_sgd_steps $local_sgd_steps \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $BATCH_SIZE \
              --learning_rate $gpu_adjust_lr \
              --seed $SEED \
              --dataset_name squad \
              --max_seq_length 384 \
              --doc_stride 128  \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done

        for grad_accum_steps in 1 2 4 8 16 32 64 128 256 ; do
            out_dir=${OUTPUT_PREF}_accum_steps_${grad_accum_steps}/$SEED/
            rm -r -f $out_dir
            mkdir -p $out_dir
            accelerate launch  run_qa_no_trainer.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $BATCH_SIZE \
              --gradient_accumulation_steps $grad_accum_steps \
              --learning_rate $gpu_adjust_lr \
              --seed $SEED \
              --dataset_name squad \
              --max_seq_length 384 \
              --doc_stride 128  \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done
    
    done

done
