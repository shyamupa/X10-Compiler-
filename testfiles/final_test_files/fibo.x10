public class myclass()
{
	
	def static myfibo(x:int):int
	{
		var a:int;
		if(x==1 || x==2)
		{
		return 1;
		}
		else
		{
		return myfibo(x-2)+myfibo(x-1);
		}
	}
	
	def static main():void
	{	
		var result,i:int;
		for(i=1;i<25;i=i+1)
		{
			result=myfibo(i);
			println(result);
		}
		return ;
	}
};

