densidad.png : graficas.py UpwindGodunov_step_0.dat
	python graficas.py

shocktube.x : shocktube.c riemann.c steps.c
	gcc shocktube.c riemann.c steps.c -lm -o shocktube.x

UpwindGodunov_step_0.dat:  shocktube.x
	./shocktube.x
