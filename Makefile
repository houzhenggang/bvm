name = bvm
objs = config.o vnet.o vm.o create.o cdisk.o cnet.o zfs.o
objects = main.o $(objs)
booter_name = bvmb
booter_objs = booter.o $(objs)
opt = 

all: $(name) $(booter_name)

$(name): $(objects)
	cc $(opt) -o /usr/local/bin/$(name) $(objects)

$(booter_name): $(booter_objs)
	cc $(opt) -o /usr/local/bin/$(booter_name)  $(booter_objs)

main.o: main.c main.h
	cc $(opt) -c main.c -o main.o

config.o: config.c config.h
	cc $(opt) -c config.c -o config.o

vnet.o: vnet.c vnet.h
	cc $(opt) -c vnet.c -o vnet.o

vm.o: vm.c vm.h
	cc $(opt) -c vm.c -o vm.o

create.o: create.c create.h
	cc $(opt) -c create.c -o create.o

cdisk.o: cdisk.c cdisk.h
	cc $(opt) -c cdisk.c -o cdisk.o

cnet.o: cnet.c cnet.h
	cc $(opt) -c cnet.c -o cnet.o

booter.o: booter.c booter.h
	cc $(opt) -c booter.c -o booter.o

zfs.o: zfs.c zfs.h
	cc $(opt) -c zfs.c -o zfs.o

clean:
	rm -f $(objects) /usr/local/bin/$(name)
	rm -f booter.o /usr/local/bin/$(booter_name)
