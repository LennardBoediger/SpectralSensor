import RPi.GPIO as GPIO           # import RPi.GPIO module  
GPIO.setmode(GPIO.BOARD)            # choose BCM or BOARD  
GPIO.setup(1, GPIO.OUT) # set a port/pin as an output   
GPIO.output(1, 1)       # set port/pin value to 1/GPIO.HIGH/True  