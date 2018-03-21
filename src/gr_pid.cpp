#include "gr_pid.h"

GRPid::GRPid(float kp,float ti,float td,float ur,float un,float t)
{
	_para.kp = kp; _para.ti = ti;
	_para.td = td; _para.ur = ur;
	_para.un = un; _para.time = t;
	Uk=0,ekk=0,ekkk=0;
	update_data();
}

GRPid::GRPid(GRPid::Parameters p)
{
	_para = p;
}

void GRPid::update_data()
{
	k2=_para.kp*(float)(1+2*_para.td/_para.time);
	k3=_para.kp*_para.td/_para.time;
}

float GRPid::pidOut(float exp,float mea)
{
	float ek=mea-exp;
	
	if(((ek<_para.un)&&(ek>-_para.un))||_para.ti==0.0)                //积分不灵敏区
		k1=_para.kp*(float)(1+_para.td/_para.time);	
	else
		k1=_para.kp*(float)(1+_para.time/_para.ti+_para.td/_para.time);
	
	Uk=Uk+k1*ek-k2*(ekk)+(k3)*(ekkk);
	ekkk=ekk;
	ekk=ek;
	
	if(Uk>_para.ur) Uk =_para.ur;
	else if(Uk<-_para.ur)Uk=-_para.ur;
	
	return Uk;
}


