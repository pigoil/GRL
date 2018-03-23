#ifndef GR_PID_H
#define GR_PID_H

#include "gr_core.h"

GR_BEGIN_NAMESPACE

class GRPid;

class GRPid
{
	public:
		struct Parameters
		{
			float kp;			float ti;
			float td;			float ur;
			float un;			float time;
		};
		GRPid(float kp,float ti,float td,float ur,float un,float time);
		GRPid(Parameters p);
		
		virtual ~GRPid(){};

		float pidOut(float exp,float mea);
		float kp(){return _para.kp;}; void setKp(float f){_para.kp=f;update_data();};
		float ti(){return _para.ti;}; void setTi(float f){_para.ti=f;update_data();};
		float td(){return _para.td;}; void setTd(float f){_para.td=f;update_data();};
		float ur(){return _para.ur;}; void setUr(float f){_para.ur=f;update_data();};
		float un(){return _para.un;}; void setUn(float f){_para.un=f;update_data();};
		float t() {return _para.time;}; 	void setT (float f){_para.time=f;update_data();};
		Parameters parameters(){return _para;};
		void setParameters(Parameters p){_para = p;update_data();};
		
	private:
		Parameters _para;
		float Uk,ekk,ekkk;
		float k1;
		float k2;
		float k3;

		void update_data();
};

GR_END_NAMESPACE

#endif
