#include "fft.h"
#include "complex.h"
#include "trig.h"

static float new_[1024];
static float new_im[1024];

void fft_magnitude(float* q, float* w, int n, int m, float* magnitudes) {
	int a,b,r,d,e,c;
	int k;
	a=n/2;
	b=1;
	int i,j;
	float real=0,imagine=0;

	// Ordering algorithm
	for(i=0; i<(m-1); i++){
		d=0;
		for (j=0; j<b; j++){
			for (c=0; c<a; c++){	
				e=c+d;
				new_[e]=q[(c*2)+d];
				new_im[e]=w[(c*2)+d];
				new_[e+a]=q[2*c+1+d];
				new_im[e+a]=w[2*c+1+d];
			}
			d+=(n/b);
		}		
		for (r=0; r<n;r++){
			q[r]=new_[r];
			w[r]=new_im[r];
		}
		b*=2;
		a=n/(2*b);
	}

	b=1;
	k=0;
	for (j=0; j<m; j++){	
		//MATH
		for(i=0; i<n; i+=2){
			if (i%(n/b)==0 && i!=0)
				k++;
			real=mult_real(q[i+1], w[i+1], cosine(-PI*k/b), sine(-PI*k/b));	
			imagine=mult_im(q[i+1], w[i+1], cosine(-PI*k/b), sine(-PI*k/b));
			new_[i]=q[i]+real;
			new_im[i]=w[i]+imagine;
			new_[i+1]=q[i]-real;
			new_im[i+1]=w[i]-imagine;
		}
		for (i=0; i<n; i++){
			q[i]=new_[i];
			w[i]=new_im[i];
		}
		//REORDER
		for (i=0; i<n/2; i++){
			new_[i]=q[2*i];
			new_[i+(n/2)]=q[2*i+1];
			new_im[i]=w[2*i];
			new_im[i+(n/2)]=w[2*i+1];
		}
		for (i=0; i<n; i++){
			q[i]=new_[i];
			w[i]=new_im[i];
		}
		b*=2;
		k=0;		
	}

	// Calculate magnitudes
	for(i=0; i<n; i++) {
		magnitudes[i] = q[i]*q[i] + w[i]*w[i];
	}
}

float fft(float* q, float* w, int n, int m, float sample_f) {
	int place, i;
	float max;
	float magnitudes[1024];

	// Compute FFT and get magnitudes
	fft_magnitude(q, w, n, m, magnitudes);

	// Find dominant frequency
	max=0;
	place=1;
	for(i=1;i<(n/2);i++) { 
		if(max < magnitudes[i]) {
			max=magnitudes[i];
			place=i;
		}
	}
	
	float s=sample_f/n; //spacing of bins
	float frequency = (sample_f/n)*place;
	
	//curve fitting for more accuracy
	float y1=magnitudes[place-1],y2=magnitudes[place],y3=magnitudes[place+1];
	float x0=s+(2*s*(y2-y1))/(2*y2-y1-y3);
	x0=x0/s-1;
	
	if(x0 <0 || x0 > 2) { //error
		return frequency;
	}
	if(x0 <= 1)  {
		frequency=frequency-(1-x0)*s;
	}
	else {
		frequency=frequency+(x0-1)*s;
	}
	
	return frequency;
}
