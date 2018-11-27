ROOT = .
all:
	cd $(ROOT)/dev_se && make
	cd $(ROOT)/dev_cl && make
clean:
	rm -rf ./Out/*
