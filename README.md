# Sudoku_solver

First, get the data set zip file from the branch data.

```
git checkout data
git pull
```

unzip the data set with the README on the data branch

Then checkout the individual branches to compile and run the code

```
# the baseline
git checkout pthread_ver
# the pthread implemented global stack version
git checkout pthread_global_stack
# the pthread implemented local stack version
git checkout pthread_local_stack
# the OpenMP implemented local stack version
git checkout OpenMP_ver
```
