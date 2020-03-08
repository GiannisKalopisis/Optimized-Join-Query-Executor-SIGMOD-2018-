IDIR=./include
CC=gcc
CFLAGS=-I$(IDIR) -Wall
LIBS=
VPATH = src INCLUDE

ODIR=./src/obj


$(shell mkdir -p ./src/obj)


_DEPS_MAIN2 = joinEnumeration.h stats.h general_functions.h relations.h result.h RHJ_stage1.h RHJ_stage2.h RHJ_stage3.h RHJ.h SQL_QueryParser.h predicateParser.h job_scheduler.h startup.h
DEPS_MAIN2 = $(patsubst %,$(IDIR)/%,$(_DEPS_MAIN2))

_OBJ_MAIN2 = joinEnumeration.o stats.o general_functions.o part2_main.o relations.o result.o RHJ_stage1.o RHJ_stage2.o RHJ_stage3.o RHJ.o SQL_QueryParser.o predicateParser.o job_scheduler.o startup.o
OBJ_MAIN2 = $(patsubst %,$(ODIR)/%,$(_OBJ_MAIN2))


$(ODIR)/%.o: %.c $(DEPS_MAIN2)
	$(CC) -c -o $@ $< $(CFLAGS)

part2_main: $(OBJ_MAIN2)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lpthread -lm


_DEPS_MAIN = general_functions.h relations.h result.h RHJ_stage1.h RHJ_stage2.h RHJ_stage3.h RHJ.h job_scheduler.h
DEPS_MAIN = $(patsubst %,$(IDIR)/%,$(_DEPS_MAIN))

_OBJ_MAIN = general_functions.o part1_main.o relations.o result.o RHJ_stage1.o RHJ_stage2.o RHJ_stage3.o RHJ.o job_scheduler.o
OBJ_MAIN = $(patsubst %,$(ODIR)/%,$(_OBJ_MAIN))


$(ODIR)/%.o: %.c $(DEPS_MAIN)
	$(CC) -c -o $@ $< $(CFLAGS)

part1_main: $(OBJ_MAIN)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lpthread


_OBJ_RANDOM = random_number_generator.o
OBJ_RANDOM = $(patsubst %,$(ODIR)/%,$(_OBJ_RANDOM))

$(ODIR)/%.o: random_number_generator/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

random: $(OBJ_RANDOM)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


_DEPS_UNIT = *.h 
DEPS_UNIT = $(patsubst %,$(IDIR)/%,$(_DEPS_UNIT))

_OBJ_UNIT = unit_tests.o unit_general_functions.o unit_predicateParser.o unit_relations.o unit_result.o unit_rhj.o unit_SQL_QueryParser.o unit_joinEnumeration.o predicateParser.o general_functions.o relations.o result.o RHJ_stage1.o RHJ_stage2.o RHJ_stage3.o RHJ.o SQL_QueryParser.o job_scheduler.o startup.o joinEnumeration.o stats.o
OBJ_UNIT = $(patsubst %,$(ODIR)/%,$(_OBJ_UNIT))


$(ODIR)/%.o: unit_testing/%.c $(DEPS_UNIT)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS_UNIT)
	$(CC) -c -o $@ $< $(CFLAGS)


unit: $(OBJ_UNIT)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lcunit -lpthread -lm
	./unit


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 
	rm -f ./part?_main
	rm -f ./random
	rm -f ./unit

cleantxt:
	rm -f ?_file.txt

