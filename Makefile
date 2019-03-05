#
#  Makefile
#  ARToolKit5
#
#  Disclaimer: IMPORTANT:  This Daqri software is supplied to you by Daqri
#  LLC ("Daqri") in consideration of your agreement to the following
#  terms, and your use, installation, modification or redistribution of
#  this Daqri software constitutes acceptance of these terms.  If you do
#  not agree with these terms, please do not use, install, modify or
#  redistribute this Daqri software.
#
#  In consideration of your agreement to abide by the following terms, and
#  subject to these terms, Daqri grants you a personal, non-exclusive
#  license, under Daqri's copyrights in this original Daqri software (the
#  "Daqri Software"), to use, reproduce, modify and redistribute the Daqri
#  Software, with or without modifications, in source and/or binary forms;
#  provided that if you redistribute the Daqri Software in its entirety and
#  without modifications, you must retain this notice and the following
#  text and disclaimers in all such redistributions of the Daqri Software.
#  Neither the name, trademarks, service marks or logos of Daqri LLC may
#  be used to endorse or promote products derived from the Daqri Software
#  without specific prior written permission from Daqri.  Except as
#  expressly stated in this notice, no other rights or licenses, express or
#  implied, are granted by Daqri herein, including but not limited to any
#  patent rights that may be infringed by your derivative works or by other
#  works in which the Daqri Software may be incorporated.
#
#  The Daqri Software is provided by Daqri on an "AS IS" basis.  DAQRI
#  MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
#  THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE, REGARDING THE DAQRI SOFTWARE OR ITS USE AND
#  OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
#
#  IN NO EVENT SHALL DAQRI BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
#  MODIFICATION AND/OR DISTRIBUTION OF THE DAQRI SOFTWARE, HOWEVER CAUSED
#  AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
#  STRICT LIABILITY OR OTHERWISE, EVEN IF DAQRI HAS BEEN ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
#  Copyright 2015 Daqri, LLC.
#  Copyright 2002-2015 ARToolworks, Inc.
#
#  Author(s): Hirokazu Kato, Philip Lamb
#

INC_DIR= /opt/artoolkit5/include
LIB_DIR= /opt/artoolkit5/lib
BIN_DIR= .

CC=gcc
LDFLAG=-L/usr/lib/x86_64-linux-gnu -L/usr/lib64 -L$(LIB_DIR)/linux-x86_64 -L$(LIB_DIR)
LIBS= -lARgsub -lARvideo -lAR -lARICP -lAR -lARUtil -lglut -lGLU -lGL -lX11 -lm -lpthread -ljpeg -lz -ludev
CFLAG= -O3 -fPIC -march=core2 -DHAVE_NFT=1 -I$(INC_DIR)

OBJS =
HEADDERS =

all: $(BIN_DIR)/simpleTest

$(BIN_DIR)/simpleTest: simpleTest.o $(OBJS)
	${CC} -o $(BIN_DIR)/simpleTest simpleTest.o $(OBJS) $(LDFLAG) $(LIBS)

simpleTest.o: simpleTest.c $(HEADDERS)
	${CC} -c $(CFLAG) simpleTest.c

clean:
	rm -f *.o
	rm -f $(BIN_DIR)/simpleTest

allclean:
	rm -f *.o
	rm -f $(BIN_DIR)/simpleTest
	rm -f Makefile

distclean:
	rm -f *.o
	rm -f Makefile
