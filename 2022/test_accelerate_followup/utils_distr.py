#
#   Distributed training utils based on code in FlexNeuART framework, 
#   which is itself distributed (no pun intended) under Apache 2 license.
#
#   https://github.com/oaqa/FlexNeuART
#
#
import torch
import torch.distributed as dist

"""
  A dummy autocast class allowing for easy enabling/disabling of AMP.
"""
class DummyAutoCast:

    def __enter__(self):
        return self
    def __exit__(self, type, value, tb):
        pass


"""
  A dummy gradient scaler class allowing for easy enabling/disabling of AMP.
"""
class DummyGradScaler:
    # just return the unscaled loss
    def scale(self, loss):
        return loss

    # just do the optimizer step without any changes.
    def step(self, optimizer):
        optimizer.step()

    # clearly nothing to update here
    def update(self):
        pass


"""
  There's "enabled" flag in autocast and GradScaler, however, 
  it is not clear if using it has no effect (as we need). 
  To ensure autocast is not involved at all, we introduce these dummy classes.

  As an additional benefit, this,  permits using older version of 
  Pytorch that have no built-in amp. 
  
  Thus, if one does not have amp or you does not want to use it,
  we do not have to use different training code.

"""
def get_amp_processors(enabled):
    if enabled:
      from torch.cuda.amp import autocast, GradScaler
      return autocast, GradScaler()
    else:
      return DummyAutoCast, DummyGradScaler()


def avg_model_params(model, amp):
    """
       Average model parameters across all GPUs.
       Set amp to True, to enable automatic mixed-precision.
    """
    auto_cast_class, scaler = get_amp_processors(amp)
        
    with auto_cast_class():
        qty = float(dist.get_world_size())
        for prm in model.parameters():
            dist.all_reduce(prm.data, op=torch.distributed.ReduceOp.SUM)
            prm.data /= qty
    

def comp_max_step_sync_qty(data_loader, local_sgd_cycle_steps) :
    """
      Compute a (safe) number of synchronization steps.
    """
    return len(data_loader) // (local_sgd_cycle_steps * dist.get_world_size())


class AcceleratorLocalSGD:
    """
      A helper class to support local SGD on top of Accelerator.
    """
    def __enter__(self):
        self.model_sync_obj = self.model.no_sync()
        self.model_sync_obj.__enter__()
        return self

    def __exit__(self, type, value, tb):
        self.model_sync_obj.__exit__(type, value, tb)

    def __init__(self, accelerator, model, local_sgd_cycle_steps, max_step_sync_qty):
        self.accelerator = accelerator
        self.model = model
        self.step_qty = 0
        self.sync_qty = 0
        self.max_step_sync_qty = max_step_sync_qty
        self.local_sgd_cycle_steps = local_sgd_cycle_steps
    
    def step(self):
        self.step_qty += 1 
        # Each process needs to run exactly the same number of synchronization steps
        if self.step_qty % (self.local_sgd_cycle_steps) == 0:
            if self.sync_qty < self.max_step_sync_qty:
                self.sync_qty += 1
                self.accelerator.wait_for_everyone()
                avg_model_params(self.model, amp=self.accelerator.use_fp16)
