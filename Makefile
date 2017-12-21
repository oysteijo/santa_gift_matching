CC = gcc
SED = sed
OPTFLAGS = -Wall -Wextra -O3 -g
INCLUDE = -I.
CFLAGS = $(OPTFLAGS) $(INCLUDE)

DATAFILES = child_wishlist.csv gift_goodkids.csv
DATA_HEADERS = $(patsubst %.csv,%.h,$(DATAFILES))

SRC = scorer.c tinymt32.c avg_happiness.c
OBJS = $(patsubst %.c,%.o,$(SRC))

%.h : %.csv; $(SED) -e "s/^[0-9]*,/\{/g" -e "s/$$/\},/g" $< > $@

all: scorer
$(OBJS): $(DATA_HEADERS) $(SRC)
scorer: $(OBJS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DATA_HEADERS) *~
	
