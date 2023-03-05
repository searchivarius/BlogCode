#!/bin/bash -e

set -o pipefail

conda activate test_accelerate

TASK=mnli
EPOCHS=3 
MAX_SEQ_LEN=128
BERT_MODEL=bert-large-uncased

BATCH_SIZE=32

gpu_qty=$(./print_accelerate_conf.sh |grep num_processes|cut -d \  -f 2)
check_r=$(($BATCH_SIZE % $gpu_qty))
if [ "$check_r" != "0" ] ; then
  echo "The total batch size is not a multiple of the number of GPUs!"
  exit 1
fi
# Let us use split_batches=True instead
adjusted_batch_size=$BATCH_SIZE
#adjusted_batch_size=$(($BATCH_SIZE/$gpu_qty))
#echo "Batch size adjusted for the number of GPUs (only for gradient accumulation with multiple GPUs) : $adjusted_batch_size"


if [ ! -d "results_mnli_lr" ] ; then
    mkdir -p "results_mnli_lr"
fi


for MAX_TRAIN_SAMPLES in 4000 40000 ; do
  for SEED in 0 1 2 ; do
    for curr_lr in 0.5e-6 1e-5 2e-5 4e-5 8e-5 1.6e-4 ; do
        OUTPUT_PREF=results_mnli_lr/lr_${curr_lr}/output_res_${MAX_TRAIN_SAMPLES}
    
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
          --learning_rate $curr_lr \
          --seed $SEED \
          --num_train_epochs $EPOCHS \
          \
          --max_seq_length $MAX_SEQ_LEN \
          \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log

        # These runs for non-synchronous gradient descent
        for local_sgd_steps in 1 ; do
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
              --learning_rate $curr_lr \
              --seed $SEED \
              --num_train_epochs $EPOCHS \
              \
              --max_seq_length $MAX_SEQ_LEN \
              \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
    
        done

        for grad_accum_steps in 1 ; do
            out_dir=${OUTPUT_PREF}_accum_steps_${grad_accum_steps}/$SEED/
            rm -r -f $out_dir
            mkdir -p $out_dir

            accelerate launch  run_glue_no_trainer.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $adjusted_batch_size \
              --gradient_accumulation_steps $grad_accum_steps \
              \
              --task_name $TASK \
              \
              --learning_rate $curr_lr \
              --seed $SEED \
              --num_train_epochs $EPOCHS \
              \
              --max_seq_length $MAX_SEQ_LEN \
              \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done
    done
  done
done