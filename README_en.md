# The Standard of Pipeline

## The Mind
### version 0.0.1
- Limit: Consider the pipeline to be one line and just forward.
- Three main part of the pipeline:
    - ADD_TASK(func, n_threads)
    > Add a task to the pipeline. 
    - STACK_DATA(batch_size)
    > Stack data for a batch invoke.
    - SPLIT_DATA()
    > Split data for data post process.
- featurs:
    - Forward pipeline
    - Mutable batch size for invoke
