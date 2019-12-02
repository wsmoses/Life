ifeq ($(UNAME_S),Darwin)
CCFLAGS += -lglut  -fopenmp
else
CCFLAGS += -framework GLUT -framework OpenGL -framework Cocoa -fopenmp
endif

life: life.c
	mpicc -lm $(CCFLAGS) -o life life.c
	#mpicc -lm -lGLU -lglut -o life life.c
