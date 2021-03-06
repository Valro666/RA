/*
 *  simpleTest.c
 *
 *  gsub-based example code to demonstrate use of ARToolKit.
 *
 *  Press '?' while running for help on available key commands.
 *
 *  Disclaimer: IMPORTANT:  This Daqri software is supplied to you by Daqri
 *  LLC ("Daqri") in consideration of your agreement to the following
 *  terms, and your use, installation, modification or redistribution of
 *  this Daqri software constitutes acceptance of these terms.  If you do
 *  not agree with these terms, please do not use, install, modify or
 *  redistribute this Daqri software.
 *
 *  In consideration of your agreement to abide by the following terms, and
 *  subject to these terms, Daqri grants you a personal, non-exclusive
 *  license, under Daqri's copyrights in this original Daqri software (the
 *  "Daqri Software"), to use, reproduce, modify and redistribute the Daqri
 *  Software, with or without modifications, in source and/or binary forms;
 *  provided that if you redistribute the Daqri Software in its entirety and
 *  without modifications, you must retain this notice and the following
 *  text and disclaimers in all such redistributions of the Daqri Software.
 *  Neither the name, trademarks, service marks or logos of Daqri LLC may
 *  be used to endorse or promote products derived from the Daqri Software
 *  without specific prior written permission from Daqri.  Except as
 *  expressly stated in this notice, no other rights or licenses, express or
 *  implied, are granted by Daqri herein, including but not limited to any
 *  patent rights that may be infringed by your derivative works or by other
 *  works in which the Daqri Software may be incorporated.
 *
 *  The Daqri Software is provided by Daqri on an "AS IS" basis.  DAQRI
 *  MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 *  THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE, REGARDING THE DAQRI SOFTWARE OR ITS USE AND
 *  OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 *
 *  IN NO EVENT SHALL DAQRI BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 *  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 *  MODIFICATION AND/OR DISTRIBUTION OF THE DAQRI SOFTWARE, HOWEVER CAUSED
 *  AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 *  STRICT LIABILITY OR OTHERWISE, EVEN IF DAQRI HAS BEEN ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  Copyright 2015 Daqri LLC. All Rights Reserved.
 *  Copyright 2002-2015 ARToolworks, Inc. All Rights Reserved.
 *
 *  Author(s): Hirokazu Kato, Philip Lamb.
 *
 */

#ifdef _WIN32
#  include <windows.h>
#  define _USE_MATH_DEFINES
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef __APPLE__
#  include <GL/gl.h>
#  include <GL/glut.h>
#else
#  include <OpenGL/gl.h>
#  include <GLUT/glut.h>
#endif
#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <ARUtil/time.h>


#define             VPARA_NAME       "Data/cameraSetting-%08x%08x.dat"
#define             PATT_NAME        "Data/tyr"
#define             PATT_NAME2       "Data/fleet"
#define             PATT_NAME3       "Data/hiro"

ARHandle           *arHandle;
ARPattHandle       *arPattHandle;
AR3DHandle         *ar3DHandle;
ARGViewportHandle  *vp;
int                 xsize, ysize;
int                 flipMode = 0;
//int                 patt_id;
double              patt_width = 80.0;
int                 count = 0;
char                fps[256];
char                errValue[256];
int                 distF = 0;
int                 contF = 0;
ARParamLT          *gCparamLT = NULL;

static void   init(int argc, char *argv[]);
static void   keyFunc( unsigned char key, int x, int y );
static void   cleanup(void);
static void   mainLoop(void);
static void   draw( void );


struct mark{
    int patt_id;
    ARdouble patt_trans[3][4];
    //ARMarkerInfo   *markerInfo;
    int             markerNum;
    int found ;
    
};

struct mark spot[3];

int main(int argc, char *argv[])
{
    
        //mark[0]
    
	glutInit(&argc, argv);
    init(argc, argv);

    argSetDispFunc( mainLoop, 1 );
	argSetKeyFunc( keyFunc );
	count = 0;
    fps[0] = '\0';
	arUtilTimerReset();
    argMainLoop();
	return (0);
}
static void keyFunc( unsigned char key, int x, int y )
{
    int   value;

    switch (key) {
		case 0x1b:
			cleanup();
			exit(0);
			break;
		case '1':
		case '-':
        	arGetLabelingThresh( arHandle, &value );
        	value -= 5;
        	if( value < 0 ) value = 0;
        	arSetLabelingThresh( arHandle, value );
        	ARLOG("thresh = %d\n", value);
        	break;
		case '2':
		case '+':
        	arGetLabelingThresh( arHandle, &value );
       		value += 5;
        	if( value > 255 ) value = 255;
        	arSetLabelingThresh( arHandle, value );
        	ARLOG("thresh = %d\n", value);
        	break;
		case 'd':
		case 'D':
        	arGetDebugMode( arHandle, &value );
       		value = 1 - value;
        	arSetDebugMode( arHandle, value );
            break;
        case 'h':
        case 'H':
            if( flipMode & AR_GL_FLIP_H ) flipMode = flipMode & AR_GL_FLIP_V;
            else                         flipMode = flipMode | AR_GL_FLIP_H;
            argViewportSetFlipMode( vp, flipMode );
            break;
        case 'v':
        case 'V':
            if( flipMode & AR_GL_FLIP_V ) flipMode = flipMode & AR_GL_FLIP_H;
            else                         flipMode = flipMode | AR_GL_FLIP_V;
            argViewportSetFlipMode( vp, flipMode );
        	break;
        case ' ':
            distF = 1 - distF;
            if( distF ) {
                argViewportSetDistortionMode( vp, AR_GL_DISTORTION_COMPENSATE_ENABLE );
            } else {
                argViewportSetDistortionMode( vp, AR_GL_DISTORTION_COMPENSATE_DISABLE );
            }
            break;
        case 'c':
            contF = 1 - contF;
            break;
		case '?':
		case '/':
			ARLOG("Keys:\n");
			ARLOG(" [esc]         Quit demo.\n");
			ARLOG(" - and +       Adjust threshhold.\n");
			ARLOG(" d             Activate / deactivate debug mode.\n");
			ARLOG(" h and v       Toggle horizontal / vertical flip mode.\n");
            ARLOG(" [space]       Toggle distortion compensation.\n");
			ARLOG(" ? or /        Show this help.\n");
			ARLOG("\nAdditionally, the ARVideo library supplied the following help text:\n");
			arVideoDispOption();
			break;
		default:
			break;
	}
}

static void mainLoop(void)
{

    
    static int      contF2 = 0;
    //static ARdouble patt_trans[3][4];
    static AR2VideoBufferT *buff = NULL;
    ARMarkerInfo   *markerInfo;
    int             markerNum;
    ARdouble        err;
    int             imageProcMode;
    int             debugMode;
    int             j, k;

    /* grab a video frame */
    buff = arVideoGetImage();
    if (!buff || !buff->fillFlag) {
        arUtilSleep(2);
        return;
    }

    arGetDebugMode(arHandle, &debugMode);
    if (debugMode == AR_DEBUG_ENABLE) {
        argViewportSetPixFormat(vp, AR_PIXEL_FORMAT_MONO); // Drawing the debug image.
        argDrawMode2D(vp);
        arGetImageProcMode(arHandle, &imageProcMode);
        if (imageProcMode == AR_IMAGE_PROC_FRAME_IMAGE) argDrawImage(arHandle->labelInfo.bwImage);
        else argDrawImageHalf(arHandle->labelInfo.bwImage);
    } else {
        AR_PIXEL_FORMAT pixFormat;
        arGetPixelFormat(arHandle, &pixFormat);
        argViewportSetPixFormat(vp, pixFormat); // Drawing the input image.
        argDrawMode2D(vp);
        argDrawImage(buff->buff);
    }

    /* detect the markers in the video frame */
    
    //for(int i = 0 ; i < 3; i++)
    //int i = 0 ;
    {
    
    if (arDetectMarker(arHandle, buff) < 0) {
        cleanup();
        exit(0);
    }

    if( count % 60 == 0 ) {
        sprintf(fps, "%f[fps]", 60.0/arUtilTimer());
        arUtilTimerReset();
    }
    count++;
    glColor3f(0.0f, 1.0f, 0.0f);
    argDrawStringsByIdealPos(fps, 10, ysize-30);

    markerNum = arGetMarkerNum( arHandle );
    //printf("--------------------------------- %d\n",markerNum);
    if( markerNum == 0 ) {
        argSwapBuffers();
        return;
    }

    /* check for object visibility */

        printf("%d %d %d\n",spot[0].patt_id,spot[1].patt_id,spot[2].patt_id);

        
        markerInfo =  arGetMarker( arHandle ); 
       k = -1;
		  int i = 0 ;
	for(i = 0 ; i < 3; i++){
	  k = -1;
	  	      spot[i].found = -1;
        for( j = 0; j < markerNum; j++ ) {
// 
        
            ARLOG("ID=%d, CF = %f\n", markerInfo[j].id, markerInfo[j].cf);
            
                if( spot[i].patt_id == markerInfo[j].id ) {
                    if( k == -1 ) {
                        if (markerInfo[j].cf >= 0.1) {
			  k = j;
			  }
                    } else if( markerInfo[j].cf > markerInfo[k].cf ) k = j;
                }
            }
        //printf("----------------------------------------------------------------------------------[ %d\n",k);
        

	
     if( k > -1 )
        
           {
                 spot[i].found = 1;
                err = arGetTransMatSquare(ar3DHandle, &(markerInfo[k]), patt_width, spot[i].patt_trans);
            }
        }
        sprintf(errValue, "err = %f", err);
        glColor3f(0.0f, 1.0f, 0.0f);
        argDrawStringsByIdealPos(fps, 10, ysize-30);
        argDrawStringsByIdealPos(errValue, 10, ysize-60);
        //ARLOG("err = %f\n", err);

        contF2 = 1;
        //for(int i = 0 ; i < 3; i++)
            draw();

        argSwapBuffers();
    }
    
}

static void   init(int argc, char *argv[])
{
    char           *cparam_name = NULL;
    ARParam         cparam;
    ARGViewport     viewport;
    char            vconf[512];
    AR_PIXEL_FORMAT pixFormat;
    ARUint32        id0, id1;
    int             i;

    if( argc == 1 ) vconf[0] = '\0';
    else {
        strcpy( vconf, argv[1] );
        for( i = 2; i < argc; i++ ) {strcat(vconf, " "); strcat(vconf,argv[i]);}
    }

    arUtilChangeToResourcesDirectory(AR_UTIL_RESOURCES_DIRECTORY_BEHAVIOR_BEST, NULL);
    
    /* open the video path */
	ARLOGi("Using video configuration '%s'.\n", vconf);
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    if( arVideoGetSize(&xsize, &ysize) < 0 ) exit(0);
    ARLOGi("Image size (x,y) = (%d,%d)\n", xsize, ysize);
    if( (pixFormat=arVideoGetPixelFormat()) < 0 ) exit(0);
    if( arVideoGetId( &id0, &id1 ) == 0 ) {
        ARLOGi("Camera ID = (%08x, %08x)\n", id1, id0);
        sprintf(vconf, VPARA_NAME, id1, id0);
        if( arVideoLoadParam(vconf) < 0 ) {
            ARLOGe("No camera setting data!!\n");
        }
    }

    /* set the initial camera parameters */
    if (cparam_name && *cparam_name) {
        if (arParamLoad(cparam_name, 1, &cparam) < 0) {
            ARLOGe("Camera parameter load error !!\n");
            exit(0);
        }
    } else {
        arParamClearWithFOVy(&cparam, xsize, ysize, M_PI_4); // M_PI_4 radians = 45 degrees.
        ARLOGw("Using default camera parameters for %dx%d image size, 45 degrees vertical field-of-view.\n", xsize, ysize);
    }
    arParamChangeSize( &cparam, xsize, ysize, &cparam );
    ARLOG("*** Camera Parameter ***\n");
    arParamDisp( &cparam );
    if ((gCparamLT = arParamLTCreate(&cparam, AR_PARAM_LT_DEFAULT_OFFSET)) == NULL) {
        ARLOGe("Error: arParamLTCreate.\n");
        exit(-1);
    }
    
    if( (arHandle=arCreateHandle(gCparamLT)) == NULL ) {
        ARLOGe("Error: arCreateHandle.\n");
        exit(0);
    }
    if( arSetPixelFormat(arHandle, pixFormat) < 0 ) {
        ARLOGe("Error: arSetPixelFormat.\n");
        exit(0);
    }

    if( (ar3DHandle=ar3DCreateHandle(&cparam)) == NULL ) {
        ARLOGe("Error: ar3DCreateHandle.\n");
        exit(0);
    }

    if( (arPattHandle=arPattCreateHandle()) == NULL ) {
        ARLOGe("Error: arPattCreateHandle.\n");
        exit(0);
    }
    if( (spot[0].patt_id=arPattLoad(arPattHandle, PATT_NAME)) < 0 ) {
        ARLOGe("pattern load error !!\n");
        exit(0);
    }
    
    if( (spot[1].patt_id=arPattLoad(arPattHandle, PATT_NAME2)) < 0 ) {
        ARLOGe("pattern load error !!\n");
        exit(0);
    }
    
    if( (spot[2].patt_id=arPattLoad(arPattHandle, PATT_NAME3)) < 0 ) {
        ARLOGe("pattern load error !!\n");
        exit(0);
    }
    arPattAttach( arHandle, arPattHandle );

    /* open the graphics window */
/*
    int winSizeX, winSizeY;
    argCreateFullWindow();
    argGetScreenSize( &winSizeX, &winSizeY );
    viewport.sx = 0;
    viewport.sy = 0;
    viewport.xsize = winSizeX;
    viewport.ysize = winSizeY;
*/
    viewport.sx = 0;
    viewport.sy = 0;
    viewport.xsize = xsize;
    viewport.ysize = ysize;
    if( (vp=argCreateViewport(&viewport)) == NULL ) exit(0);
    argViewportSetCparam( vp, &cparam );
    argViewportSetPixFormat( vp, pixFormat );
    //argViewportSetDispMethod( vp, AR_GL_DISP_METHOD_GL_DRAW_PIXELS );
    argViewportSetDistortionMode( vp, AR_GL_DISTORTION_COMPENSATE_DISABLE );

	if (arVideoCapStart() != 0) {
        ARLOGe("video capture start error !!\n");
        exit(0);
	}
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    argCleanup();
	arPattDetach(arHandle);
	arPattDeleteHandle(arPattHandle);
	ar3DDeleteHandle(&ar3DHandle);
	arDeleteHandle(arHandle);
    arParamLTFree(&gCparamLT);
    arVideoClose();
}

static void draw( void )
{
    ARdouble  gl_para[16];
    GLfloat   mat_diffuse[]     = {0.0f, 0.0f, 1.0f, 0.0f};
    GLfloat   mat_flash[]       = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat   mat_flash_shiny[] = {50.0f};
    GLfloat   light_position[]  = {100.0f,-200.0f,200.0f,0.0f};
    GLfloat   light_ambi[]      = {0.1f, 0.1f, 0.1f, 0.0f};
    GLfloat   light_color[]     = {1.0f, 1.0f, 1.0f, 0.0f};
    
    argDrawMode3D(vp);
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* load the camera transformation matrix */
    int i = 0;
        glMatrixMode(GL_MODELVIEW);
	
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_diffuse);
    
    for ( i=0; i < 3 ; i++){
	    //trans = spot[i].patt_trans;
	      printf("found ::: %d %d %d\n", spot[0].found,spot[1].found,spot[2].found);
	      if(spot[i].found == 1){
	    argConvGlpara(spot[i].patt_trans, gl_para);
	    //glMatrixMode(GL_MODELVIEW);

	    
	#ifdef ARDOUBLE_IS_FLOAT
	    glLoadMatrixf( gl_para );
	#else
	    glLoadMatrixd( gl_para );
#endif
	    
	    glutSolidCube(20.0);
	      
	    }
	    
	    printf("--------------------------------------------------\n");
	    int a = 0 ;
	    int b = 0 ;
	    for(a = 0 ; a < 3 ; a ++){
	      for(b = 0 ; b < 4 ; b ++){
			    printf("%f ", spot[i].patt_trans[a][b]);
	      }
	      printf("\n");
	    }


	    

    }
    
    
    if(spot[0].found == 1){
	if(spot[1].found==1){
	  printf("ouiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n");
	  double xa = spot[0].patt_trans[0][3];
	  double ya = spot[0].patt_trans[1][3];
	  double za = spot[0].patt_trans[2][3];
	  
	  double tx = 100;
	  double ty = 100;
	  double tz = 0;
	  double xb = spot[1].patt_trans[0][3];
	  double yb = spot[1].patt_trans[1][3];
	  double zb = spot[1].patt_trans[2][3];  
	  printf("%lf %lf %lf \n",xa,ya,za);
	  
	  xa = spot[1].patt_trans[0][0]*tx + spot[1].patt_trans[0][1]*ty+spot[1].patt_trans[0][2]*tz+xa;
	  ya = spot[1].patt_trans[1][0]*tx + spot[1].patt_trans[1][1]*ty+spot[1].patt_trans[1][2]*tz+ya;
	  za = spot[1].patt_trans[2][0]*tx + spot[1].patt_trans[2][1]*ty+spot[1].patt_trans[2][2]*tz+za;
	  
	  //xa = xa + tx;
	  //ya = ya + ty;
	  //za = za + tz;
	  
	  
	  
	  
	  printf("%lf %lf %lf \n",xa,ya,za);
	  
	  double d = (xa-xb)*(xa-xb)+(ya-yb)*(ya-yb)+(za-zb)*(za-zb);
	  d = sqrt(d);
	  //d = d *100;
	  printf("dddddddddddddddddddddddddddddddddddddddddddddddddddddd --->  %f\n",d);
	  // d en mm
	  if(d>10){    

	    argConvGlpara(spot[0].patt_trans, gl_para);
	    glLoadMatrixd( gl_para );
	
	    glutSolidCube(40.0);

	        

	    argConvGlpara(spot[1].patt_trans, gl_para);    
	    glLoadMatrixd( gl_para );
	
	    	    glutSolidCube(40.0);
	    glTranslated(tx,ty,tz);
	    glutSolidCube(40.0);

	  }else{
	    argConvGlpara(spot[0].patt_trans, gl_para);
	    glLoadMatrixd( gl_para );
	
	    glutWireCube(40.0);
	    glTranslated(tx,ty,tz);
	    glutWireCube(40.0);
	    
	    argConvGlpara(spot[1].patt_trans, gl_para);
	    glLoadMatrixd( gl_para );
	    glutWireCube(40.0);

	  }
	  
	}
    }
        glDisable(GL_LIGHT0);
    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}
