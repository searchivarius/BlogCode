#!/bin/bash -e

set -o pipefail

conda activate test_accelerate

TASK=mnli
EPOCHS=3 
MAX_SEQ_LEN=128
BERT_MODEL=bert-large-uncased

BATCH_SIZE=32
BASE_LR=2e-5

gpu_qty=$(nvidia-smi -L|wc -l)
gpu_adjust_lr=`python -c "print($BASE_LR*$gpu_qty)"`

if [ ! -d "results_mnli" ] ; then
    mkdir -p "results_mnli"
fi


for MAX_TRAIN_SAMPLES in 4000 40000 ; do
    OUTPUT_PREF=results_$TASK/output_res_${MAX_TRAIN_SAMPLES}
 
    for SEED in 0 1 2 ; do

        # These runs for non-synchronous gradient descent
        for local_sgd_steps in 1 2 4 8 16 32 64 128 256 ; do
            out_dir=${OUTPUT_PREF}_nosync_steps_${local_sgd_steps}/$SEED
            rm -r -f $out_dir
            mkdir -p $out_dir

            accelerate launch  run_glue_no_trainer_local_sgd.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $BATCH_SIZE \
              --gradient_accumulation_steps 1 \
              --local_sgd_steps $local_sgd_steps \
              \
              --task_name $TASK \
              \
              --learning_rate $BASE_LR \
              --seed $SEED \
              --num_train_epochs $EPOCHS \
              \
              --max_seq_length $MAX_SEQ_LEN \
              \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
    
        done

        for grad_accum_steps in 1 2 4 8 16 32 64 128 256 ; do
            out_dir=${OUTPUT_PREF}_accum_steps_${grad_accum_steps}/$SEED/
            rm -r -f $out_dir
            mkdir -p $out_dir

            accelerate launch  run_glue_no_trainer.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $BATCH_SIZE \
              --gradient_accumulation_steps $grad_accum_steps \
              \
              --task_name $TASK \
              \
              --learning_rate $BASE_LR \
              --seed $SEED \
              --num_train_epochs $EPOCHS \
              \
              --max_seq_length $MAX_SEQ_LEN \
              \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done
    done
   

    for SEED in 0 1 2 ; do
        out_dir=${OUTPUT_PREF}_1gpu/$SEED/
        rm -r -f $out_dir
        mkdir -p $out_dir
        python run_glue_no_trainer.py \
          --force_bf16 \
          --max_train_samples $MAX_TRAIN_SAMPLES \
          --model_name_or_path bert-large-uncased \
          --per_device_train_batch_size $BATCH_SIZE \
          --gradient_accumulation_steps 1 \
          \
          --task_name $TASK \
          \
          --learning_rate $BASE_LR \
          --seed $SEED \
          --num_train_epochs $EPOCHS \
          \
          --max_seq_length $MAX_SEQ_LEN \
          \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log
    done
done
