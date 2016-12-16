CC := clang
LD := clang

CC_FLAGS := -std=c99 -g -O2
LD_FLAGS := -O2

OBJDIR := obj/
BINDIR := bin/

C_SRC := main.c tokenize.c match.c
MATCHER_SRC := matcher_main.c match.c

C_OBJS := $(C_SRC:%=$(OBJDIR)%.o)
MATCHER_OBJS := $(MATCHER_SRC:%=$(OBJDIR)%.o)

TOKENIZER_BIN := $(BINDIR)tokenizer
MATCHER_BIN := $(BINDIR)matcher

all : $(TOKENIZER_BIN) $(MATCHER_BIN)
	
clean: 
	rm $(TOKENIZER_BIN) $(MATCHER_BIN) $(C_OBJS) $(MATCHER_OBJS)
	rmdir $(OBJDIR)
	rmdir $(BINDIR)

$(MATCHER_BIN): $(MATCHER_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(LD_FLAGS) $(MATCHER_OBJS)

$(TOKENIZER_BIN): $(C_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(LD_FLAGS) $(C_OBJS)

$(OBJDIR)%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@ 
