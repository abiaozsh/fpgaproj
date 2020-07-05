using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace debugger
{
	public class CodeSym
	{
		public string name;
		public int pos;
	}
	public class DataSym
	{
		public string name;
		public int pos;
	}
	public class Config
	{
		public string text;
		public string scmd;
		public string op1;
		public string op2;
		public int textformat;
		public int insformat;
		public int cmd;
		public Config(string text)
		{
			this.text = text;
			string[] temparr;
			int pos1 = text.IndexOf(" ");
			if (pos1 == -1)
			{
				scmd = text;
				temparr = new string[0];
			}
			else
			{
				scmd = text.Substring(0, pos1);
				string temp = text.Substring(pos1);
				temparr = temp.Split(',');
			}
			if (temparr.Length == 2)
			{
				op1 = temparr[0].Trim();
				op2 = temparr[1].Trim();
			}
			else if (temparr.Length == 1)
			{
				op1 = temparr[0].Trim();
			}
		}
		public static List<Config> loadConfig(string path)
		{
			List<Config> cfgs = new List<Config>();
			{
				FileStream fs = new FileStream(path, FileMode.Open, FileAccess.Read);
				StreamReader sr = new StreamReader(fs);
				var temp = sr.ReadToEnd().Split('\n');
				foreach (var txt in temp)
				{
					var item = txt.Trim();
					if (item.Length == 0 || item.StartsWith("#"))
					{
						continue;
					}
					var arr = item.Split('@');
					Config cfg = new Config(arr[0].Trim());
					cfg.textformat = int.Parse(arr[1]);
					cfg.insformat = int.Parse(arr[2]);
					cfg.cmd = Convert.ToInt32(arr[3].Trim(), 2);// int.Parse(arr[3], );
					cfgs.Add(cfg);
				}

				fs.Close();
			}
			return cfgs;
		}

		private static string Reg(uint val)
		{
			if (val == 26)
			{
				return "gp";
			}
			else if (val == 27)
			{
				return "sp";
			}
			else if (val == 28)
			{
				return "fp";
			}
			else
			{
				return "r" + val;
			}
		}

		public static string dasm(List<CodeSym> syms, List<Config> cfgs, uint ins, int pos, int baseaddr)
		{
			StringBuilder sb = new StringBuilder();

			string hex = Util.getHex8(ins);
			string sIns = "";

			uint cmd = (ins & 0x1F);

			sIns = "";
			int ext = 0;
			int pc = 0;
			if (cmd == 0x1F)
			{
				pc = 1;
				cmd = ins >> 5 & 0x1F;
			}
			else if (cmd == 0x00)
			{
				ext = 1;
				cmd = (ins >> 5) & 0x1F;
			}

			if (ext == 1)
			{
				Config cfg = null;
				foreach (var item in cfgs)
				{
					if (cmd == item.cmd && item.insformat == 1)
					{
						cfg = item;
					}
				}
				if (cfg != null)
				{
					sIns = cfg.scmd + " ";
				}
				else
				{
					sIns = "unknow";
				}

				string A = Reg((ins >> (31 - 5)) & 0x1F);
				string B = Reg((ins >> (31 - 10)) & 0x1F);
				string C = Reg((ins >> (31 - 15)) & 0x1F);
				uint imm6 = (ins >> (10)) & 0x3F;
				sIns += "C:" + C + " <- (A:" + A + " ... B:" + B + ")" + " imm6:" + imm6;
			}
			else if (pc == 1)
			{
				Config cfg = null;
				foreach (var item in cfgs)
				{
					if (cmd == item.cmd && item.insformat == 3)
					{
						cfg = item;
					}
				}
				if (cfg != null)
				{
					sIns = cfg.ToString() + " ";
				}
				else
				{
					sIns = "unknow";
				}

				string A = Reg((ins >> (31 - 5)) & 0x1F);
				string B = Reg((ins >> (31 - 10)) & 0x1F);
				string C = Reg((ins >> (31 - 15)) & 0x1F);
				uint imm6 = (ins >> (10)) & 0x3F;
				uint imm26 = ((ins & 0x7FFFFFFF) >> (5));
				sIns += "C:" + C + " <- (A:" + A + " ... B:" + B + ") " + " imm6:" + imm6 + " imm26:" + imm26;
			}
			else
			{
				Config cfg = null;
				foreach (var item in cfgs)
				{
					if (cmd == item.cmd && (item.insformat == 0 || item.insformat == 2))
					{
						cfg = item;
					}
				}
				if (cfg != null)
				{
					sIns = cfg.scmd + " ";
				}
				else
				{
					sIns = "unknow";
				}
				string A = Reg((ins >> (31 - 5)) & 0x1F);
				string B = Reg((ins >> (31 - 10)) & 0x1F);
				uint IMM = (ins >> (5)) & 0xFFFF;
				sIns += "B:" + B + " A:" + A + " IMM:" + Util.getHex4((int)IMM);
			}
			sIns += " cmd:" + cmd;

			string sym = getSym(pos, syms);

			sb.Append("(" + sym + ") [" + hex + "]" + " : " + sIns);

			return sb.ToString();
		}

		public static string getSym(int pos, List<CodeSym> syms)
		{
			string foundsym = "";
			if (syms != null)
			{
				foreach (var sym in syms)
				{
					if (sym.pos == pos)
					{
						foundsym += sym.name + ",";
					}
				}
			}
			return foundsym;
		}

		public static string getReg(int? val)
		{
			switch (val)
			{
				case 0: return "eax";
				case 1: return "ebx";
				case 2: return "ecx";
				case 3: return "edx";
				case 4: return "ebp";
				case 5: return "esp";
				case 6: return "esi";
				case 7: return "edi";
				case 8: return "ra";
				case 9: return "rb";
			}
			return "reg(" + val + ")";
		}
		public static string getReg8(int? val)
		{
			switch (val)
			{
				case 0: return "al";
				case 1: return "bl";
				case 2: return "cl";
				case 3: return "dl";
				case 8: return "ralowbyte";
				case 9: return "rblowbyte";
			}
			return "reg8(" + val + ")";
		}

		public string ToString()
		{
			return text;
		}
	}

}
