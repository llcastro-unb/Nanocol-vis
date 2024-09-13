##### C-OPENGL DEFINITIONS ####

CFLAGS = -g -O2 -funroll-loops -ansi -pedantic -ffast-math -D_SVID_SOURCE -D_BSD_SOURCE -DSHM -I/usr/X11R6/include
CFLAGS2 = -Wdeprecated-declarations -Wdeclaration-after-statement
CDEBUG = -ggdb

XLIBS = -L/usr/X11/lib -L/usr/X11R6/lib -lX11 -lXext -lXmu -lXt -lXi -lSM -lICE

INCDIR = /usr/include
LIBDIR = /usr/lib

GL_LIBS = -L$(LIBDIR) -lglut -lGLU -lGL -lm $(XLIBS)

LIB_DEP = $(LIBDIR)/$(GL_LIB)

##### COMMANDS ####


nanoVis-3d: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc -I$(INCDIR) $(CFLAGS) nanoVis-3d.c $(GL_LIBS) -o $@ 
	@./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------

nanoVis-3d-debug: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc $(CDEBUG) -I$(INCDIR) $(CFLAGS) nanoVis-3d.c $(GL_LIBS) -o $@ 
	@gdb ./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------

nanoVis-3d--agglomerates: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc -I$(INCDIR) $(CFLAGS) nanoVis-3d--agglomerates.c $(GL_LIBS) -o $@ 
	@./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------
	
nanoVis-3d--agglomerate_of_particle_ip: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc -I$(INCDIR) $(CFLAGS) nanoVis-3d--agglomerate_of_particle_ip.c $(GL_LIBS) -o $@ 
	@./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------

nanoVis-2d: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc -I$(INCDIR) $(CFLAGS) nanoVis-2d.c $(GL_LIBS) -o $@ 
	@./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------

nanoVis-2d--agglomerates: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc -I$(INCDIR) $(CFLAGS) nanoVis-2d--agglomerates.c $(GL_LIBS) -o $@ 
	@./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------

nanoVis-2d--agglomerate_of_particle_ip: $(LIB_DEP)
	@echo ..................................................................................... 
	@echo  "${green}Visualization starts${NC}"
	@gcc -I$(INCDIR) $(CFLAGS) nanoVis-2d--agglomerate_of_particle_ip.c $(GL_LIBS) -o $@ 
	@./$@
	@echo  "${green}Visualization finishes${NC}"
	@echo --------------------------------------------------------------------------------------


clean:
	rm -f *.o *.mod *.MOD

reset:
	rm -f OUT-* out-*

