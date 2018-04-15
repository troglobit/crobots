int
	ang,
	oang,
	range,
	orange,   /* Variabili di puntamento                              */
	d,dmax,   /* Variabili temporanee salva danni subiti              */
	dir,      /* Direzione del cammino                                */
	posx,     /* Variabili ad 1 bit                                   */
	posy,     /* salva posizione                                      */
	dist,     /* Distanza dal corner                                  */
	flag,     /* Bit-oriented flag per la gestione del movimento      */
	timer,    /* Timer per il conteggio dei nemici                    */
	enemies,  /* Conta nemici superstiti                              */
  t,b,      /* Timer oscillazione                                   */
  i,td,tl,  /* Variabili di utilità                                 */
  deg1,     /* Limite inferiore di scansione per contare i nemici   */
  deg2;     /* Limite superiore di scansione per contare i nemici   */

stop()
{
  fire2(drive(dir,t=0));  
  while(speed()>59) ;
}

fastradar() /* Conta nemici fast */
{
  enemies=3
  -(scan(deg1,    10)!=0)
  -(scan(deg1+20, 10)!=0)
  -(scan(deg1+40, 10)!=0)
  -(scan(deg1+60, 10)!=0)
  -(scan(deg1+80, 10)!=0)
  -(scan(deg1+100,10)!=0)
  ;
}

runX()
{
  dir=180*!posx;
  if(posx) { while(loc_x()<910) fire3(dir,100); while(loc_x()<950) drive(dir,100); }
  else     { while(loc_x()> 90) fire3(dir,100); while(loc_x()> 50) drive(dir,100); }
  stop();
}

runY()
{
  dir=270-180*posy;
  if(posy) { while(loc_y()<910) fire3(dir,100); while(loc_y()<950) drive(dir,100); }
  else     { while(loc_y()> 90) fire3(dir,100); while(loc_y()> 50) drive(dir,100); }  
  stop();
}

corner()
{
  if (speed()) stop(); if (flag^posx^posy) runY(runX()); else runX(runY());
}

fire(dir) /* Many thanks to Daniele Nuzzo */
{
  int asin,acos;
  /*if (safe) return;*/
  if (speed()<100) drive(dir,100); 
      
  if (scan(ang,10)>150)
  {  
      asin=(sin(ang-dir)/14384);
      acos=(cos(ang-dir)/3796)-230;

      ang-=18*(scan(ang-18,10)>0);  
      ang+=18*(scan(ang+18,10)>0); 

      if(scan(ang-16,10)) ang-=8;
      else if(scan(ang+16,10)) ang+=8;
      if(scan(ang-12,10)) ang-=4;
      else if(scan(ang+12,10)) ang+=4;
      if(scan(ang-11,10)) ang-=2;
      if(scan(ang+11,10)) ang+=2;

      if (orange=scan(oang=ang,3))
        {
            if(scan(ang-13,10)) ang-=5;
            else if(scan(ang+13,10)) ang+=5; 
            if(scan(ang+12,10)) ang+=4;
            else if(scan(ang-12,10)) ang-=4;
            if(scan(ang-11,10)) ang-=2;
            if(scan(ang+11,10)) ang+=2;

            cannon(ang+(ang-oang)*((880+(range=scan(ang,10)))/482)-asin,
                   range*230/(orange-range-acos)); 
        }  else search(); 
  } else search();  
}

search()
{
  if (scan(ang-=350,10)) return fire2();
  if (scan(ang-=20,10))  return fire2();
  if (scan(ang-=320,10)) return fire2();
  if (scan(ang-=60,10))  return fire2();
  if (scan(ang-=280,10)) return fire2();
  if (scan(ang-=100,10)) return fire2();
  if (scan(ang-=240,10)) return fire2();
  if (scan(ang-=140,10)) return fire2();
  if (scan(ang-=200,10)) return fire2();
  if (scan(ang-=180,10)) return fire2();
  if (scan(ang-=160,10)) return fire2();
  if (scan(ang-=220,10)) return fire2();
  if (scan(ang-=120,10)) return fire2();
  if (scan(ang-=260,10)) return fire2();
  if (scan(ang-=80,10))  return fire2();
  if (scan(ang-=300,10)) return fire2();
  if (scan(ang-=40,10))  return fire2();
  if (scan(ang-=340,10)) return fire2();
}

fire2()
{
  if (range=scan(oang=ang,10)) {
    /*if (range>500) return cannon(ang,range);*/
    if (scan(ang+350,10)) ang+=355; else ang+=5;
    if (scan(ang+350,10)) ang+=357; else ang+=3; 
    cannon((ang<<1)-oang,(scan(ang,10)<<1)-range);
  } else search();   
}

fire3(dir,v)
{
  drive(dir,v);
	if (range=scan(oang=ang,10)) 
	{
		if (scan(ang-8,5))  
		{ 	
			if (scan(ang-=5,2)) ; 
			else ang-=4; 
		}
		else
		{
			if (scan(ang+8,5))  
			{
				if (scan(ang+=5,2)) ; 
				else ang+=4; 
			}
      else
      {
				if (scan(ang,1)) ;
				else if (scan(ang-=3,2)) ; else ang+=6;
			}
		}
		return(cannon((ang<<1)-oang,(scan(ang,10)<<1)-range)); 
	} 
	else 
	{
		if(range=scan(ang+=20,10)) cannon(ang,range);
		else if(range=scan(ang-=40,10)) cannon(ang,range);
		else ang+=80;
	}
}

int look(a)
{
       if (scan(a,10))    ;
  else if (scan(a+=20,10));
  else return 0;
  ang=a;
  return 1;
}

main() /* Inizializza alcune variabili ed innesca la routine principale */
int abra,bula,cadabra;
{
  corner(posx=loc_x(posy=(loc_y(tl=td=32)>499))>499);
  fastradar(deg2=(deg1=(90*((posy<<1)+(posx^posy))))+100);
  while (enemies<1) /* fase 1 */
  {
    dir=deg1+(30<<flag);
    if (look(deg1+(70*flag)))
    {
      fire(dir);
      while(scan(ang,10)>840) fire2();
      corner();
    }
    else if (scan(dir,10))
    {
      fire(ang=dir);
      while(scan(ang,10)>660) fire(dir);
      corner();
    }
    else
    {
      while(++t<4) fire(dir);
      corner(tl=timer);
    }
    if ((d=damage()-d)>dmax)
    {
      dmax=d; flag^=1;
    }
    if ((++timer>tl)&&(range>400))
  	{
  	    if ((timer>115)&&(damage()<60)) enemies=1;
  	    else fastradar(tl=timer+(td>>=1)+1);
    }
  }
  while(enemies<=1) /* fase 2 */
  {
    if (look(deg1))
    {
      dir=deg1+(30<<flag);
      if (look(deg1+(70*flag)))
      {
        fire(dir);
        while(scan(ang,10)>680) fire(dir);
        corner();
        flag^=1;
      }
      else if (scan(dir,10))
      {
        fire(ang=dir);
        while(scan(ang,10)>640) fire(dir);
        corner();
        flag^=1;
      }
      else
      {
        while(++t<4) fire(dir);
        corner();
        if ((d=damage()-d)>dmax)
        {
          dmax=d; flag^=1;
        }
      }
      if ((++timer%2)&&(range>400))
    	{
  	    if ((timer>125)&&(damage()<40)) enemies=2;
  	    else {
	        int deg, brange; deg=deg1-20; brange=1500; enemies=3;
	        while((deg<=deg2)&& enemies)
    		    if (range=scan(deg+=20, 10))
    		    {
    		      --enemies;
    		      if (range<brange)
    		      {
          			ang=deg;
          			brange=range;
    		      }
    		    }
    		} 
      }
    }
    else
    {
      dir=deg1; abra=(posy<<1)|(posx^posy); t=0;
      while(t<4)
      {
        if (abra&1) bula=loc_y();     else bula=loc_x();
        if (abra&2) cadabra=bula>180; else cadabra=bula<820;
        
        if (cadabra) fire(dir);
        else
        {
          fire3(dir+=90,0); if (scan(dir,10)) ang=dir; ++abra; timer+=++t;
        }
      }
      fastradar(t=0);
    }    
  }
	while(1)
  {
		if (((posx=loc_x())%880)<120) dir=180*(posx>500);
		else if (((posy=loc_y())%880)<120) dir=90+180*(posy>500);
		else if (range>600) dir=ang+25;
		else if (range<150) dir=ang+195;
		else dir=ang+180*(b^=1);
	
		fire3(dir,100);
		fire3(dir,100);
		fire3(dir,100);         
  }
}

