#!/bin/bash -e

set -o pipefail

conda activate test_accelerate

BERT_MODEL=bert-large-uncased
BATCH_SIZE=8

ONE_GPU_LR=3e-5

LOCAL_SGD_LR=6e-5
GRAD_ACCUM_LR=6e-5

gpu_qty=$(./print_accelerate_conf.sh |grep num_processes|cut -d \  -f 2)
check_r=$(($BATCH_SIZE % $gpu_qty))
if [ "$check_r" != "0" ] ; then
  echo "The total batch size is not a multiple of the number of GPUs!"
  exit 1
fi
# Let us use split_batches=True instead
adjusted_batch_size=$BATCH_SIZE

OUTPUT_ROOT="results_qa"
if [ ! -d $OUTPUT_ROOT ] ; then
    mkdir -p "$OUTPUT_ROOT"
fi

for MAX_TRAIN_SAMPLES in 4000 40000 ; do
    
    for SEED in 0 1 2 ; do
    
        out_dir=${OUTPUT_ROOT}/1gpu/output_res_${MAX_TRAIN_SAMPLES}/SEED/
        rm -r -f $out_dir
        mkdir -p $out_dir
        python run_qa_no_trainer.py \
          --force_bf16 \
          --max_train_samples $MAX_TRAIN_SAMPLES \
          --model_name_or_path bert-large-uncased \
          --per_device_train_batch_size $BATCH_SIZE \
          --gradient_accumulation_steps 1 \
          --learning_rate $ONE_GPU_LR \
          --seed $SEED \
          --dataset_name squad \
          --max_seq_length 384 \
          --doc_stride 128  \
          --output_dir $out_dir  2>&1|tee $out_dir/run.log

        # These runs for non-synchronous gradient descent
        for local_sgd_steps in 1 2 4 8 16 24 32 64 ; do
            out_dir=${OUTPUT_ROOT}/${gpu_qty}gpus/output_res_${MAX_TRAIN_SAMPLES}_nosync_steps_${local_sgd_steps}/$SEED
            rm -r -f $out_dir
            mkdir -p $out_dir
    
            # The first run is on a single GPU and without gradient accumulation
            accelerate launch run_qa_no_trainer_local_sgd.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --local_sgd_steps $local_sgd_steps \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $BATCH_SIZE \
              --learning_rate $LOCAL_SGD_LR \
              --seed $SEED \
              --dataset_name squad \
              --max_seq_length 384 \
              --doc_stride 128  \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done

        for grad_accum_steps in 1 2 4 8 16 24 32 64 ; do
            out_dir=${OUTPUT_ROOT}/${gpu_qty}gpus/output_res_${MAX_TRAIN_SAMPLES}_accum_steps_${grad_accum_steps}/$SEED
            rm -r -f $out_dir
            mkdir -p $out_dir
            accelerate launch  run_qa_no_trainer.py \
              --force_bf16 \
              --max_train_samples $MAX_TRAIN_SAMPLES \
              --model_name_or_path bert-large-uncased \
              --per_device_train_batch_size $adjusted_batch_size \
              --gradient_accumulation_steps $grad_accum_steps \
              --learning_rate $GRAD_ACCUM_LR \
              --seed $SEED \
              --dataset_name squad \
              --max_seq_length 384 \
              --doc_stride 128  \
              --output_dir $out_dir  2>&1|tee $out_dir/run.log
        done
    done

done
