public class myclass()
{
	
	def static myfun(x:boolean,y:int):void
	{
		var p,c,t:boolean;
		var a,b:int;
		a = 1;
		b = y;
		t = x;
		
		
		c = a < b;
		println(c);
		
		c = a > b;
		println(c);		
		
		c = a <= b;
		println(c);
			
		c = a >= b;
		println(c);
			
		if(a < b)
		{
			println(95);
		}
		else
		{
			println(96);
		}
		
		if(a > b)
		{
			println(95);
		}
		else
		{
			println(96);
		}
		
		
		if(a <= b)
		{
			println(95);
		}
		else
		{
			println(96);
		}
		
		if(a >= b)
		{
			println(95);
		}
		else
		{
			println(96);
		}
		
		if(3 >= 4)
		{
			println(95);
		}
		else
		{
			println(96);
		}	
		
				
		if(3!=4 && 3 <= 4 && (4==4 || 6<=9) && a<=b && a!=b && t )
		{
			println(95);
		}
		else
		{
			println(96);
		}
		
		if(a!=6 )
		{
			println(95);
		}
		else
		{
			println(96);
		}
	return;  
	}
	
	def static main():void
	{	
		myfun(true,2);
		return;
	}
};
