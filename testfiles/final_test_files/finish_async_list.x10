public class myclass()
{
	
	def static myfun():void
	{
		var i:int;
		for(i=0;i<100;i++)
		{
		println(1);		
		}		
		return;
	}
	def static yourfun():void
	{
		var i:int;
		for(i=0;i<100;i++)
		{
		println(2);		
		}		
		return;
	}
	
	def static main():void
	{	
		var i:int;
		async{ yourfun();} ;
		finish { async{ myfun(); }; };
		for(i=0;i<1000;i++)
		{
		println(6);		
		}		

		return;
	}
};

