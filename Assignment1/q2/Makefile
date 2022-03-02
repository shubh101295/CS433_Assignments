seq_col:
	gcc -O3 -fopenmp -Wall -g  seq_col_trs.c -o seq_col_trs
	gcc -O3 -fopenmp -Wall -g  seq_col_trs_simd.c -o seq_col_trs_simd

seq_row:
	gcc -O3 -fopenmp -Wall -g  seq_row_trs.c -o seq_row_trs
	gcc -O3 -fopenmp -Wall -g  seq_row_trs_simd.c -o seq_row_trs_simd

log_clean:
	rm -rf *.out.*

exec_clean:
	rm -rf seq_trs seq_col_trs seq_col_trs_simd seq_row_trs seq_row_trs_simd
