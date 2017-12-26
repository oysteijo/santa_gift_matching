CC = gcc
SED = sed
OPTFLAGS = -Wall -Wextra -O3 -g
INCLUDE = -I.
CFLAGS = $(OPTFLAGS) $(INCLUDE)
DATADIR = ./data

ZIPPED_DATAFILES = $(wildcard $(DATADIR)/*.zip)
#child_wishlist.zip gift_goodkids.zip
DATAFILES = $(patsubst %.zip,%.csv,$(ZIPPED_DATAFILES))
DATA_HEADERS = $(patsubst %.csv,%.h,$(DATAFILES))

SCORER_SRC = scorer.c avg_happiness.c
SCORER_OBJS = $(patsubst %.c,%.o,$(SCORER_SRC))

%.h : %.csv; $(SED) -e "s/^[0-9]*,/\{/g" -e "s/$$/\},/g" $< > $@
%.csv: %.zip; unzip -o $< -d $(DATADIR) 

#mytest:
#	echo $(DATA_HEADERS)

all: scorer
$(SCORER_OBJS): $(DATA_HEADERS) $(SCORER_SRC)
scorer: $(SCORER_OBJS)

.PHONY: clean
clean:
	$(RM) $(SCORER_OBJS) $(DATA_HEADERS) *~
	
