//Get RAW AS72651(NIR) readings
int getR(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_R_G_A, fd));}
int getS(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_S_H_B, fd));}
int getT(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_T_I_C, fd));}
int getU(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_U_J_D, fd));}
int getV(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_V_K_E, fd));}
int getW(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_W_L_F, fd));}

//Get RAW AS72652(color) readings
int getG(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_R_G_A, fd));}
int getX(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_S_H_B, fd));}
int getI(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_T_I_C, fd));}
int getJ(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_U_J_D, fd));}
int getK(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_V_K_E, fd));}
int getL(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_W_L_F, fd));}

//Get the various UV readings
//Get RAW AS72653(UV) readings
int getA(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_R_G_A, fd));}
int getB(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_S_H_B, fd));}
int getC(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_T_I_C, fd));}
int getD(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_U_J_D, fd));}
int getE(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_V_K_E, fd));}
int getF(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_W_L_F, fd));}
