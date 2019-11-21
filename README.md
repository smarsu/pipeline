# Pipeline
A std of pipeline.

## The Mind
- Limit: Consider the pipeline to be one line and just forward.
- featurs:
    - Forward pipeline
    - Mutable batch size for invoke

## API

### 1. ForwardPipeline
- Constructe a forward pipeline.
```
ForwardPipeline pipeline = pipeline::ForwardPipeline;
```

### 2. addTask
- Add a task with num threads.
```
shared_tsque(Ret) addTask(int num_threads,
                          Ret (*func)(Args...),
                          shared_tsque(T) in,
                          TArgs... rest)
```

### 3. addTaskStackBatch
- Add a task with num threads.
- Load list of datas in inque, put one data to outque.
```
shared_tsque(Ret) addTaskStackBatch(int num_threads,
                                    Ret (*func)(Args...),
                                    shared_tsque(T) in,
                                    int batch_size,
                                    TArgs... rest)
```

### 4. addTaskSplitBatch
- Add a task with num threads.
- Load one data for inque (it should be a vector), put list of datas to outque.
```
shared_tsque(Ret) addTaskSplitBatch(int num_threads,
                                    std::vector<Ret> (*func)(Args...),
                                    shared_tsque(T) in,
                                    TArgs... rest)
```

### 5. stackPipe
- Stack the datas for different pipe to one single pipe.
```
shared_tsque(T) stackPipe(int num_threads, 
                          std::vector<shared_tsque(T)> in)
```
