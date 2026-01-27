Setting up GLFW on MacOS with command-line 'make'
-------------------------------------------------

1. Open a terminal window.

2. Install Homebrew by executing this:

       /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

   This will take a while.  If errors occur during the installation,
   correct them and try again.  Only continue if you have a successful
   installation.

3. Execute: brew install gcc

4. Execute: xcode-select -install

5. Execute: brew install glfw

Problems:

  If you have problems on compilation where the header files
  (e.g. GLFW/glfw3.h) are not being found, you must determine where
  those are stored on your machine and modify the Makefile to point to
  them.  If, for example, the missing header files are stored in

     /opt/homebrew/Cellar/glfw/3.4/include

  then you should change the second line of the Makefile to

     CXXFLAGS = -g -Wall -Wno-write-strings -Wno-parentheses -DMACOS -I/opt/homebrew/Cellar/glfw/3.4/include

  If you have problems where the libraries are not found, you have to
  determine where they are stored and modify the Makefile accordingly.
  If, for example, the missing libraries are stored in

     /opt/homebrew/Cellar/glfw/3.4/lib

  then you should change the second line of the Makefile to

     LDFLAGS = -L. -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw -ldl
