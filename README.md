# Hunt-And-Kill-Maze-Generator
A C++ program that generates random mazes using the Hunt and Kill maze generation algorithm. This algorithm likes to produce long winding tunnels and forks. There are still plenty of stubs and dead ends as well. With such long tunnels it seems to be harder to find where the given tunnel ends. This algorithm behaves like a recursive backtracker due to the Hunt portion extending from already visited cells, however it is not a backtracker because it does not backtrack.

This program uses OpenGL and GLUT to draw the maze on the screen for the user to view. The generation of the maze is visualized but there are options to skip the visualization and only show the final product.

# Play Mode
There is a "Play Mode" to allow the user to attempt to navigate through the maze. The player is distinguish as the cell that is filled in. The start and end of the maze are marked by different colored walls. Start is marked by purple walls and the end is marked by pink walls. As you move the player along the maze, the program will keep track of how many total moves you have gone and prints it to the terminal.

# Keybindings
  1. w  ------ Try to move the player up
  2. a  ------ Try to move the player left
  3. s  ------ Try to move the player down
  4. d	------ Try to move the player right
  5. r  ------ Enter play mode
  6. R	------ Draw everything in Red
  7. g	------ Regenerate the maze (Interactively)
  8. i	------ Instantly regenerate the maze (non-interactive)
  9. G	------ Draw everything in Green
  10. b	------ Draw everything in Black
  11. B	------ Draw everything in blue
  12. q or Q - Quit the program	
  13. V	------ Toggle maze generation verbosity
  14. + ------ Increase the delay between maze generation steps
  15. - ------ Decrease the delay between maze generations steps

# How To Use
!IMPORTANT!
Be sure to have installed openGL/GLEW/glut and include –LGL –lglut –lGLEW so that the program will link to the correct libraries.

The following lines will compile the program correctly on a Linux machine
  g++ -c HKMaze.cpp; 
  g++ main.cpp HKMaze.o –LGL –lglut –lGLEW – o maze
You can then use the following to run the program from a terminal in the same directory
  ./maze

The given make file will also compile program correctly.
