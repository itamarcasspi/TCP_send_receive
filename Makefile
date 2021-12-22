all: measure sender

measure: measure.c
	gcc measure.c -o measure

sender: sender.c
	gcc sender.c -o sender

.PHONY: clean all

clean: 
	rm -rf *.o measure sender