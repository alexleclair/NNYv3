package
{
	import flash.display.MovieClip;
	public class TopLevel //Will just be called to get clientInterface.root. So that it is easier to call it (and not as long!)
	{
		public static function get root():MovieClip
		{
			return ClientInterface.root;
		}
	}
}