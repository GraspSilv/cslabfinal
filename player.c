//Main Program
#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
#include <unistd.h>
#include <math.h>
#include <strings.h>
#define SIZE 18
#define SCR 1000
#define OBJ 500
#define STICKS 30
#define max_frames 20

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
void perform_action(float *move_holder[STICKS*max_frames],float *curr_screen[SCR],char a,int stick);
void back_to_normal(float *curr_screen[SCR],int F,int arms);
void attached(float *curr_screen[SCR],float *move_holder[STICKS*max_frames],int fired,int who);
void calc_next_screen(float *curr_screen[SCR],float *move_holder[STICKS*max_frames]);
void check_weapons(float *curr_screen[SCR],float *move_holder[STICKS*max_frames]);

// MARKERS
float regular=12349;
float end_curr_screen=123456;
float new_object=12345;
float stickman_mark=12347;
float blank=54321;
float end_arb=5432;
float end_move=8765;
float weapon_change_end_move=9876;
float punch_end_move=3210;

// IMPORTANT VARIABLES
int count=0;
int inarow=0;
int right=1;
int uwait=50000;
int ncap=0;
int ncells=0;
float xcurrent=0;
int stickdesc=20;
int arbdesc=20;
int nweapons=3;
int nbullets=4;
int bull_vel=60;

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
	gfx_color(250,250,color);											//center, whether you want spots, the length of
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
	gfx_color(250,250,250);
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
				draw_lengths(temp,curr_screen[curr_obj][3],curr_screen[curr_obj][1]-xcurrent,curr_screen[curr_obj][2],spots==1?1:0,curr_screen[curr_obj][3]/2.5,curr_obj==0?0:color);	
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
void perform_action(float *move_holder[STICKS*max_frames],float *curr_screen[SCR],char a,int stick){
	stick=end_cscreen(curr_screen,stick,stickman_mark);
	if(curr_screen[stick][13]>.1){
	char *po="Walk.mot";
	switch(a){
		FILE *walker;
		case 'w':
			if(curr_screen[stick][2]>510){
				curr_screen[stick][5]=-20;
			}
		break;
		case 'e':
			if(abs(curr_screen[stick][11])<.1){
				curr_screen[stick][11]++;
			}
		break;
		case 1:
			if(a==1 && curr_screen[stick][10]<.1){
				if(curr_screen[stick][14]>.1){
					po="Punch.mot";
				}else{
					po="Punchback.mot";
				}
			}else{
//				calc_next_screen(curr_screen,move_holder);
				if(curr_screen[stick][16]<.1){
					attached(curr_screen,move_holder,1,stick);
					curr_screen[stick][16]=20;
				}
				break;
			}			
		case 'd': 
			if(a=='d'){
				po=(stick==0?"Walk.mot":"Slowwalk.mot");
				curr_screen[stick][14]=1;
			}
		case 'a':
			if(a=='a'){
				po=(stick==0?"Back.mot":"Slowback.mot");
				curr_screen[stick][14]=0;
			}
			if(curr_screen[stick][8]<.1){
				if((walker=fopen(po,"r"))==NULL){
					printf("File Couldnt Be Opened\n");
				}else{
					int stop;
					fscanf(walker,"%d ",&stop);
					int i,j;
					for(j=0;j<stop;j++){
						for(i=0;i<OBJ;i++){
							float f;
							fscanf(walker,"%f ",&f);
							move_holder[(int)(curr_screen[stick][9]-1)*max_frames+j][i]=f;	
							curr_screen[stick][8]=1;
						}
						fscanf(walker,"\n");
					}
					move_holder[(int)(curr_screen[stick][9]-1)*max_frames+stop][0]=end_move;
					move_holder[(int)(curr_screen[stick][9]-1)*max_frames+stop][1]=0;	
					if(a==1){
						move_holder[(int)(curr_screen[stick][9]-1)*max_frames+j][1]=punch_end_move;	
					}
					fclose(walker);
				}
			}
		break;
			
	}
	}
	if(stick==1){
	int F;
	for(F=0;F<50;F++){
		if(gfx_event_waiting()==1){
			char a;
			a=gfx_wait();
		}
	}
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
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark){
			back_to_normal(curr_screen,F,1);
		}
	}
}
int total_cscreen(float *curr_screen[SCR],int search){
	int F,tot=0;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>search && curr_screen[F][0]-1<search){
			tot++;
		}
	}
	return tot;
}
void back_to_normal(float *curr_screen[SCR],int F,int arms){
	FILE *norm;
	if((norm=fopen("norm.man","r"))==NULL){
		printf("file couldnt be opened1");
	}else{
		float n;
		fscanf(norm,"%f ",&n);
		int M;
		for(M=0;M<OBJ;M++){
			float f;
			fscanf(norm,"%f ",&f);
			if(M>=stickdesc && M<stickdesc+9){
				curr_screen[F][M]=f;
			}
			if(arms==1 && M>=stickdesc+9 && M<stickdesc+18){
				curr_screen[F][M]=f;
			}
		}
		fclose(norm);
	}
}
void punch_kill(float *curr_screen[SCR],int F){
	float x=curr_screen[F][1],y=curr_screen[F][2];
	int M;
	for(M=0;M<end_cscreen(curr_screen,1,end_curr_screen);M++){
		if(curr_screen[M][0]+1>stickman_mark && curr_screen[M][0]-1<stickman_mark && M!=F){
			float x1=curr_screen[M][1],y1=curr_screen[M][2];
			if(dist(x,y,x1,y1)<100 && curr_screen[M][13]>.1 && x1>x && curr_screen[F][14]==1){
				if(F==0){
					curr_screen[M][13]=0;
				}else if(M==0){
					curr_screen[M][15]--;
				}
			}else if(dist(x,y,x1,y1)<100 && curr_screen[M][13]>.1 && x1<x && curr_screen[F][14]==0){
				if(F==0){
					curr_screen[M][13]=0;
				}else if(M==0){
					curr_screen[M][15]--;
				}
			}
		}
	}
}
void saved_action_enforce(float *curr_screen[SCR],float *move_holder[STICKS*max_frames]){
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark && curr_screen[F][8]>0.1){
			int M;
			for(M=0;M<OBJ;M++){
				curr_screen[F][M]+=move_holder[(int)((curr_screen[F][9]-1)*max_frames+curr_screen[F][8])-1][M];
			}
			if(move_holder[(int)(curr_screen[F][9]-1)*max_frames+(int)(curr_screen[F][8]+1)][0]+1>end_move && move_holder[(int)(curr_screen[F][9]-1)*max_frames+(int)(curr_screen[F][8]+1)][0]-1<end_move){
				if(curr_screen[F][13]>.1){
					back_to_normal(curr_screen,F,0);
				}
				if(move_holder[(int)(curr_screen[F][9]-1)*max_frames+(int)(curr_screen[F][8]+1)][1]==weapon_change_end_move){
					curr_screen[F][11]+=.5;
				}else if(move_holder[(int)(curr_screen[F][9]-1)*max_frames+(int)(curr_screen[F][8]+1)][1]==punch_end_move){
					back_to_normal(curr_screen,F,1);
				}
				curr_screen[F][8]=0;
			}else{
				if(move_holder[(int)(curr_screen[F][9]-1)*max_frames+14][1]==punch_end_move && (int)(curr_screen[F][8]>10)){
					punch_kill(curr_screen,F);
				}
				curr_screen[F][8]++;
			}
		}
	}
}
void check_velocities(float *curr_screen[SCR]){
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark && curr_screen[F][13]>.1){
			curr_screen[F][5]+=curr_screen[F][7];
			curr_screen[F][4]+=curr_screen[F][6];
			curr_screen[F][2]+=curr_screen[F][5];
			curr_screen[F][1]+=curr_screen[F][4];
			if(curr_screen[F][5]>0 && curr_screen[F][2]>510){
				curr_screen[F][2]=511;
				curr_screen[F][5]=0;
				curr_screen[F][7]=0;
			}
			else if(curr_screen[F][2]<510){
				curr_screen[F][7]=2;
			}
		}else if(curr_screen[F][0]+1>regular && curr_screen[F][0]-1<regular){
			curr_screen[F][4]+=curr_screen[F][6];
			curr_screen[F][3]+=curr_screen[F][5];
			curr_screen[F][2]+=curr_screen[F][4];
			curr_screen[F][1]+=curr_screen[F][3];
		}
	}
}
void attached(float *curr_screen[SCR],float *move_holder[STICKS*max_frames],int fired,int who){
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark && curr_screen[F][13]>.1){
			if(curr_screen[F][16]>0){curr_screen[F][16]--;}
			float x=curr_screen[F][1],y=curr_screen[F][2],x1,y1,x2,y2;
			int M;
			for(M=0;M<15;M++){
				x1=x+curr_screen[F][3]*cos(curr_screen[F][stickdesc+M]);
				y1=y-curr_screen[F][3]*sin(curr_screen[F][stickdesc+M]);
				x=x1;
				y=y1;
				if(M==9){
					x2=x;
					y2=y;
				}
			}
			float add_ang=curr_screen[F+1][arbdesc]+(curr_screen[F][stickdesc+M-1]+M_PI/2);
			while(add_ang>0){
				add_ang-=2*M_PI;
			}
			while(add_ang<0){
				add_ang+=2*M_PI;
			}				
			curr_screen[F+1][1]=x-10*cos(curr_screen[F+1][arbdesc]-add_ang);
			curr_screen[F+1][2]=y-10*sin(curr_screen[F+1][arbdesc]-add_ang);
			if(add_ang>.1 || add_ang<-.1){
				M=0;
				while(curr_screen[F+1][arbdesc+M]+.1<end_arb || curr_screen[F+1][arbdesc+M]-.1>end_arb){
					curr_screen[F+1][arbdesc+M]-=add_ang;
					M+=4;
				}
				add_ang=curr_screen[F+2][arbdesc]+(curr_screen[F][stickdesc+M-1]-2*M_PI/3);
				M=0;
				if(curr_screen[F][8]<.1){
					while(curr_screen[F+2][arbdesc+M]+.1<end_arb || curr_screen[F+2][arbdesc+M]-.1>end_arb){
						curr_screen[F+2][arbdesc+M]-=add_ang;
						M+=4;
					}
				}
			}
			curr_screen[F+2][1]=x2-40*cos(curr_screen[F+2][arbdesc]-add_ang);
			curr_screen[F+2][2]=y2-40*sin(curr_screen[F+2][arbdesc]-add_ang);
			if(curr_screen[F][8]<.1){
				if(fired==1){
					int N;
					for(N=0;N<nbullets;N++){
						curr_screen[F+nweapons+1+N][1]=x;
						curr_screen[F+nweapons+1+N][2]=y;
						curr_screen[F+nweapons+1+N][0]=blank;
					}
					if(F==who){
						curr_screen[F+nweapons+1+(int)(curr_screen[F][12])][0]=regular;
					}
					if(curr_screen[F][10]<1.1){
						curr_screen[F+nweapons+1+(int)(curr_screen[F][12])][arbdesc]=curr_screen[F+(int)(curr_screen[F][10])][arbdesc+4];
						curr_screen[F][12]++;
						if(curr_screen[F][12]>nbullets-1){
							curr_screen[F][12]=0;
						}
					}else if(curr_screen[F][10]>1.1){
						int iter;
						for(iter=0;iter<3;iter++){
							curr_screen[F+nweapons+1+(int)(curr_screen[F][12])][0]=regular;
							float add;
							if(iter==0){ add=-.2;}
							if(iter==1){ add=0;}
							if(iter==2){ add=.2;}
							curr_screen[F+nweapons+1+(int)(curr_screen[F][12])][stickdesc]=curr_screen[F+(int)(curr_screen[F][10])][arbdesc+16]+add;
							curr_screen[F][12]++;
							if(curr_screen[F][12]>nbullets-1){
								curr_screen[F][12]=0;
							}
						}
					}
					for(N=0;N<nbullets;N++){
						curr_screen[F+nweapons+1+N][3]=bull_vel*cos(curr_screen[F+nweapons+1+N][arbdesc]);
						curr_screen[F+nweapons+1+N][4]=bull_vel*sin(curr_screen[F+nweapons+1+N][arbdesc]);
					}
				}
			}
		}
	}
}				
void check_weapons(float *curr_screen[SCR],float *move_holder[STICKS*max_frames]){
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark && curr_screen[F][13]>.1){
			if((curr_screen[F][11]-.1<1 && curr_screen[F][11]+.1>1)||(curr_screen[F][11]-.1<3 && curr_screen[F][11]+.1>3)){
				if(curr_screen[F][8]<.1){
				curr_screen[F][11]+=.5;
				char *p;
				if(curr_screen[F][10]<.1){
					if(curr_screen[F][11]-.1<1.5 && curr_screen[F][11]+.1>1.5){
						p="Blankput.mot";
					}else{
						p="Blanktake.mot";
					}
				}else if(curr_screen[F][10]<1.1){
					if(curr_screen[F][11]-.1<1.5 && curr_screen[F][11]+.1>1.5){
						p="Pistolput.mot";
					}else{	
						p="Pistoltake.mot";
					}
				}else if(curr_screen[F][10]<2.1){
					if(curr_screen[F][11]-.1<1.5 && curr_screen[F][11]+.1>1.5){
						p="Shotgunput.mot";
					}else{
						int M=0;
						while(curr_screen[F+2][M+arbdesc]+.1<end_arb || curr_screen[F+2][M+arbdesc]-.1>end_arb){
							curr_screen[F+2][M+arbdesc]+=M_PI;
							M+=4;
						}
						p="Shotguntake.mot";
					}
				}
				FILE *weapon;
				if((weapon=fopen(p,"r"))==NULL){
					printf("file couldnt be opened");
				}else{
					if(curr_screen[F][8]<.1){
						int stop;
						fscanf(weapon,"%d ",&stop);
						int i,j;
						for(j=0;j<stop;j++){
							for(i=0;i<OBJ;i++){
								float f;
								fscanf(weapon,"%f ",&f);
								move_holder[(int)(round((curr_screen[F][9]-1)*max_frames+j))][i]=f;	
								curr_screen[F][8]=1;
							}
							fscanf(weapon,"\n");
						}
						move_holder[(int)(round((curr_screen[F][9]-1)*max_frames+j))][0]=end_move;
						move_holder[(int)(round((curr_screen[F][9]-1)*max_frames+j))][1]=weapon_change_end_move;
						fclose(weapon);
					}
				}
				}
			}else if(curr_screen[F][11]-.1<2 && curr_screen[F][11]+.1>2){
				FILE *arms;
				if((arms=fopen("weapon.man","r"))==NULL){
					printf("file couldnt open");
				}else{
					int M;
					for(M=stickdesc+9;M<stickdesc+18;M++){
						float f;
						fscanf(arms,"%f ",&f);
						curr_screen[F][M]=f;
					}
					fclose(arms);
				}
				if(curr_screen[F][10]<.1){
					curr_screen[F+3][0]=blank;
					curr_screen[F+1][0]=regular;
				}else if(curr_screen[F][10]<1.1){
					curr_screen[F+1][0]=blank;
					curr_screen[F+2][0]=regular;
				}else if(curr_screen[F][10]<2.1){
					curr_screen[F+2][0]=blank;
					curr_screen[F+3][0]=blank;
				}
				curr_screen[F][11]++;
				if(curr_screen[F][10]+.1>nweapons-1 && curr_screen[F][10]-.1<nweapons-1){
					curr_screen[F][10]=0;
				}else{
					curr_screen[F][10]++;
				}
			}else if(curr_screen[F][11]-.1>1.5 || curr_screen[F][11]+.1<1.5){
				curr_screen[F][11]=0;
			}
			if(curr_screen[F][11]==1.5){
				inarow++;
			}
			if(inarow>20){
				curr_screen[F][11]=0;
				inarow=0;
			}
		}
	}
}
void xcurr_move(float *curr_screen[SCR]){
	if(curr_screen[0][1]>400 && curr_screen[0][1]<3000){
		xcurrent=curr_screen[0][1]-400;
	}else if(curr_screen[0][1]<400){
		xcurrent=0;
	}else{
		xcurrent=2600;
	}
}
void aim(float *curr_screen[SCR],float x2,float y2,int F){
	if(curr_screen[F][10]>.1 && curr_screen[F][8]<.1 && curr_screen[F][11]<.1){
	if(x2<0){
		x2=gfx_xpos(),y2=gfx_ypos();
	}
	float x=curr_screen[F][1],y=curr_screen[F][2],x1,y1;
	int M;
	for(M=0;M<9;M++){
		x1=x+curr_screen[F][3]*cos(curr_screen[F][stickdesc+M]);
		y1=y-curr_screen[F][3]*sin(curr_screen[F][stickdesc+M]);
		x=x1;
		y=y1;
	}
	x2+=xcurrent;
	float ang;
	if(x2-x>0){
		ang=atan((y-y2)/(x2-x));
	}else{
		ang=atan((y-y2)/(x2-x))+M_PI;
	}
	if(curr_screen[F][10]==2){
		ang+=.5;
	}
	while(curr_screen[F][14+stickdesc]>0){
		curr_screen[F][14+stickdesc]-=2*M_PI;
	}
	while(curr_screen[F][14+stickdesc]<0){
		curr_screen[F][14+stickdesc]+=2*M_PI;
	}
	ang-=curr_screen[F][14+stickdesc];
	while(ang>0){
		ang-=2*M_PI;
	}
	while(ang<0){
		ang+=2*M_PI;
	}
	if(ang<-.1 || ang>.1){
		for(M=9;M<17;M++){
			curr_screen[F][stickdesc+M]+=ang;
		}
	}
	}
}
void death(float *curr_screen[SCR]){
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark && curr_screen[F][13]>.1){
			int N,doneb;
			for(N=0;N<nbullets;N++){
				doneb=0;
				if(curr_screen[F+nweapons+1+N][0]+1>regular && curr_screen[F+nweapons+1+N][0]-1<regular){
					int S,x=curr_screen[F+nweapons+1+N][1],y=curr_screen[F+nweapons+1+N][2];
					for(S=0;S<20;S++){
						x+=(bull_vel/20)*cos(curr_screen[F+nweapons+1+N][arbdesc]);
						y+=(bull_vel/20)*sin(curr_screen[F+nweapons+1+N][arbdesc]);
						int M;
						for(M=0;M<end_cscreen(curr_screen,1,end_curr_screen);M++){
							if(curr_screen[M][0]+1>stickman_mark && curr_screen[M][0]-1<stickman_mark && curr_screen[M][13]>.1){			
								int R;
								float x1=curr_screen[M][1],y1=curr_screen[M][2],x2,y2;
								for(R=0;R<17;R++){
									x2=x1+curr_screen[M][3]*cos(curr_screen[M][stickdesc+R]);
									y2=y1-curr_screen[M][3]*sin(curr_screen[M][stickdesc+R]);
									int S;
									for(S=0;S<3;S++){
										float x3,y3;
										x3=x1+S*(x2-x1)/3;
										y3=y1+S*(y2-y1)/3;
										if(dist(x3,y3,x,y)<5){
											if(doneb<.1){
												curr_screen[M][15]--;
											}
											if(curr_screen[M][15]<0){
												curr_screen[M][13]=0;
											}
											curr_screen[F+nweapons+1+N][0]=blank;
											doneb=1;
										}
									}
									x1=x2;
									y1=y2;
								}
								x1+=(curr_screen[M][3]/2.5)*cos(curr_screen[M][stickdesc+R]);
								y1-=(curr_screen[M][3]/2.5)*sin(curr_screen[M][stickdesc+R]);
								if(dist(x1,y1,x,y)<(curr_screen[M][3])/2.5){
									curr_screen[F+nweapons+1+N][0]=blank;
									curr_screen[M][15]-=2;
								}
							}
						}
					}
				}
			}
		}
	}
}
void kill(float *curr_screen[SCR],float *move_holder[STICKS*max_frames]){
	int F;
	for(F=0;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark){
			if((curr_screen[F][13]<.1 && curr_screen[F][13]>-.1) || (curr_screen[F][15]<.1 && curr_screen[F][13]>-.1)){
				char *po;
				int a=rand()%3;
				switch(a){
					case 0: po="Death.mot"; break;
					case 1: po="Death1.mot"; break;
					case 2: po="Death2.mot"; break;
				}
				FILE *dead;
				if((dead=fopen(po,"r"))==NULL){
					printf("file couldnt be opened");
				}else{
						int stop;
						fscanf(dead,"%d ",&stop);
						int i,j;
						for(j=0;j<stop;j++){
							for(i=0;i<OBJ;i++){
								float f;
								fscanf(dead,"%f ",&f);
								move_holder[(int)(round((curr_screen[F][9]-1)*max_frames+j))][i]=f;	
								curr_screen[F][8]=1;
							}
							fscanf(dead,"\n");
						}
						move_holder[(int)round(((curr_screen[F][9]-1)*max_frames+j))][0]=end_move;
						move_holder[(int)round(((curr_screen[F][9]-1)*max_frames+j))][1]=1;
						fclose(dead);
				}
				curr_screen[F][13]--;
			}
		}
	}
}
void reset_stickman(float *curr_screen[SCR]){
	int start;
	for(start=1;start<end_cscreen(curr_screen,1,end_curr_screen);start++){
		if(curr_screen[start][0]+1>stickman_mark && curr_screen[start][0]-1<stickman_mark && curr_screen[start][13]<.1){
			curr_screen[start][17]++;
			if(curr_screen[start][17]>30){
			float sman[SIZE]={5*M_PI/4+.1,11*M_PI/8,0,0,7*M_PI/4-.1,13*M_PI/8,0,0,M_PI/2,5*M_PI/4+.1,11*M_PI/8,0,0,7*M_PI/4-.1,13*M_PI/8,0,0,M_PI/2};
			float filled_sman[SIZE]; 
			fill(sman,filled_sman);
			int a=rand()%500;
			int b=rand()%2;
			curr_screen[start][0]=stickman_mark;
			curr_screen[start][1]=(b==0?2000+a:-500-a);
			curr_screen[start][2]=300;
			curr_screen[start][3]=50;
			curr_screen[start][4]=0;    //xvelocity
			curr_screen[start][5]=0;    //yvelocity
			curr_screen[start][6]=0;    //xacc
			curr_screen[start][7]=0;    //yacc
			curr_screen[start][8]=0;    //this is what gives information about saved sequences.
			curr_screen[start][10]=0;   //this tells what weapon the stickman is using.
			curr_screen[start][11]=0;   //has to do with the changing the weapons -- what stage of change stickman is on
			curr_screen[start][12]=0;   //what bullet hes using
			curr_screen[start][13]=1;   //hes alive
			curr_screen[start][14]=1;   //direction he is facing (1 is right,0 is left)
			curr_screen[start][15]=curr_screen[start][18]+2;   //health
			curr_screen[start][16]=0;   //tells how long ago stickman fired a weapon
			curr_screen[start][17]=0;   //tells how long stick has been dead for
			curr_screen[start][18]++;   //tells how many times hes died
			int F;
			for(F=0;F<SIZE;F++){
				curr_screen[start][stickdesc+F]=filled_sman[F];
			}
			curr_screen[start][stickdesc+SIZE]=new_object;
			char *p;
			int S,R;
			FILE *wep;
				for(S=0;S<nweapons+nbullets;S++){
				switch(S){
					case 0:
						p="pistol.wep";
					break;
					case 1:
						p="shotgun.wep";
					break;
					case 2:
						p="rpg.wep";
					break;
					default:
						p="bullet.wep";
					break;
				}
				if((wep=fopen(p,"r"))==NULL){
					printf("cant open file");
				}else{
					for(R=0;R<OBJ;R++){
						float f;
					fscanf(wep,"%f ",&f);
						curr_screen[start+1+S][R]=f;
					}
				fclose(wep);
				curr_screen[start+1+S][0]=blank;
				}
			}
			}
		}
	}
}			
void stickman_ai(float *move_holder[SCR],float *curr_screen[SCR]){
	int F;
	for(F=1;F<end_cscreen(curr_screen,1,end_curr_screen);F++){
		if(curr_screen[F][0]+1>stickman_mark && curr_screen[F][0]-1<stickman_mark){
			if(curr_screen[F][1]-curr_screen[0][1]-90>0){
				perform_action(move_holder,curr_screen,'a',curr_screen[F][9]);
			}else if(curr_screen[F][1]-curr_screen[0][1]+90<0){
				perform_action(move_holder,curr_screen,'d',curr_screen[F][9]);
			}else{
				perform_action(move_holder,curr_screen,1,curr_screen[F][9]);
			}
		}
	}
}
void calc_next_screen(float *curr_screen[SCR],float *move_holder[STICKS*max_frames]){
	reset_stickman(curr_screen);
	stickman_ai(move_holder,curr_screen);
	saved_action_enforce(curr_screen,move_holder);	
	check_velocities(curr_screen);
	check_weapons(curr_screen,move_holder);
	attached(curr_screen,move_holder,0,0);
	xcurr_move(curr_screen);
	aim(curr_screen,-3,0,0);
	kill(curr_screen,move_holder);
	death(curr_screen);
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
	float *move_holder[STICKS*max_frames];
	for(F=0;F<STICKS*max_frames;F++){
		move_holder[F]=malloc(OBJ*sizeof(float*));
	}
	int full = 255;
	char a;
	while(1){
		if(gfx_event_waiting()==1){
			a=gfx_wait();
			perform_action(move_holder,curr_screen,a,1);
		}
		calc_next_screen(curr_screen,move_holder);
		draw_screen(curr_screen,0,250);
		gfx_color(255,0,0);
		draw_width(3, 30, 30, 30, 45);
		draw_width(3, 20, 38, 40, 38);
		draw_width(3, 60, 38, 60+curr_screen[0][15]*40, 38);
//		if (curr_screen[0][15] == 0) {
//			draw_width(5, 300, 200, 500, 500);
//			draw_width(5, 300, 500, 500, 300);
		}
		gfx_color(full,full,full);
		gfx_flush();
		usleep(uwait);
		gfx_clear();
	}
}
