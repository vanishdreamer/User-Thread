# Target library
## TODO: Phase 1.1
lib 	:= libuthread.a
objs    := uthread.o context.o preempt.o queue.o

CC      := gcc
CFLAGS  := -Wall -Werror
CFLAGS  += -g
AR 		:= ar rcs

ifneq ($(V),1) 
Q = @
endif

all: $(lib)
# Dep tracking *must* be below the 'all' rule
deps := $(patsubst %.o,%.d,$(objs)) 
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

$(lib): $(objs) 
	@echo "CC $@"
	$(Q)$(AR) $@ $^
%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)
clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)
