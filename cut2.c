//Main Program
#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
#include <unistd.h>
#include <math.h>
#define SIZE 18
#define SCR 1000
#define OBJ 500

// FUNCTIONS
void fill(float sman[SIZE],float filled_sman[SIZE]);
void draw_spot(float xp,float yp,int length,float dt,int filled,int color);
void draw_lengths(float sman[SIZE],int length,float xcent,float ycent,int spots,float head_r,int color);
float dist(float x1,float y1,float x2,float y2);
int end_obj(float *p, int n, float search);
int end_cscreen(float *curr_screen[SCR],int n,float search);
void draw_screen(float *curr_screen[SCR],int spots,int color);
void draw_width(float width, float x,float y,float nx,float ny);
void draw_arb(float new[OBJ],int spots,int color,int screen3);
void draw_new(float new[OBJ]);
void perform_action(char a);

// MARKERS
float regular=12349;
float end_curr_screen=123456;
float new_object=12345;
float stickman_mark=12347;
float blank=54321;
float end_arb=5432;

// IMPORTANT VARIABLES
int uwait=50000;
int ncap=0;
int ncells=0;
float xcurrent=0;
int stickdesc=20;
int arbdesc=20;

void draw_width(float width, float x,float y,float nx,float ny){				//draws thicker lines
	int M;								
	float F;
	float plusang=M_PI/2;	
	float angle=atan((ny-y)/(x-nx));
	for(F=0;F<width;F+=.2){	
		gfx_line(x+F*cos(angle+plusang),y-F*sin(angle+plusang),nx+F*cos(angle+plusang),ny-F*sin(angle+plusang));
		gfx_line(x-F*cos(angle+plusang),y+F*sin(angle+plusang),nx-F*cos(angle+plusang),ny+F*sin(angle+plusang));
	}
}
void fill(float sman[SIZE],float filled_sman[SIZE]){	// Nature of stickman matrix is such that there are 18 angles but some
	filled_sman[0]=sman[0];				// simply backtrack (and need to be exactly opposite other angles).
	filled_sman[1]=sman[1];				// This function automatically fills in these angles in a matrix so you dont have
	filled_sman[2]=sman[1]+M_PI;			// to write +M_PI 8 times every time you initialize the matrix
	filled_sman[3]=sman[0]+M_PI;
	filled_sman[4]=sman[4];
	filled_sman[5]=sman[5];
	filled_sman[6]=sman[5]+M_PI;
	filled_sman[7]=sman[4]+M_PI;
	filled_sman[8]=sman[8];
	filled_sman[9]=sman[9];
	filled_sman[10]=sman[10];
	filled_sman[11]=sman[10]+M_PI;
	filled_sman[12]=sman[9]+M_PI;
	filled_sman[13]=sman[13];
	filled_sman[14]=sman[14];
	filled_sman[15]=sman[14]+M_PI;
	filled_sman[16]=sman[13]+M_PI;
	filled_sman[17]=sman[17];
}
void draw_spot(float xp,float yp,int length,float dt,int filled,int color){	//draws the spots for all objects.  Also draws the stickmans head (thats why you can 
	if(filled==0){								//leave the circle unfilled and uncolored.
		gfx_color(color,color==100?100:0,color==100?color:0);
	}
	float F;
	float x1,y1,x2,y2;
	for(F=0;F<2*M_PI;F+=dt){
		x1=xp+length*cos(F);
		y1=yp-length*sin(F);
		x2=xp+length*cos(F+dt);
		y2=yp-length*sin(F+dt);
		if(filled==0){
			draw_width(2.5,xp,yp,x1,y1);
		}else{
			draw_width(2.5,x1,y1,x2,y2);
		}
	}
	gfx_color(color,color,color);
}
void draw_lengths(float sman[SIZE],int length,float xcent,float ycent,int spots,float head_r,int color){	//draws a stickman based on its angle matrix,
	gfx_color(color,color,color);										//center, whether you want spots, the length of
	int F;													//the limbs, the radius of the head, and color 
	float xp=xcent,yp=ycent,nx,ny;										//(so you can make a grey one to show where last
	for(F=0;F<SIZE;F++){											//frame was)
		nx=xp+length*cos(sman[F]);
		ny=yp-length*sin(sman[F]);
		if(F==0||F==1||F==4||F==5||F==8||F==9||F==10||F==13||F==14){
		draw_width(2.5,xp,yp,nx,ny);
		}
		if(spots==1){
			if(F==0||F==1||F==2||F==5||F==6||F==9||F==10||F==11||F==14||F==15){
				draw_spot(xp,yp,length/10,1,0,color);
			}
			if(F==17){
				nx=xp+head_r*2*cos(sman[F]);
				ny=yp-head_r*2*sin(sman[F]);
				draw_spot(nx,ny,length/10,1,0,color);
			}
		}
		if(F==SIZE-1){
			draw_spot(xp+(head_r)*cos(sman[F]),yp-(head_r)*sin(sman[F]),head_r,.3,1,color);
		}
		xp=nx;
		yp=ny;
	}
}
void draw_arb(float new[OBJ],int spots,int color,int screen3){			//reads the matrix for an arbitrary shape to draw the shape.
	int end=end_obj(new,1,end_arb);
	int F;
	float x1=new[1]-(screen3==1?xcurrent:0);
	float y1=new[2];
	float x2,y2;
	for(F=0;F<(end-arbdesc)/4;F++){
		x2=x1+new[arbdesc+4*F+2]*cos(new[arbdesc+4*F]);
		y2=y1+new[arbdesc+4*F+2]*sin(new[arbdesc+4*F]);
		if(new[arbdesc+4*F+1]==0){
			draw_width(2.5,x1,y1,x2,y2);
		}else if(new[arbdesc+4*F+1]==1){
			draw_spot(.5*x1+.5*x2,.5*y1+.5*y2,.5*dist(x1,y1,x2,y2),.2,1,250);
		}
		if(spots==1){
			draw_spot(x1,y1,5,.2,0,color);
			draw_spot(x2,y2,5,.2,0,color);
		}
		x1=x2;
		y1=y2;
	}
}

float dist(float x1,float y1,float x2,float y2){			//distance between two points
	return sqrt(pow((x2-x1),2)+pow((y1-y2),2));
}
int end_obj(float *p, int n, float search){
	int found=0,iter=0;
	while(found<n){
		if(p[iter]+1>search && p[iter]-1<search){
			found++;
		}
		iter++;
	}
	iter--;
	return iter;
}
int end_cscreen(float *curr_screen[SCR],int n,float search){	//tells which element the nth 'search' number is in for different size matrix.
	int found=0,iter=0;					//only works (and only is used) for n=1;
	while(found<n){
		if(curr_screen[iter][0]+1>search && curr_screen[iter][0]-1<search){
			found++;
			if(search+1>end_curr_screen && search-1<end_curr_screen){
				return iter;
			}
		}
		iter++;
		if(curr_screen[iter][0]+1>end_curr_screen && curr_screen[iter][0]-1<end_curr_screen && search != end_curr_screen){
			if(search+1>end_curr_screen && search-1<end_curr_screen){
				return iter;
			}else{
				return -1;
			}
		}
	}	
	return --iter;
}
void draw_screen(float *curr_screen[SCR],int spots,int color){		//reads thrugh the matrix curr_screen to draw everything in it
	gfx_color(color,color,color);
	int done=0,F;
	float temp[SIZE];
	int curr_obj=0;
	while(done==0){
		if(curr_screen[curr_obj][0]+1>stickman_mark && curr_screen[curr_obj][0]-1<stickman_mark){
			if(xcurrent-100<curr_screen[curr_obj][1] && xcurrent+1100>curr_screen[curr_obj][1]){
				for(F=0;F<SIZE;F++){
					temp[F]=curr_screen[curr_obj][stickdesc+F];
				}
				draw_lengths(temp,curr_screen[curr_obj][3],curr_screen[curr_obj][1]-xcurrent,curr_screen[curr_obj][2],spots==1?1:0,curr_screen[curr_obj][3]/2.5,color);	
			}
			if(curr_screen[curr_obj+1][0]+1>end_curr_screen && curr_screen[curr_obj+1][0]-1<end_curr_screen){done=1;
			}else{curr_obj++;}
		}else if(curr_screen[curr_obj][0]+1>blank && curr_screen[curr_obj][0]-1<blank){			//doesnt draw deleted figure
			if(curr_screen[curr_obj+1][0]+1>end_curr_screen && curr_screen[curr_obj+1][0]-1<end_curr_screen){done=1;
			}else{curr_obj++;}			
		}else if(curr_screen[curr_obj][0]+1>regular && curr_screen[curr_obj][0]-1<regular){
			if(xcurrent-100<curr_screen[curr_obj][1] && xcurrent+1100>curr_screen[curr_obj][1]){
				int M=0;
				float temporary[OBJ];
				while((curr_screen[curr_obj][M]+1<end_arb || curr_screen[curr_obj][M]-1>end_arb)){
					temporary[M]=curr_screen[curr_obj][M];
					M++;
				}
				temporary[M]=curr_screen[curr_obj][M];
				draw_arb(temporary,spots,color,1);
			}
			if(curr_screen[curr_obj+1][0]+1>end_curr_screen && curr_screen[curr_obj+1][0]-1<end_curr_screen){done=1;
			}else{curr_obj++;}
		}else if(curr_screen[curr_obj][0]+1>end_curr_screen && curr_screen[curr_obj][0]-1<end_curr_screen){
			done=1;
		}
	}
}
void perform_action(char a){
	switch(a){
		FILE *walker;
		case 'd': 
			if((walker=fopen("Walk.mot","r"))==NULL){
				printf("file couldnt be opened");
			}else{
				int stop;
				fscanf(walker,"%d ",&stop);
				int i,j;
				for(j=1;j<stop;j++){
					for(i=0;i<OBJ;i++){
						float f;
						fscanf(walker,"%f ",&f);
						int m=rand()%1000;
						int n=rand()%600;
						gfx_point(m,n);gfx_flush();
						printf("%f ",f);
					}
					fscanf(walker,"\n");
					printf("\n");
				}
				fclose(walker);
			}
		break;
	}
}
void initialize_level(float *curr_screen[SCR]){
	FILE *level;
	if((level=fopen("one.lev","r"))==NULL){
		printf("file couldnt be opened");
	}else{
		int stop;
		fscanf(level,"%d ",&stop);
		int i,j;
		for(j=0;j<=stop;j++){
			for(i=0;i<OBJ;i++){
				float f;
				fscanf(level,"%f ",&f);
				curr_screen[j][i]=f;
			}
			fscanf(level,"\n");
		}
	}	
}
int main(void){
	float xmax=1000,ymax=600;
	gfx_open(xmax,ymax,"hey");
	float *curr_screen[SCR];
	int F;
	for(F=0;F<SCR;F++){
		curr_screen[F]=malloc(OBJ*sizeof(float*));
	}
	curr_screen[0][0]=end_curr_screen;
	initialize_level(curr_screen);
	char a;
	while(1){
		if(gfx_event_waiting()==1){
			a=gfx_wait();
			perform_action(a);
		}
//		calc_next_screen(curr_screen);
		draw_screen(curr_screen,0,250);
		gfx_flush();
		usleep(uwait);
		gfx_clear();
	}
}
