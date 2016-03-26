all:
	mkdir build;cd build;cmake ../unittest;make
clean:
	cd build; make clean; cd ../;rm -rf build
