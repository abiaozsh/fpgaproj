using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
	public class Base : Form
	{
		public SerialPort port;

		public void connect(string portName)
		{
			if (port == null)
			{
				//COM4为Arduino使用的串口号，需根据实际情况调整  115200
				port = new SerialPort(portName, 2000000, Parity.None, 8, StopBits.One);
				port.Open();
			}
			else
			{
				port.Close();
				port = new SerialPort(portName, 2000000, Parity.None, 8, StopBits.One);
				port.Open();
			}
		}
		public byte[] readFromPort(int count)
		{
			if (count <= 0) return null;
			byte[] buff = new byte[count];
			int idx = 0;
			long t = DateTime.Now.Ticks;
			while (true)
			{
				if (port.BytesToRead > 0)
				{
					buff[idx] = (byte)port.ReadByte();
					idx++;
					count--;
					if (count <= 0) return buff;
				}
				var diff = DateTime.Now.Ticks - t;
				if (diff > 250000)
				{
					return buff;
					//throw new Exception();
				}
			}
		}
		public bool readFromPort(int count, byte[] buff, int idx)
		{
			if (count <= 0) return false;
			if (port == null) return false;
			int pos = 0;
			long t = DateTime.Now.Ticks;
			while (true)
			{
				int num = port.BytesToRead;
				if (num > 0)
				{
					if (num > count)
					{
						num = count;
						count = 0;
					}
					else
					{
						count -= num;
					}
					port.Read(buff, idx + pos, num);
					pos += num;
					if (count <= 0) return true;
				}
				var diff = DateTime.Now.Ticks - t;
				if (diff > 250000)
				{
					throw new Exception();
				}
			}
		}

		public void portWrite(byte val1, byte val2)
		{
			byte[] buff = new byte[2];
			buff[0] = val1;
			buff[1] = val2;
			if (port != null)
			{
				port.Write(buff, 0, 2);
			}
		}
		public void portWrite(byte val1)
		{
			byte[] buff = new byte[1];
			buff[0] = val1;
			if (port != null)
			{
				port.Write(buff, 0, 1);
			}
		}

		public void portWrite(byte val1, byte val2, Stream s)
		{
			s.WriteByte(val1);
			s.WriteByte(val2);

		}
		public void portWrite(byte val1, Stream s)
		{
			s.WriteByte(val1);
		}

		public void sendall(byte[] buff)
		{
			port.Write(buff, 0, buff.Length);
		}

		private void InitializeComponent()
		{
			this.SuspendLayout();
			// 
			// Base
			// 
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Name = "Base";
			this.Load += new System.EventHandler(this.Base_Load);
			this.ResumeLayout(false);

		}

		private void Base_Load(object sender, EventArgs e)
		{

		}





	}
}
