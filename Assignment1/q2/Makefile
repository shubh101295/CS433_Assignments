seq_col:
	gcc -O3 -fopenmp -Wall -g  seq_col_trs.c -o seq_col_trs
	gcc -O3 -fopenmp -Wall -g  seq_col_trs_simd.c -o seq_col_trs_simd

par_col:
	gcc -O3 -fopenmp -Wall -g  par_col_major_trs.c -o par_col_major_trs
	gcc -O3 -fopenmp -Wall -g  par_col_inner_trs.c -o par_col_inner_trs

seq_row:
	gcc -O3 -fopenmp -Wall -g  seq_row_trs.c -o seq_row_trs
	gcc -O3 -fopenmp -Wall -g  seq_row_trs_simd.c -o seq_row_trs_simd

par_row:
	gcc -O3 -fopenmp -Wall -g  par_row_inner_trs.c -o par_row_inner_trs

log_clean:
	rm -rf *.out.*

exec_clean:
	rm -rf seq_trs seq_col_trs seq_col_trs_simd seq_row_trs seq_row_trs_simd par_col_inner_trs par_col_major_trs par_row_inner_trs
