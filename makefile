SRC= 	lex.l		\
	parse.y		\
	main.c		\
	mktime.c	\
	gnuplot_i.c	\
	ts_bcktst_mgr.c	\
	ts_csv_reader.c	\
	ts_file.c	\
	ts_func.c	\
	ts_func_mgr.c	\
	ts_hash_table.c	\
	ts_list.c	\
	ts_misc.c	\
	ts_prgm.c	\
	ts_prgm_mgr.c	\
	ts_stack.c	\
	ts_symbol_id_factory.c	\
	ts_table.c	\
	ts_tree.c	\
	ts_type.c	\
	ts_var_mgr.c	\
	ts_vm.c		\
	ts_xzlib.c	\


CC=	gcc
LEX=	flex
YACC=	bison
PRGM=	p
DEBUG=-g -DDEBUG
OPT=-O3
LIBS=-lm -lz -llzma -lpthread -ltcmalloc_minimal
LEXFILE	=$(filter %.l,$(SRC))
LEXCFILE=$(LEXFILE:%.l=%.c)
LEXHFILE=$(LEXFILE:%.l=%.h)
YACCFILE=$(filter %.y,$(SRC))
YACCCFILE=$(YACCFILE:%.y=%.c)
YACCHFILE=$(YACCFILE:%.y=%.h)
CFILE=$(filter %.c,$(SRC))
CFILE+=$(LEXCFILE)
CFILE+=$(YACCCFILE)
OBJC=	$(SRC:%.c=%.o)
OBJL=	$(OBJC:%.l=%.o)
OBJ=	$(OBJL:%.y=%.o)
OPTOBJ=	$(OBJ:%.o=%opt.o)

build:	$(PRGM)

buildopt:	$(PRGM)-opt

$(PRGM): $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o $(PRGM)

$(PRGM)-opt: $(OPTOBJ)
	$(CC) $(OPTOBJ) $(LIBS) -o $(PRGM)-opt

$(LEXCFILE) $(LEXHFILE):	$(LEXFILE) $(YACCHFILE)
	$(LEX) --header-file=$(LEXHFILE) -o $(LEXCFILE) $(LEXFILE)

$(YACCCFILE) $(YACCHFILE):	$(YACCFILE)
	$(YACC) -y --defines=$(YACCHFILE) -o $(YACCCFILE) $(YACCFILE)

$(OBJ):	%.o:	%.c
	$(CC) $(DEBUG) -Wall -c $< -o $@

$(OPTOBJ):	%opt.o:	%.c
	$(CC) -g $(OPT) -Wall -c $< -o $@

.PHONY:	opt
opt: buildopt

.PHONY:	clean
clean:
	rm -rf *.o $(PRGM) $(PRGM)-opt $(LEXCFILE) $(LEXHFILE) $(YACCCFILE) $(YACCHFILE)
.phony:	cleanobj
cleanobj:
	rm -rf *.o
