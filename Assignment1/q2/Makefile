CC = gcc
CFLAGS = -O3 -fopenmp -Wall -g

seq_col:
	$(CC) $(CFLAGS)  seq_col_trs.c -o seq_col_trs

par_col:
	$(CC) $(CFLAGS)  par_col_major_trs.c -o par_col_major_trs
	$(CC) $(CFLAGS)  par_col_inner_trs.c -o par_col_inner_trs

seq_row:
	$(CC) $(CFLAGS)  seq_row_trs.c -o seq_row_trs

par_row:
	$(CC) $(CFLAGS)  par_row_inner_trs.c -o par_row_inner_trs

log_clean:
	rm -rf *.out.*

exec_clean:
	rm -rf seq_col_trs seq_row_trs par_col_inner_trs par_col_major_trs par_row_inner_trs
