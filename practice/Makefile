CFLAGS = -Wno-implicit-function-declaration

all: learn arger

learn: learn.o mpg2km.o 
	@echo "Linking and producing the learn application."
	@gcc $(CFLAGS) learn.o mpg2km.o -o learn 
	@chmod +x learn

arger: arger.o
	@echo "Linking and producing the arger application."
	@gcc $(CFLAGS)  arger.o -o arger 
	@chmod +x arger

arger.o: arger.c 
	@echo "Compile the arger.c"
	@gcc $(CFLAGS) -c arger.c 

learn.o: learn.c 
	@echo "Compile the learn.c"
	@gcc $(CFLAGS) -c learn.c 

mpg2km.o: mpg2km.c  mpg2km.h
	@echo "Compile the mpg2km.c"
	@gcc $(CFLAGS) -c mpg2km.c 

clean: 
	@echo "Removing"
	rm -f learn.o mpg2km.o arger.o final

