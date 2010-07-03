package dataTypes
{
	public class Word //Defining the "Word" data type.
	{
		private var m_val:uint;
		public function Short(val:uint)
		{
			m_val=val>=0?Math.floor(val):0;
		}
		public function valueOf()
		{
			return m_val;
		}
	}
}