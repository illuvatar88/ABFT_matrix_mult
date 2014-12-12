
pcc = /home/arunkumar/x-tools/powerpc-e500v2-linux-gnuspe/bin/powerpc-e500v2-linux-gnuspe-g++
cc = g++
executable = launch
mvm_src = matrix_vector_mult_abft.cpp
tran_src = transpose_abft.cpp
flags = -static -g -O2 -fopenmp
defines = -D SIMPLIFI
sig_file = SimPLiFI_sighandlers.c

mvm:
	$(cc) $(flags) $(mvm_src) -o $(executable)

tran:
	$(cc) $(flags) $(tran_src) -o $(executable)

run_mvm: mvm
	./$(executable)

run_tran: tran
	./$(executable)

power_mvm:
	$(pcc) $(flags) $(sig_file) $(mvm_src) -o $(executable) $(defines)

power_tran:
	$(pcc) $(flags) $(sig_file) $(tran_src) -o $(executable) $(defines)