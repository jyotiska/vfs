../bin/vfs_group26: ../bin/create_vfs.o ../bin/vfs_main.o ../bin/vfsdriver.o ../bin/mount.o ../bin/nary.o ../bin/hash.o ../bin/unmount.o ../bin/directoryoper.o ../bin/fileoperation.o
	gcc ../bin/vfsdriver.o ../bin/create_vfs.o ../bin/mount.o ../bin/vfs_main.o  ../bin/nary.o ../bin/hash.o ../bin/unmount.o ../bin/directoryoper.o ../bin/fileoperation.o -o ../bin/vfs_group26

../bin/vfsdriver.o: ../src/vfsdriver.c ../include/create_vfs.h ../include/hash.h ../include/mount.h ../include/vfs_errorcodes.h
	gcc -c ../src/vfsdriver.c -o ../bin/vfsdriver.o
	
../bin/vfs_main.o: ../src/vfs_main.c ../include/vfs_main.h
	gcc -c ../src/vfs_main.c -o ../bin/vfs_main.o

../bin/create_vfs.o:../src/create_vfs.c ../include/vfs_main.h ../include/create_vfs.h
	gcc -c ../src/create_vfs.c -o ../bin/create_vfs.o

../bin/mount.o:../src/mount.c ../include/vfs_main.h ../include/nary.h ../include/hash.h ../include/mount.h 
	gcc -c  ../src/mount.c -o ../bin/mount.o

../bin/unmount.o:../src/unmount.c ../include/vfs_main.h ../include/unmount.h 
	gcc -c  ../src/unmount.c -o ../bin/unmount.o

../bin/nary.o:../src/nary.c ../include/nary.h ../include/vfs_main.h
	gcc -c  ../src/nary.c -o ../bin/nary.o

../bin/hash.o:../src/hash.c ../include/hash.h ../include/vfs_main.h
	gcc -c  ../src/hash.c -o ../bin/hash.o

../bin/directoryoper.o:../src/directoryoper.c ../include/vfs_main.h ../include/nary.h ../include/hash.h
	gcc -c  ../src/directoryoper.c -o ../bin/directoryoper.o 

../bin/fileoperation.o:../src/fileoperation.c ../include/vfs_main.h ../include/nary.h ../include/hash.h
	gcc -c  ../src/fileoperation.c -o ../bin/fileoperation.o 
