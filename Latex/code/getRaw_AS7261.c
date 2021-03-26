//Get RAW AS7261 readings
int getX(int fd) { return(getChannel(AS7261_X, fd));}
int getY(int fd) { return(getChannel(AS7261_Y, fd));}
int getZ(int fd) { return(getChannel(AS7261_Z, fd));}
int getNIR(int fd) { return(getChannel(AS7261_NIR, fd));}
int getDark(int fd) { return(getChannel(AS7261_DARK, fd));}
int getClear(int fd) { return(getChannel(AS7261_CLEAR, fd));}