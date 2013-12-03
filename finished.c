#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
#include <unistd.h>
#include <math.h>
#define SIZE 18
#define SCR 1000
#define OBJ 500
#define MEM 100000

// FUNCTIONS
void fill(float sman[SIZE],float filled_sman[SIZE]);
void draw_spot(float xp,float yp,int length,float dt,int filled,int color);
void draw_lengths(float sman[SIZE],int length,float xcent,float ycent,int spots,float head_r,int color);
float dist(float x1,float y1,float x2,float y2);
int check_point(float xpoint,float ypoint,int length,float sman[SIZE],float xcent,float ycent,float head_r);
void play(int grey,int color,int screen);
int end_obj(float *p, int n, float search);
int end_cscreen(float *curr_screen[SCR],int n,float search);
int end_sscreen(float *curr_screen[MEM],int n,float search);
void add_stickman(float *curr_screen[SCR],float xcent,float ycent,int length);
void find_bubble(float *curr_screen[SCR],float x,float y,int remove);
void draw_screen(float *curr_screen[SCR],int spots,int color);
void draw_width(float width, float x,float y,float nx,float ny);
void draw_arb(float new[OBJ],int spots,int color,int screen3);
void draw_new(float new[OBJ]);
int check_arb_point(float x,float y,float new[OBJ]);

// MARKERS
float regular=12349;
float end_curr_screen=123456;
float new_object=12345;
float stickman_mark=12347;
float blank=54321;
float end_arb=5432;

// IMPORTANT VARIABLES
int wait=50000;
int ncap=0;
int ncells=0;
float xcurrent=0;
int stickdesc=20;
int arbdesc=20;

// MEMORY
float *saver[MEM];

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
int check_point(float xpoint,float ypoint,int length,float sman[SIZE],float xcent,float ycent,float head_r){	//checks if a point is within 5 pixels of
	int F;													//one of the spots that can control something
	float xp=xcent,yp=ycent,nx,ny;										//in the stickman -- returns an int that tells
	int save=-1;												//which one.
	for(F=0;F<SIZE;F++){
		if(F==SIZE-1){length=head_r*2;}
		nx=xp+length*cos(sman[F]);
		ny=yp-length*sin(sman[F]);
		if(dist(xpoint,ypoint,xp,yp)<length/10){
			if(F!=4 && F!=8 && F!=13 && F!=17){
				save=F;
			}
			if(save==1||save==2||save==5||save==6||save==10||save==11||save==14||save==15){
				return save;
			}
		}
		if(F==SIZE-1){
			if(dist(xpoint,ypoint,nx,ny)<length/10 && save==-1){
				save=SIZE;
			}			
		}
		xp=nx;
		yp=ny;
	}
	if(save==-1){return -1;}
	return save;
}
void play(int grey,int color,int screen){		//reads the saver with a pause 'wait' in between each screen to play the animation
	int F,M,T,N;
	float *curr_s[SCR];
	for(F=0;F<SCR;F++){
		curr_s[F]=malloc(OBJ*sizeof(float*));
	}
	for(F=0;F<ncap;F++){
		if(grey==0){gfx_clear();}
		if(grey==0||F==screen){
			if(F==0){
				for(M=0;M<=end_sscreen(saver,1,end_curr_screen);M++){
					for(N=0;N<OBJ;N++){
						curr_s[M][N]=saver[M][N];
					}
				}
			}else{
				for(M=end_sscreen(saver,F,end_curr_screen)+1,T=0;M<=end_sscreen(saver,F+1,end_curr_screen);M++,T++){
					for(N=0;N<OBJ;N++){
						curr_s[T][N]=saver[M][N];
					}
				}
			}
			draw_screen(curr_s,grey,color);
			gfx_flush();
		}
		if(grey==0){usleep(wait);}
	}
	for(F=0;F<SCR;F++){
		free(curr_s[F]);
	}
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
int end_sscreen(float *curr_screen[MEM],int n,float search){	//tells which element the nth 'search' number is in.
	int found=0,iter=0;
	if(n==0){return 0;}
	while(found<n){
		if(curr_screen[iter][0]+1>search && curr_screen[iter][0]-1<search){
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
void add_stickman(float *curr_screen[SCR],float xcent,float ycent,int length){			//just adds a stickman to the screen
	float sman[SIZE]={5*M_PI/4+.1,11*M_PI/8,0,0,7*M_PI/4-.1,13*M_PI/8,0,0,M_PI/2,5*M_PI/4+.1,11*M_PI/8,0,0,7*M_PI/4-.1,13*M_PI/8,0,0,M_PI/2};
	float filled_sman[SIZE]; 
	fill(sman,filled_sman);
	int start=end_cscreen(curr_screen,1,end_curr_screen);
	curr_screen[start][0]=stickman_mark;
	curr_screen[start][1]=xcent;
	curr_screen[start][2]=ycent;
	curr_screen[start][3]=length;
	curr_screen[start][4]=0;    //xvelocity
	curr_screen[start][5]=0;    //yvelocity
	curr_screen[start][6]=0;    //xacc
	curr_screen[start][7]=0;    //yacc
	curr_screen[start][8]=0;    //this is what gives information about saved sequences.				
	int F;
	for(F=0;F<SIZE;F++){
		curr_screen[start][stickdesc+F]=filled_sman[F];
	}
	curr_screen[start][stickdesc+SIZE]=new_object;
	curr_screen[start+1][0]=end_curr_screen;
}
int check_arb_point(float x,float y,float new[OBJ]){			//checks an arbitrary drawing to see if you clicked one of its spots.
	int end=end_obj(new,1,end_arb);
	int F;
	float x1=new[1];
	float y1=new[2];
	float x2,y2;
	int save=-1;
	for(F=0;F<(end-arbdesc+1)/4;F++){
		x2=x1+new[arbdesc+4*F+2]*cos(new[arbdesc+4*F]);
		y2=y1+new[arbdesc+4*F+2]*sin(new[arbdesc+4*F]);
		if(dist(x1,y1,x,y)<5){
			save=F;
		}
		x1=x2;
		y1=y2;
		if(dist(x2,y2,x,y)<5){
			save=F+1;
		}
	}
	return save;
}	

	
void find_bubble(float *curr_screen[SCR],float x,float y,int remove){	//uses the check point functions to figure out where you clicked and if that is going to
	int done=0,F;								//change one of the shapes and then changes that shape.
	float temp[SIZE];							//different cases for stickman, arbitrary drawing
	int curr_obj=0;
	while(done==0){
		if(curr_screen[curr_obj][0]+1>stickman_mark && curr_screen[curr_obj][0]-1<stickman_mark){
			float length=curr_screen[curr_obj][3];
			for(F=0;F<SIZE;F++){
				temp[F]=curr_screen[curr_obj][stickdesc+F];
			}
			int nang=check_point(x,y,length,temp,curr_screen[curr_obj][1],curr_screen[curr_obj][2],length/2.5);
			if(remove==1 && nang!=-1){
				curr_screen[curr_obj][0]=blank;
				done=1;
				nang=-1;
			}
			if(nang>0 && nang<18){
				x=x+length*cos(temp[nang-1]+M_PI);
				y=y-length*sin(temp[nang-1]+M_PI);
			}
			if(nang==18){
				x=x+(length/2.5)*2*cos(temp[nang-1]+M_PI);
				y=y-(length/2.5)*2*sin(temp[nang-1]+M_PI);
			}
			if(nang>-1){
				int a=0;
				while(a!=1){
					a=gfx_wait();
				}
				float nxp=gfx_xpos(),nyp=gfx_ypos();
				if(nang==0){
					curr_screen[curr_obj][1]=nxp;
					curr_screen[curr_obj][2]=nyp;
					done=1;
				}
				float ang;
				if(nxp-x>0){
					ang=atan((y-nyp)/(nxp-x));
				}else{
					ang=atan((y-nyp)/(nxp-x))+M_PI;
				}
				if(nang>0){
					if(nang==1||nang==5||nang==10||nang==14){
						temp[nang]=temp[nang]+ang-temp[nang-1];
					}
					if(nang==9){
						temp[17]=temp[17]+ang-temp[8];
						temp[9]=temp[9]+ang-temp[8];
						temp[10]=temp[10]+ang-temp[8];
						temp[13]=temp[13]+ang-temp[8];
						temp[14]=temp[14]+ang-temp[8];
					}
					temp[nang-1]=ang;
				}
				float replace[SIZE];
				fill(temp,replace);
				for(F=0;F<SIZE;F++){
					curr_screen[curr_obj][stickdesc+F]=replace[F];
				}

			}
			if(curr_screen[curr_obj+1][0]+1>end_curr_screen && curr_screen[curr_obj+1][0]-1<end_curr_screen){done=1;
			}else{curr_obj++;}

		}else if(curr_screen[curr_obj][0]+1>blank && curr_screen[curr_obj][0]-1<blank){			//doesnt let you click on a deleted stickman
			if(curr_screen[curr_obj+1][0]+1>end_curr_screen && curr_screen[curr_obj+1][0]-1<end_curr_screen){done=1;
			}else{curr_obj++;}
		}else if(curr_screen[curr_obj][0]+1>regular && curr_screen[curr_obj][0]-1<regular){
			int M=0;
			float temporary[OBJ];
			while((curr_screen[curr_obj][M]+1<end_arb || curr_screen[curr_obj][M]-1>end_arb)){
				temporary[M]=curr_screen[curr_obj][M];
				M++;
			}
			temporary[M]=curr_screen[curr_obj][M];
			int nang=check_arb_point(x,y,temporary);
			if(remove==1 && nang!=-1){
				curr_screen[curr_obj][0]=blank;
				nang=-1;
				done=1;
			}
			if(nang>0){
				x=x+temporary[arbdesc-4+nang*4+2]*cos(temporary[arbdesc-4+nang*4]+M_PI);
				y=y+temporary[arbdesc-4+nang*4+2]*sin(temporary[arbdesc-4+nang*4]+M_PI);
				done=1;
			}
			if(nang>-1){
				int a=0;
				while(a!=1){
					a=gfx_wait();
				}
				float nxp=gfx_xpos(),nyp=gfx_ypos();
				if(nang==0){
					curr_screen[curr_obj][1]=nxp;
					curr_screen[curr_obj][2]=nyp;
					done=1;
				}
				float ang;
				if(nxp-x>0){
					ang=atan((y-nyp)/(nxp-x));
				}else{
					ang=atan((y-nyp)/(nxp-x))+M_PI;
				}
				ang*=-1;
				if(nang>0){
					float added_angle=ang-curr_screen[curr_obj][arbdesc-4+nang*4];	//stopped
					curr_screen[curr_obj][arbdesc-4+nang*4]=ang;
					int Q;
					for(Q=nang+1;Q<=(M-arbdesc+1)/4;Q++){
						curr_screen[curr_obj][arbdesc-4+Q*4]+=added_angle;
					}
					done=1;
				}
			}
			if(curr_screen[curr_obj+1][0]+1>end_curr_screen && curr_screen[curr_obj+1][0]-1<end_curr_screen){done=1;
			}else{curr_obj++;}
		}
	}
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
		}
	}
}
void draw_new(float new[OBJ]){
	int curr_shape=arbdesc;
	int shape=0;
	char a;
	float x1,y1;
	while(a!=1){
		a=gfx_wait();
		if(a=='c'){shape=1;}
		if(a=='l'){shape=0;}
		if(a==1){
			x1=gfx_xpos();
			y1=gfx_ypos();
			new[1]=x1;
			new[2]=y1;
			new[3]=0;	//xvelocity
			new[4]=0;	//yvelocity
			new[5]=0;	//xacc
			new[6]=0;	//yacc
			new[7]=0;	//circular velocity
			new[arbdesc]=end_arb;
		}	
	}
	int stop=0;
	while(stop==0){	
		a=gfx_wait();
		if(a=='x'){
			stop=1;
			new[1]+=xcurrent;
		}
		if(a=='l'){
			shape=0;
		}
		if(a=='c'){
			shape=1;
		}
		if(a=='z'){
			if(curr_shape>arbdesc){
				curr_shape-=4;
				new[curr_shape]=end_arb;
				gfx_clear();
				draw_arb(new,1,250,0);
				int Y;
				float x3=new[1],y3=new[2],x4,y4;
				for(Y=0;Y<(end_obj(new,1,end_arb)-4)/4;Y++){
					x4=(x3+new[3+(Y+1)*4]*cos(new[1+(Y+1)*4]));
					y4=(y3+new[3+(Y+1)*4]*sin(new[1+(Y+1)*4]));
					x3=x4;
					y3=y4;
				}
				x1=x4;
				y1=y4;
			}
			if(curr_shape==arbdesc){
				char b=0;
				while(b!=1){
					b=gfx_wait();
					if(b=='c'){shape=1;}
					if(b=='l'){shape=0;}
					if(b==1){
						x1=gfx_xpos();
						y1=gfx_ypos();
						new[1]=x1;
						new[2]=y1;
						new[arbdesc]=end_arb;
					}	
				}
			}
		}
		if(a==1){
			float x2=gfx_xpos();
			float y2=gfx_ypos();
			if(x2-x1<.0001 && x2-x1>-.0001){x2+=.001;}
			float ang;
			if(x2-x1>0){
				ang=atan((y1-y2)/(x2-x1));
			}else{
				ang=atan((y1-y2)/(x2-x1))+M_PI;
			}
			ang*=-1;
			new[curr_shape]=ang;
			new[curr_shape+1]=shape;
			new[curr_shape+2]=dist(x1,y1,x2,y2);
			new[curr_shape+3]=2.5;
			new[curr_shape+4]=end_arb;
			curr_shape+=4;
			x1=x2;
			y1=y2;
			gfx_clear();
			draw_arb(new,1,250,0);
		}
	}
}
int main(void){
	usleep(1000000);
	float xmax=1000,ymax=600;
	gfx_open(xmax,ymax,"hey");
	float *curr_screen[SCR];
	int F;
	for(F=0;F<SCR;F++){
		curr_screen[F]=malloc(OBJ*sizeof(float));

	}
	for(F=0;F<MEM;F++){
		saver[F]=malloc(OBJ*sizeof(float));
	}	
	curr_screen[0][0]=end_curr_screen;
	add_stickman(curr_screen,xmax/2+100,ymax/2,50);	
	while(1){
		gfx_clear();
		play(1,100,ncap-1);
		int F;
		draw_screen(curr_screen,1,250);
		gfx_flush();
		char c;	
		c=gfx_wait();
		if(c==1){
			float x=gfx_xpos();
			float y=gfx_ypos();
			find_bubble(curr_screen,x,y,0);
		}
		if(c=='d'){
			float x=gfx_xpos();
			float y=gfx_ypos();
			find_bubble(curr_screen,x,y,1);
		}			
		if(c=='c'){
			int M,N;
			for(M=0;M<end_cscreen(curr_screen,1,end_curr_screen)+1;M++){
				for(N=0;N<OBJ;N++){
					saver[M+ncells][N]=curr_screen[M][N];
				}
			}
			ncap++;
			ncells+=1+end_cscreen(curr_screen,1,end_curr_screen);
		}
		if(c=='p'){
			play(0,250,ncap-1);
		}
		if(c=='a'){
			add_stickman(curr_screen,xmax/2+xcurrent,ymax/2,50);	
		}
		if(c=='=' && wait<1000000){
			wait+=25000;
		}
		if(c=='-' && wait>25000){
			wait-=25000;
		}
		if(c=='n'){
			gfx_clear();
			float new[OBJ];
			new[0]=regular;
			draw_new(new);
			int a=end_cscreen(curr_screen,1,end_curr_screen);
			int b=OBJ;
			int M;
			for(M=0;M<b;M++){
				curr_screen[a][M]=new[M];
			}
			curr_screen[a+1][0]=end_curr_screen;
		}
		if(c==';'){
			xcurrent+=500;
		}
		if(c=='l' && xcurrent>490){
			xcurrent-=500;
		}
		if(c=='f'){
			FILE *walker;
			if((walker=fopen("Walk.mot","w"))==NULL){
				printf("File Could not be opened");
			}else{
				int i,j;
				for(j=1;j<ncap;j++){
					for(i=0;i<OBJ;i++){
						fprintf(walker,"%f ",saver[j*2][i]-saver[j*2-2][i]);
					}
					fprintf(walker,"\n");
				}
				fclose(walker);
			}
		}
	}
}
