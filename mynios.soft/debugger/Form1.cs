using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.IO;
using System.Globalization;
using debugger;
using SDcard;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace WindowsFormsApplication1
{

	public sealed partial class Form1 : Base
	{
		public Form1()
		{
			InitializeComponent();

			this.FormClosed += new FormClosedEventHandler(Form1_FormClosed);
		}

		void Form1_FormClosed(object sender, FormClosedEventArgs e)
		{
			if (port != null && port.IsOpen)
			{
				port.Close();
			}
		}
		string portName;

		private void Form1_Load(object sender, EventArgs e)
		{
			String[] list = SerialPort.GetPortNames();
			foreach (String s in list)
			{
				this.comboBox1.Items.Add(s);
			}
			this.comboBox2.SelectedIndex = 0;

			foreach (var item in this.Controls)
			{
				(item as Control).Enabled = false;
			}
			this.comboBox1.Enabled = true;
			this.button1.Enabled = true;
			this.button10.Enabled = true;
			this.textBox3.Enabled = true;
			this.button7.Enabled = true;
			this.button15.Enabled = true;
			this.button13.Enabled = true;
			loadSym();
		}
		private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
		{
			portName = (string)this.comboBox1.SelectedItem;
		}

		private void button1_Click(object sender, EventArgs e)
		{
			connect(portName);
			foreach (var item in this.Controls)
			{
				(item as Control).Enabled = true;
			}
		}

		List<CodeSym> syms;

		private void button2_Click(object sender, EventArgs e)
		{
			if (port != null && port.IsOpen)
			{
				port.Close();
				port = null;
			}
		}

		private void button3_Click(object sender, EventArgs e)
		{
			//test uart
			Random r = new Random();
			bool err = false;
			int count = 0;
			for (int i = 0; i < 100000; i++)
			{
				byte[] buff = new byte[1];
				r.NextBytes(buff);
				port.Write(buff, 0, 1);

				byte[] buff2 = readFromPort(1);

				if (buff[0] != buff2[0])
				{
					err = true;
					count = i;
					break;
				}
			}
			this.Text = "error:" + count + "," + err.ToString();
		}



		private void button4_Click(object sender, EventArgs e)
		{
			getstatus();
		}
		public string getCacheAddr(int id)
		{
			portWrite((byte)(0x2A), (byte)id);
			portWrite((byte)(0x6A), (byte)0);
			portWrite((byte)(0x6B), (byte)0);
			var temp = readFromPort(2);
			int val = temp[0] | (temp[1] << 8);
			return "   " + Util.getHex4(val);
		}
		public string getCacheLife(int id)
		{
			portWrite((byte)(0x29), (byte)id);
			portWrite((byte)(0x68), (byte)0);
			portWrite((byte)(0x69), (byte)0);
			var temp = readFromPort(2);
			int val = temp[0] | (temp[1] << 8);
			return String.Format("{0:000000} ", val);
		}
		//public int getCacheInfoVal(int id)
		//{
		//	portWrite((byte)(0x50 + id), (byte)0x00);
		//	portWrite((byte)(0x50 + id + 1), (byte)0x00);
		//	var temp = readFromPort(2);
		//	int val = temp[0] | (temp[1] << 8);
		//	return val;
		//}


		public void getstatus()
		{
			byte[] temp;
			portWrite((byte)(0x00), (byte)(0x00));
			int baseaddr = 0;


			portWrite((byte)(0x06), (byte)(123));
			temp = readFromPort(1);


			StringBuilder sb = new StringBuilder();
			sb.Append("pc:"); int pc = getreg(0x43, 0x00, sb);
			sb.Append("  private_offset:"); getreg(0x44, 0, sb);
			sb.Append("  pcResult:"); getreg(0x45, 0, sb);

			sb.Append("  last RtypeCmd:");
			portWrite((byte)(0x18), (byte)0x00); temp = readFromPort(1); sb.Append(Util.getHex2(temp[0]));

			int halt_cpu;
			sb.Append("  halt_cpu:");
			portWrite((byte)(0x14), (byte)0x00); temp = readFromPort(1); sb.Append(Util.getHex2(temp[0])); halt_cpu = temp[0];
			int halt_uart;
			sb.Append("  halt_uart:");
			portWrite((byte)(0x15), (byte)0x00); temp = readFromPort(1); sb.Append(Util.getHex2(temp[0])); halt_uart = temp[0];


			sb.Append("  waitRequest:");
			portWrite((byte)(0x16), (byte)0x00); temp = readFromPort(1); sb.Append(Util.getHex2(temp[0]));

			sb.Append("  irq_req:");
			portWrite((byte)(0x17), (byte)0x00); temp = readFromPort(1); sb.Append(Util.getHex2(temp[0]));

			sb.Append("  irq_addr:"); getreg(0x40, 0, sb);

			sb.Append("  debugin:");
			portWrite((byte)(0x64), (byte)0x00); temp = readFromPort(1); sb.Append(Util.getBin8(temp[0]));
			sb.Append("  debugin32:");
			getData(0x60, sb);

			sb.AppendLine();

			sb.Append("  access time:");
			portWrite((byte)(0x4), (byte)0x00);
			portWrite((byte)(0x5), (byte)0x00);
			temp = readFromPort(2);
			sb.Append((temp[0] + (temp[1] << 8)));
			sb.AppendLine();

			sb.Append("cache_life: ");
			for (int i = 0; i < 16; i++)
			{
				sb.Append(" " + getCacheLife(i));
			}
			sb.AppendLine();

			sb.Append("cache_addr:");
			for (int i = 0; i < 16; i++)
			{
				sb.Append(" " + getCacheAddr(i));
			}
			sb.AppendLine();

			//sb.Append("  numer:"); getreg(0x40, 0, sb);
			//sb.Append("  denom:"); getreg(0x41, 0, sb);
			//sb.Append("  quotient:"); getreg(0x42, 0, sb);
			//sb.Append("  remain:"); getreg(0x45, 0, sb);


			sb.AppendLine();

			if (true && (halt_cpu != 0 || halt_uart != 0))
			{

				sb.Append("r0:");
				getreg(0x47, 0x00, sb);
				sb.Append(" r1:");
				getreg(0x47, 0x01, sb);

				sb.Append("   r2:");
				getreg(0x47, 0x02, sb);
				sb.Append(" r3:");
				getreg(0x47, 0x03, sb);
				sb.AppendLine();

				for (int i = 4; i < 8; i++)
				{
					sb.Append(" r" + i + ":");
					getreg(0x47, (byte)i, sb);
				}
				sb.AppendLine();

				for (int i = 8; i < 16; i++)
				{
					sb.Append(" r" + i + ":");
					getreg(0x47, (byte)i, sb);
				}
				sb.AppendLine();

				for (int i = 16; i < 24; i++)
				{
					sb.Append(" r" + i + ":");
					getreg(0x47, (byte)i, sb);
				}
				sb.AppendLine();

				sb.Append("sp:");
				int sp = getreg(0x47, 27, sb);

				sb.Append("    fp:");
				getreg(0x47, 28, sb);
				sb.AppendLine();

				sb.Append(" re:");
				getreg(0x47, 29, sb);

				sb.Append(" ra:");
				getreg(0x47, 31, sb);
				sb.AppendLine();


				sb.AppendLine("stack: ");
				baseaddr = sp;
				for (int i = -Convert.ToInt32(this.comboBox2.Text) * 4; i < Convert.ToInt32(this.comboBox2.Text) * 4; i += 4)
				{
					sb.Append((i == 0 ? "*" : " ") + Util.getHex8((uint)(i + baseaddr)) + ":"); getmem(i + baseaddr, sb); sb.AppendLine();
				}

				baseaddr = 0;
				sb.AppendLine("code: ");
				for (int i = -32; i < 32; i += 4)
				{
					StringBuilder sb2 = new StringBuilder();
					uint code = getmem(pc + i, sb2);
					int target = pc + i;
					string foundsym = Config.getSym(target, syms);
					string scode = Config.dasm(syms, cfgs, (uint)code, pc + i, baseaddr);
					if (i == 0)
					{
						sb.AppendLine("----------------------------------------------------------------------------------");
					}
					sb.AppendLine((i == 0 ? "*" : " ") + Util.getHex8((uint)(pc + i)) + ":" + scode);
					if (i == 0)
					{
						sb.AppendLine("----------------------------------------------------------------------------------");
					}
				}

				sb.AppendLine("mem: ");
				baseaddr = Convert.ToInt32(textBox1.Text, 16);
				for (int i = 0; i < Convert.ToInt32(this.comboBox2.Text) * 4; i += 4)
				{
					sb.Append("" + Util.getHex8((uint)(i + baseaddr)) + ":"); getmem(i + baseaddr, sb); sb.AppendLine();
				}
			}

			this.textBox4.Text = sb.ToString();
			Application.DoEvents();
		}

		private void textBox4_TextChanged(object sender, EventArgs e)
		{

		}
		public int getreg(byte cmd, byte reg, StringBuilder sb)
		{
			portWrite(cmd, reg);
			byte[] temp;
			int val = 0;
			portWrite((byte)(0x13), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 24; sb.Append(Util.getHex2(temp[0]));
			portWrite((byte)(0x12), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 16; sb.Append(Util.getHex2(temp[0]));
			portWrite((byte)(0x11), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 8; sb.Append(Util.getHex2(temp[0]));
			portWrite((byte)(0x10), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 0; sb.Append(Util.getHex2(temp[0]));
			return val;
		}
		public int getData(int baseCmd, StringBuilder sb)
		{
			byte[] temp;
			int val = 0;
			portWrite((byte)(baseCmd + 0x3), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 24; sb.Append(Util.getHex2(temp[0]));
			portWrite((byte)(baseCmd + 0x2), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 16; sb.Append(Util.getHex2(temp[0]));
			portWrite((byte)(baseCmd + 0x1), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 8; sb.Append(Util.getHex2(temp[0]));
			portWrite((byte)(baseCmd + 0x0), (byte)0x00); temp = readFromPort(1); val |= temp[0] << 0; sb.Append(Util.getHex2(temp[0]));
			return val;
		}

		public uint getmem(int addr, StringBuilder sb)
		{
			byte a0 = (byte)((addr >> 0) & 0xFF);
			byte a1 = (byte)((addr >> 8) & 0xFF);
			byte a2 = (byte)((addr >> 16) & 0xFF);
			byte a3 = (byte)((addr >> 24) & 0xFF);

			if (buff_a0 == null || buff_a0 != a0) { buff_a0 = a0; portWrite((byte)(0x20), buff_a0.Value); }
			if (buff_a1 == null || buff_a1 != a1) { buff_a1 = a1; portWrite((byte)(0x21), buff_a1.Value); }
			if (buff_a2 == null || buff_a2 != a2) { buff_a2 = a2; portWrite((byte)(0x22), buff_a2.Value); }
			if (buff_a3 == null || buff_a3 != a3) { buff_a3 = a3; portWrite((byte)(0x23), buff_a3.Value); }

			portWrite((byte)(0x30), 0);
			byte[] temp;
			temp = readFromPort(1);
			if (temp[0] != 123)
			{
				//throw new Exception("err");
			}
			uint val = 0;
			portWrite((byte)(0x13), (byte)0x00);
			portWrite((byte)(0x12), (byte)0x00);
			portWrite((byte)(0x11), (byte)0x00);
			portWrite((byte)(0x10), (byte)0x00);
			temp = readFromPort(4);
			val |= (uint)(temp[0]) << 24;
			val |= (uint)(temp[1]) << 16;
			val |= (uint)(temp[2]) << 8;
			val |= (uint)(temp[3]) << 0;
			if (sb != null)
			{
				sb.Append(Util.getHex2(temp[0]));
				sb.Append(Util.getHex2(temp[1]));
				sb.Append(Util.getHex2(temp[2]));
				sb.Append(Util.getHex2(temp[3]));
			}
			return val;
		}

		byte? buff_a0;
		byte? buff_a1;
		byte? buff_a2;
		byte? buff_a3;

		byte? buff_d0;
		byte? buff_d1;
		byte? buff_d2;
		byte? buff_d3;

		public bool setmem(uint addr, uint data, bool noerr = false)
		{
			byte a0 = (byte)((addr >> 0) & 0xFF);
			byte a1 = (byte)((addr >> 8) & 0xFF);
			byte a2 = (byte)((addr >> 16) & 0xFF);
			byte a3 = (byte)((addr >> 24) & 0xFF);

			byte d0 = (byte)((data >> 0) & 0xFF);
			byte d1 = (byte)((data >> 8) & 0xFF);
			byte d2 = (byte)((data >> 16) & 0xFF);
			byte d3 = (byte)((data >> 24) & 0xFF);

			if (buff_a0 == null || buff_a0 != a0) { buff_a0 = a0; portWrite((byte)(0x20), buff_a0.Value); }
			if (buff_a1 == null || buff_a1 != a1) { buff_a1 = a1; portWrite((byte)(0x21), buff_a1.Value); }
			if (buff_a2 == null || buff_a2 != a2) { buff_a2 = a2; portWrite((byte)(0x22), buff_a2.Value); }
			if (buff_a3 == null || buff_a3 != a3) { buff_a3 = a3; portWrite((byte)(0x23), buff_a3.Value); }

			if (buff_d0 == null || buff_d0 != d0) { buff_d0 = d0; portWrite((byte)(0x24), buff_d0.Value); }
			if (buff_d1 == null || buff_d1 != d1) { buff_d1 = d1; portWrite((byte)(0x25), buff_d1.Value); }
			if (buff_d2 == null || buff_d2 != d2) { buff_d2 = d2; portWrite((byte)(0x26), buff_d2.Value); }
			if (buff_d3 == null || buff_d3 != d3) { buff_d3 = d3; portWrite((byte)(0x27), buff_d3.Value); }

			portWrite((byte)(0x31), 0);
			byte[] temp;
			temp = readFromPort(1);
			if (temp[0] != 123)
			{
				if (!noerr)
				{
					throw new Exception("err");
				}
				else
				{
					return false;
				}
			}
			return true;
		}

		private void button2_Click_1(object sender, EventArgs e)
		{
			portWrite((byte)(0x00), (byte)(0x00));

			clearBuff();

			int addr = Convert.ToInt32(textBox1.Text, 16);
			StringBuilder sb = new StringBuilder();
			getmem(addr, sb);
			textBox2.Text = sb.ToString();
			//getstatus();
		}
		private void button6_Click(object sender, EventArgs e)
		{
			portWrite((byte)(0x00), (byte)(0x00));

			clearBuff();

			int byteEnable = 0;
			byteEnable |= this.checkBox4.Checked ? 8 : 0;
			byteEnable |= this.checkBox5.Checked ? 4 : 0;
			byteEnable |= this.checkBox6.Checked ? 2 : 0;
			byteEnable |= this.checkBox7.Checked ? 1 : 0;

			portWrite((byte)(0x28), (byte)byteEnable);

			uint addr = Convert.ToUInt32(textBox1.Text, 16);
			setmem(addr, Convert.ToUInt32(textBox2.Text, 16));
			//getstatus();
		}


		private void checkBox1_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox1.Checked)
			{
				portWrite((byte)(0x01), 1);
			}
			else
			{
				portWrite((byte)(0x01), 0);
			}

		}

		private void button3_Click_1(object sender, EventArgs e)
		{
			portWrite((byte)(0x03), 0);
			getstatus();
		}

		private void clearBuff()
		{
			buff_a0 = null;
			buff_a1 = null;
			buff_a2 = null;
			buff_a3 = null;

			buff_d0 = null;
			buff_d1 = null;
			buff_d2 = null;
			buff_d3 = null;
			portWrite((byte)(0x00), 0);
			readFromPort(10);
		}
		private void button5_Click(object sender, EventArgs e)
		{
			clearBuff();
		}
		private void button12_Click(object sender, EventArgs e)
		{
			for (int i = 0; i < 256; i++)
			{
				getmem(0x00001000, null);
				getmem(0x00002000, null);
				getmem(0x00003000, null);
				getmem(0x00004000, null);
			}
		}

		private void button13_Click(object sender, EventArgs e)
		{
            String[] files = Directory.GetFiles(@"E:\fpgaproj\mine.new\", "*.bmp");

			foreach (var item in files)
			{
				Bitmap b = new Bitmap(item);

				FileStream fs = new FileStream(item.Replace(".bmp",".img"), FileMode.CreateNew, FileAccess.Write);
				for (int j = 0; j < b.Height; j++)
				{
                    for (int i = 0; i < b.Width; i++)
					{
						uint val = getpixel(b.GetPixel(i, j));
						fs.WriteByte((byte)val);
						fs.WriteByte((byte)(val >> 8));
					}
				}
				fs.Close();
			}
		}
		private uint getpixel(Color c)
		{
			uint val = 0;
			val += ((uint)(c.R) >> 3) << (5 + 6);
			val += ((uint)(c.G) >> 2) << (5);
			val += ((uint)(c.B) >> 3);
			return val;
		}

		private void setpixel(int x, int y)
		{
			if ((x & 1) == 0)
			{
				setmem((uint)(0x00200000 + x * 2 + y * 2048), 0x0000FFFF);
			}
			else
			{
				setmem((uint)(0x00200000 + x * 2 + y * 2048), (uint)(0x0000FFFF) << 16);
			}
		}
		private void clearpixel(int x, int y)
		{
			if ((x & 1) == 0)
			{
				setmem((uint)(0x00200000 + x * 2 + y * 2048), 0x00000000);
			}
			else
			{
				setmem((uint)(0x00200000 + x * 2 + y * 2048), 0x00000000);
			}
		}

		uint[] data;

		private void button7_Click(object sender, EventArgs e)
		{
			clearBuff();
			int baseAddr = 0;
			for (int i = 0; i < data.Length; i += 64)
			{
				bool error = false;
				string sretry = "";
				for (int retry = 0; retry < 5; retry++)
				{
					error = false;
					//:04001600 1005003a 97
					for (int j = i; j < i + 64 && j < data.Length; j++)
					{
						uint b = getmem(baseAddr + j * 4, null);
						if (data[j] != b)
						{
							error = true;
							break;
						}
					}
					if (!error)
					{
						break;
					}
					else
					{
						portWrite((byte)(0x00), 0);
						readFromPort(10);
						sretry = "retry";
					}
				}
				if (error)
				{
					MessageBox.Show("err");
					break;
				}
				this.Text = (i * 4).ToString() + sretry;
				Application.DoEvents();
			}


			/*

		/*
			portWrite((byte)(0x01), 1);


			//portWrite((byte)(0x16), (byte)0x00); var temp = readFromPort(1);
			//textBox2.Text += temp[0];
			//portWrite((byte)(0x17), (byte)0x00); temp = readFromPort(1);
			//textBox2.Text += temp[0];
			Bitmap b2 = new Bitmap(1024, 768);
			if (true)
			{
				Bitmap b = new Bitmap("e:\\test.bmp");
				Graphics g = Graphics.FromImage(b2);
				g.DrawImage(b, new Rectangle(0, 0, 1024, 768), 0, 0, b.Width, b.Height, GraphicsUnit.Pixel, null);
				g.Dispose();
			}
			else
			{
				//按照屏幕宽高创建位图
				//从一个继承自Image类的对象中创建Graphics对象
				Graphics gc = Graphics.FromImage(b2);
				//抓屏并拷贝到myimage里
				gc.CopyFromScreen(new Point(0, 0), new Point(0, 0), new Size(1024, 768));
				gc.Flush();
				gc.Dispose();
			}

			if (false)
			{

				draw(b2);

				for (int j = 0; j < 768; j++)
				{
					for (int i = 0; i < 1024; i += 2)
					{
						uint v1 = getpixel(b2.GetPixel(i, j));
						uint v2 = getpixel(b2.GetPixel(i + 1, j));
						setmem((uint)(0x00200000 + i * 2 + j * 2048), (v2 << 16) | v1);//
					}
					this.Text = "" + j;
				}

				//				setmem(0x02040008, 1);
				//				for (int j = 0; j < 20; j++)
				//				{
				//					for (int i = 0; i < 1024; i++)
				//					{
				//						uint v1 = getpixel(b2.GetPixel(i, j));
				//						//setmem((uint)(0x00200000 + i * 2 + j * 2048), (v2 << 16) | v1);//
				//						setmem((uint)(0x80000000 + (i + j * 1024) * 4), v1);
				//						setmem((uint)(0x80000000 + (i + j * 1024) * 4), v1);
				//
				//					}
				//					this.Text = "" + j;
				//				}
				//				setmem(0x02040008, 0);

			}
			else
			{
				try
				{
					BitmapData bmpData = b2.LockBits(new Rectangle(0, 0, 1024, 768), System.Drawing.Imaging.ImageLockMode.ReadOnly, System.Drawing.Imaging.PixelFormat.Format16bppRgb565);
					IntPtr ptr = bmpData.Scan0;

					byte[] buff = new byte[768 * 2 * 1024];
					Marshal.Copy(ptr, buff, 0, 768 * 2 * 1024);

					FileStream fs = new FileStream("e:\\test.img", FileMode.Create, FileAccess.Write);
					for (int y = 0; y < buff.Length; y++)
					{
						fs.WriteByte(buff[y]);
					}
					fs.Flush();
					fs.Close();

					b2.UnlockBits(bmpData);
				}
				catch (Exception ex)
				{
					ex.ToString();
				}
			}
		 */

		}
		List<Config> cfgs;

		private void loadSym()
		{
			cfgs = Config.loadConfig(@"assembler\config.txt");

			syms = new List<CodeSym>();
			FileStream fs = new FileStream("temp.sym", FileMode.Open, FileAccess.Read);
			StreamReader sw = new StreamReader(fs);
			string s = sw.ReadToEnd();
			sw.Close();
			fs.Close();
			foreach (var line in s.Split('\n'))
			{
				if (line != "")
				{
					CodeSym sym = new CodeSym();
					sym.pos = int.Parse(line.Split(',')[0]);
					sym.name = line.Split(',')[1];
					syms.Add(sym);
				}
			}
		}


		private void button8_Click(object sender, EventArgs e)
		{
			portWrite((byte)(0x00), (byte)(0x00));

			clearBuff();

			loadSym();

			int baseAddr = 0x02000000;

			portWrite((byte)(0x01), 1);//halt_uart

			loadprog(baseAddr);

			//      end else if (command == 8'h02) begin debug_reset_n<=data[0]; command_done<=1;

			portWrite((byte)(0x02), 0);
			portWrite((byte)(0x02), 1);
			if (!checkBox1.Checked)
			{
				portWrite((byte)(0x01), 0);
			}

		}

		private void button9_Click(object sender, EventArgs e)
		{
			portWrite((byte)(0x00), (byte)(0x00));

			portWrite((byte)(0x02), 0);

			Thread.Sleep(100);

			portWrite((byte)(0x02), 1);

		}

		private void button10_Click(object sender, EventArgs e)
		{
			/*
struct bpb_t {//biosParmBlock
  char bytesPerSector_0;
  char bytesPerSector_1;
  char sectorsPerCluster;
  char reservedSectorCount_0;
  char reservedSectorCount_1;
  char fatCount; 
  char rootDirEntryCount_0;
  char rootDirEntryCount_1;
  char totalSectors16_0;
  char totalSectors16_1;

  char mediaType;

  char sectorsPerFat16_0;
  char sectorsPerFat16_1;
  char sectorsPerTrtack_0;
  char sectorsPerTrtack_1;
  char headCount_0;
  char headCount_1;
  char hidddenSectors_0;
  char hidddenSectors_1;
  char hidddenSectors_2;
  char hidddenSectors_3;

  char totalSectors32_0;
  char totalSectors32_1;
  char totalSectors32_2;
  char totalSectors32_3;
  char sectorsPerFat32_0;
  char sectorsPerFat32_1;
  char sectorsPerFat32_2;
  char sectorsPerFat32_3;
  char fat32Flags_0;
  char fat32Flags_1;

  char fat32Version_0;
  char fat32Version_1;
  char fat32RootCluster_0;
  char fat32RootCluster_1;
  char fat32RootCluster_2;
  char fat32RootCluster_3;
  char fat32FSInfo_0;
  char fat32FSInfo_1;
  char fat32BackBootBlock_0;
  char fat32BackBootBlock_1;

  char fat32Reserved[12];
};41+12

struct fbs_t {//fat32BootSector
  char jmpToBootCode[3]; 3
  char oemName[8]; 8 
  bpb_t bpb; 41+12

  char driveNumber;
  char reserved1;
  char bootSignature;
  char volumeSerialNumber_0;
  char volumeSerialNumber_1;
  char volumeSerialNumber_2;
  char volumeSerialNumber_3;  7

  char volumeLabel[11]; 11 
  char fileSystemType[8]; 8
  char bootCode[420];
  char bootSectorSig0;
  char bootSectorSig1;
};
struct dir_t {//directoryEntry
  char name[11];
  char attributes;
  char reservedNT;
  char creationTimeTenths;
  char creationTime_0;
  char creationTime_1;

  char creationDate_0;
  char creationDate_1;
  char lastAccessDate_0;
  char lastAccessDate_1;
  char firstClusterHigh_0; 20
  char firstClusterHigh_1;
  char lastWriteTime_0;
  char lastWriteTime_1;
  char lastWriteDate_0;
  char lastWriteDate_1;
  char firstClusterLow_0; 26
  char firstClusterLow_1;
  char fileSize_0;
  char fileSize_1;
  char fileSize_2;
  char fileSize_3;
};

			 */
			SDUtils sd = new SDUtils(this.textBox3.Text + ":");
			byte[] fbs = sd.ReadSector(0);
			int volumeStartBlock = 0;
			int reservedSectorCount = fbs[3 + 8 + 3 + 0] | (fbs[3 + 8 + 3 + 1] << 8);
			int fatStartBlock = volumeStartBlock + reservedSectorCount;

			int fatCount = fbs[3 + 8 + 5];
			int blocksPerFat = fbs[3 + 8 + 11 + 0] | (fbs[3 + 8 + 11 + 1] << 8);
			int rootDirStart = fatStartBlock + fatCount * blocksPerFat;

			int rootDirEntryCount = fbs[3 + 8 + 6 + 0] | (fbs[3 + 8 + 6 + 1] << 8);

			int dataStartBlock = rootDirStart + (((rootDirEntryCount << 5) + 511) >> 9);


			byte[] root = sd.ReadSector(rootDirStart);
			string name = "BOOT    BIN";
			//string name = "TEST1   TXT";
			int index = -1;
			for (int i = 0; i < 512; i += 32)
			{
				int match = 1;
				for (int j = 0; j < 11; j++)
				{
					if (root[i + j] != (byte)name[j])
					{
						match = 0;
						break;
					}
				}
				if (match == 1)
				{
					index = i;
				}
			}

			int firstCluster;
			int fileSize;
			if (index >= 0)
			{
				firstCluster = root[index + 26 + 0] | (root[index + 26 + 1] << 8) | (root[index + 20 + 0] << 16) | (root[index + 20 + 1] << 24);
				fileSize = root[index + 28 + 0] | (root[index + 28 + 1] << 8) | (root[index + 28 + 2] << 16) | (root[index + 28 + 3] << 24);
			}
			else
			{
				throw new Exception("not found boot.bin");
			}

			short[] fat = new short[blocksPerFat * 256];
			for (int i = 0; i < blocksPerFat; i++)
			{
				byte[] fattemp = sd.ReadSector(fatStartBlock + i);
				for (int j = 0; j < 256; j++)
				{
					fat[i * 256 + j] = (short)(fattemp[j * 2] + (fattemp[j * 2 + 1] << 8));
				}
			}

			int blocks = 0;
			int start = firstCluster;
			while (true)
			{
				int oldStart = start;
				start = fat[start];
				blocks++;
				if (start == -1)
				{
					break;
				}
				if (start != oldStart + 1)
				{
					throw new Exception("not continue block");
				}

			}

			int sectorsPerCluster = fbs[3 + 8 + 2];

			int blockOfCluster = 0;
			int firstBlock = dataStartBlock + (firstCluster - 2) * sectorsPerCluster + blockOfCluster;

			byte[] data = sd.ReadSector(firstBlock);


			int bootrec = 3 + 8 + 41 + 12 + 7 + 11 + 8;
			fbs[bootrec + 0] = 0x12;
			fbs[bootrec + 1] = 0x34;
			fbs[bootrec + 2] = 0x56;
			fbs[bootrec + 3] = 0x78;

			fbs[bootrec + 4] = (byte)((firstBlock >> 0) & 0xFF);
			fbs[bootrec + 5] = (byte)((firstBlock >> 8) & 0xFF);
			fbs[bootrec + 6] = (byte)((firstBlock >> 16) & 0xFF);
			fbs[bootrec + 7] = (byte)((firstBlock >> 24) & 0xFF);

			fbs[bootrec + 8] = (byte)((blocks >> 0) & 0xFF);
			fbs[bootrec + 9] = (byte)((blocks >> 8) & 0xFF);
			fbs[bootrec + 10] = (byte)((blocks >> 16) & 0xFF);
			fbs[bootrec + 11] = (byte)((blocks >> 24) & 0xFF);

			sd.WriteSector(fbs, 0);
			MessageBox.Show("ok");
			sd.Close();
		}

		private void button11_Click(object sender, EventArgs e)
		{
			portWrite((byte)(0x00), (byte)(0x00));

			clearBuff();

			loadSym();

			int baseAddr = 0x02000000;

			portWrite((byte)(0x01), 1);//halt_uart

			{
				FileStream fs = new FileStream("debugins.hex", FileMode.Open, FileAccess.Read);
				StreamReader sr = new StreamReader(fs);
				String s = sr.ReadToEnd();
				sr.Close();
				fs.Close();
				string[] data = s.Split('\n');
				setmem(0x02000000, Convert.ToUInt32(data[0].Substring(9, 8), 16));
				setmem(0x02000004, Convert.ToUInt32(data[1].Substring(9, 8), 16));
				setmem(0x02000008, Convert.ToUInt32(data[2].Substring(9, 8), 16));
			}

			baseAddr = 0x00000000;

			loadprog(baseAddr);

			//      end else if (command == 8'h02) begin debug_reset_n<=data[0]; command_done<=1;

			portWrite((byte)(0x02), 0);
			portWrite((byte)(0x02), 1);

			if (!checkBox1.Checked)
			{
				portWrite((byte)(0x01), 0);
			}
		}
		public void loadprog(int baseAddr)
		{
			setmem((uint)(0x02010000), 0, true);//reset cache

			FileStream fs = new FileStream("out.hex", FileMode.Open, FileAccess.Read);
			StreamReader sr = new StreamReader(fs);
			String s = sr.ReadToEnd();
			sr.Close();
			fs.Close();

			int index = 0;
			uint[] data = new uint[s.Split('\n').Length];
			foreach (var item in s.Split('\n'))
			{
				if (item.Length == ":040016001005003a97".Length + 1)
				{
					//:04001600 1005003a 97
					String str = item.Substring(9, 8);
					data[index] = Convert.ToUInt32(str, 16);
					index++;
				}
			}
			for (int i = 0; i < data.Length; i += 64)
			{
				bool error = false;
				string sretry = "";
				for (int retry = 0; retry < 5; retry++)
				{
					error = false;
					//:04001600 1005003a 97
					for (int j = i; j < i + 64 && j < data.Length; j++)
					{
						bool result = setmem((uint)(baseAddr + j * 4), data[j], true);
						if (!result)
						{
							error = true;
							break;
						}
					}
					for (int j = i; j < i + 64 && j < data.Length; j++)
					{
						uint b = getmem(baseAddr + j * 4, null);
						if (data[j] != b)
						{
							error = true;
							break;
						}
					}
					if (!error)
					{
						break;
					}
					else
					{
						portWrite((byte)(0x00), 0);
						readFromPort(10);
						sretry = "retry";
					}
				}
				if (error)
				{
					MessageBox.Show("err");
					break;
				}
				this.Text = (i * 4).ToString() + sretry;
				Application.DoEvents();

			}


		}

		private void trackBar1_Scroll(object sender, EventArgs e)
		{
			setmem(0x02040004, (uint)(trackBar1.Value));
		}

		private void checkBox2_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox2.Checked)
			{
				setmem(0x02040008, 1);
			}
			else
			{
				setmem(0x02040008, 0);
			}

		}

		private void checkBox3_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox3.Checked)
			{
				setmem(0x02040000, 0);
			}
			else
			{
				setmem(0x02040000, 2);
			}

		}

		private void button15_Click(object sender, EventArgs e)
		{
			data = new uint[64 * 1024];
			Random r = new Random();
			for (int i = 0; i < data.Length; i++)
			{
				data[i] = (uint)r.Next();
			}

		}

		private void button14_Click(object sender, EventArgs e)
		{
			int baseAddr = 0;
			for (int i = 0; i < data.Length; i += 64)
			{
				bool error = false;
				string sretry = "";
				for (int retry = 0; retry < 5; retry++)
				{
					error = false;
					//:04001600 1005003a 97
					for (int j = i; j < i + 64 && j < data.Length; j++)
					{
						bool result = setmem((uint)(baseAddr + j * 4), data[j], true);
						if (!result)
						{
							error = true;
							break;
						}
					}
					for (int j = i; j < i + 64 && j < data.Length; j++)
					{
						uint b = getmem(baseAddr + j * 4, null);
						if (data[j] != b)
						{
							error = true;
							break;
						}
					}
					if (!error)
					{
						break;
					}
					else
					{
						portWrite((byte)(0x00), 0);
						readFromPort(10);
						sretry = "retry";
					}
				}
				if (error)
				{
					MessageBox.Show("err");
					break;
				}
				this.Text = (i * 4).ToString() + sretry;
				Application.DoEvents();
			}
		}

		private void button16_Click(object sender, EventArgs e)
		{
			portWrite((byte)(0x01), 1);


			//portWrite((byte)(0x16), (byte)0x00); var temp = readFromPort(1);
			//textBox2.Text += temp[0];
			//portWrite((byte)(0x17), (byte)0x00); temp = readFromPort(1);
			//textBox2.Text += temp[0];
			Bitmap b2 = new Bitmap(1024, 768);
			if (true)
			{
				Bitmap b = new Bitmap("d:\\test.bmp");
				Graphics g = Graphics.FromImage(b2);
				g.DrawImage(b, new Rectangle(0, 0, 1024, 768), 0, 0, b.Width, b.Height, GraphicsUnit.Pixel, null);
				g.Dispose();
			}
			else
			{
				//按照屏幕宽高创建位图
				//从一个继承自Image类的对象中创建Graphics对象
				Graphics gc = Graphics.FromImage(b2);
				//抓屏并拷贝到myimage里
				gc.CopyFromScreen(new Point(0, 0), new Point(0, 0), new Size(1024, 768));
				gc.Flush();
				gc.Dispose();
			}
			draw(b2);

			//setmem(0x02040008, 1);
			for (int j = 0; j < 768; j++)
			{
				for (int i = 0; i < 1024; i++)
				{
					uint v1 = getpixel(b2.GetPixel(i, j));
					//setmem((uint)(0x00200000 + i * 2 + j * 2048), (v2 << 16) | v1);//
					setmem((uint)(0x80000000 + (i + j * 1024) * 4), v1);
					//setmem((uint)(0x80000000 + (i + j * 1024) * 4), v1);

				}
				this.Text = "" + j;
			}
			//setmem(0x02040008, 0);

		}

		void draw(Bitmap b2)
		{

			//setpixel(10, 256);
			//setpixel(256, 10);
			//setpixel(10, 255);
			//setpixel(255, 10);
			for (int j = 0; j < 768; j++)
			{
				//if ((j & 1) == 0)
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF);
				//}
				//else
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF << 16);
				//}
				b2.SetPixel(j, j, Color.White);
			}
			for (int j = 0; j < 768; j++)
			{
				//if ((j & 1) == 0)
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF);
				//}
				//else
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF << 16);
				//}
				b2.SetPixel(j + 30, j, Color.White);
			}

			for (int j = 30; j < 768; j++)
			{
				//if ((j & 1) == 0)
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF);
				//}
				//else
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF << 16);
				//}
				b2.SetPixel(j - 30, j, Color.White);
			}

			for (int j = 0; j < 768; j++)
			{
				//if ((j & 1) == 0)
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF);
				//}
				//else
				//{
				//	setmem(0x00200000 + j * 2 + j * 2048, 0x0000FFFF << 16);
				//}
				b2.SetPixel(j + 250, j, Color.White);
			}

			for (int j = 0; j < 50; j++)
			{
				b2.SetPixel(254, j, Color.White);
				b2.SetPixel(255, j, Color.Black);
				b2.SetPixel(256, j, Color.Red);
				b2.SetPixel(257, j, Color.Green);
				b2.SetPixel(258, j, Color.Blue);
			}

			for (int j = 100; j < 150; j++)
			{
				b2.SetPixel(254, j, Color.White);
				b2.SetPixel(255, j, Color.Black);
				b2.SetPixel(256, j, Color.White);
				b2.SetPixel(257, j, Color.Black);
				b2.SetPixel(258, j, Color.White);
			}
		}

		private void button17_Click(object sender, EventArgs e)
		{
			portWrite((byte)(0x01), 1);


			//setmem(0x02040008, 1);
			for (int j = 0; j < 768; j++)
			{
				for (int i = 0; i < 1024; i++)
				{
					//setmem((uint)(0x00200000 + i * 2 + j * 2048), (v2 << 16) | v1);//
					setmem((uint)(0x80000000 + (i + j * 1024) * 4), 0);
					//setmem((uint)(0x80000000 + (i + j * 1024) * 4), v1);

				}
				this.Text = "" + j;
			}
			//setmem(0x02040008, 0);

		}

	}
}
