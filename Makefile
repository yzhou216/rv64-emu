PROG = rv64_emu
OBJS = rv64_emu.o harness.o rv_emu.o exec.o alu.o    \
       control.o analysis.o bits.o cache.o verbose.o \
       fib_rec_c.o fib_rec_s.o                       \
       find_max_index_c.o find_max_index_s.o         \
       get_bitseq_c.o get_bitseq_s.o                 \
       is_pal_rec_c.o is_pal_rec_s.o                 \
       max3_c.o max3_s.o                             \
       str_to_int_c.o str_to_int_s.o                 \
       int_to_str_c.o int_to_str_s.o                 \
       midpoint_c.o midpoint_s.o                     \
       quadratic_c.o quadratic_s.o                   \
       sort_c.o sort_s.o                             \
       to_upper_c.o to_upper_s.o

# Find the sources under src and the test programs in progs
VPATH = src src/emu src/cache progs

HEADERS = analysis.h exec.h harness.h progs.h rv_emu.h

%.o: %.c $(HEADERS)
	gcc -g -Isrc -Iprogs -c -o $@ $<

%.o: %.s
	as -g -o $@ $<

$(PROG): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(PROG) $(OBJS)
