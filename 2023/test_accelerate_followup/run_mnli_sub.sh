#!/bin/bash -e

set -o pipefail

conda activate test_accelerate

TASK=mnli
EPOCHS=3 
MAX_SEQ_LEN=128
BERT_MODEL=bert-large-uncased

BATCH_SIZE=32

ONE_GPU_LR=2e-5

LOCAL_SGD_LR=4e-5
GRAD_ACCUM_LR=4e-5

gpu_qty=$(./print_accelerate_conf.sh |grep num_processes|cut -d \  -f 2)
check_r=$(($BATCH_SIZE % $gpu_qty))
if [ "$check_r" != "0" ] ; then
  echo "The total batch size is not a multiple of the number of GPUs!"
  exit 1
fi


if [ ! -d "results_mnli" ] ; then
    mkdir -p "results_mnli"
fi


OUTPUT_ROOT=results_$TASK/
for MAX_TRAIN_SAMPLES in 4000 40000 ; do
    OUTPUT_PREF=output_res_${MAX_TRAIN_SAMPLES}
 
    for SEED in 0 1 2 ; do

        out_dir=${OUTPUT_ROOT}/1gpu/output_res_${MAX_TRAIN_SAMPLES}/$SEED/
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
          --learning_rate $ONE_GPU_LR \
          --seed $SEED \
          --num_train_epochs $EPOCHS \
          \
          --max_seq_length $MAX_SEQ_LEN \
          \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log

        # These runs for non-synchronous gradient descent
        for local_sgd_steps in 1 2 4 8 16 32 64 ; do
            out_dir=${OUTPUT_ROOT}/${gpu_qty}gpus/output_res_${MAX_TRAIN_SAMPLES}_nosync_steps_${local_sgd_steps}/$SEED
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
              --learning_rate $LOCAL_SGD_LR \
              --seed $SEED \
              --num_train_epochs $EPOCHS \
              \
              --max_seq_length $MAX_SEQ_LEN \
              \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
    
        done

        for grad_accum_steps in 1 2 4 8 16 32 64 ; do
            out_dir=${OUTPUT_ROOT}/${gpu_qty}gpus/output_res_${MAX_TRAIN_SAMPLES}_accum_steps_${grad_accum_steps}/$SEED
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
              --learning_rate $GRAD_ACCUM_LR \
              --seed $SEED \
              --num_train_epochs $EPOCHS \
              \
              --max_seq_length $MAX_SEQ_LEN \
              \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done
    done
   
done
