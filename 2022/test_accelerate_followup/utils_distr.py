#
#   Distributed training utils from FlexNeuART framework, which is 
#   itself distributed (no pun intended) under Apache 2 license.
#
#   https://github.com/oaqa/FlexNeuART
#
#


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
    

