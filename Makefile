OBJS	= jobExecutor.o worker.o send_msg.o set_up_worker.o user_query.o list.o trie.o stack.o map_file.o maxcount.o wc.o search.o
SOURCE	= jobExecutor.c worker.c send_msg.c set_up_worker.c user_query.c list.c trie.c stack.c map_file.c maxcount.c wc.c search.c
HEADER	= worker.h send_msg.h set_up_worker.h user_query.h list.h trie.h stack.h map_file.h maxcount.h wc.h search.h
OUT	= jobExecutor
CC	= gcc
FLAGS	= -c

all:	$(OBJS)
	$(CC) -o $(OUT) $(OBJS)

jobExecutor.o: jobExecutor.c
	$(CC) $(FLAGS) jobExecutor.c

worker.o: worker.c
	$(CC) $(FLAGS) worker.c

send_msg.o: send_msg.c
	$(CC) $(FLAGS) send_msg.c

set_up_worker.o: set_up_worker.c
	$(CC) $(FLAGS) set_up_worker.c

user_query.o: user_query.c
	$(CC) $(FLAGS) user_query.c

list.o: list.c
	$(CC) $(FLAGS) list.c

trie.o: trie.c
	$(CC) $(FLAGS) trie.c

stack.o: stack.c
	$(CC) $(FLAGS) stack.c

map_file.o: map_file.c
	$(CC) $(FLAGS) map_file.c

maxcount.o: maxcount.c
	$(CC) $(FLAGS) maxcount.c

wc.o: wc.c
	$(CC) $(FLAGS) wc.c

search.o: search.c
	$(CC) $(FLAGS) search.c

clean:
	rm -f $(OBJS) $(OUT)