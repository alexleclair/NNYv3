package dataTypes
{
	public class Short //Defining the "Short" data type.
	{
		private var m_val:int;
		public function Short(val:int)
		{
			m_val=val>=0?Math.floor(val):0;
		}
		public function valueOf()
		{
			return m_val;
		}
	}
}