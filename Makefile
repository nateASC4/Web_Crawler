all: build																															
build: 																															
	@echo "Building" 																										
	gcc -o output web_crawler.c 																		
	./output												
clean:                                                                 
	@echo "cleaning" 
	rm output