using System;
using System.Collections.Generic;
using System.IO;
using Assembler;

public class Base
{
	public class Sym
	{
		public string name;
		public string filename;
		public int pos;
		public string ToString()
		{
			return name + "," + pos;
		}
	}

	public class Line
	{
		public enum LineType
		{
			cmd,// mov 指令
			sym,// _xxx: 符号
			dot,// .xxx 
			sharp// # 7 "d.c" 1
		}
		public enum DotType
		{
			codeSection,
			dataSection,
			other
		}

		public bool isCodeSeg()
		{
			if (dotType == DotType.codeSection)
			{
				return true;
			}
			return false;
		}
		public bool isDataSeg()
		{
			if (dotType == DotType.dataSection)
			{
				return true;
			}
			return false;
		}
		public LineType type;
		public DotType dotType;
		public string text;
		public string ToString()
		{
			return text;
		}
		public string sym;
		public string filename;
		public string cmd;
		public Op op1;
		public Op op2;
		public Op op3;
		public Op op4;
		public static Line match(string line, string filename)
		{
			Line l = new Line();
			l.filename = filename;
			l.text = line;
			if (line.EndsWith(":"))
			{
				l.type = LineType.sym;
				l.sym = line.Substring(0, line.Length - 1);
			}
			else if (line.StartsWith("."))
			{
				l.type = LineType.dot;
				if (line.StartsWith(".file"))
				{
					l.dotType = DotType.other;
				}
				else if (line.StartsWith(".section .text"))
				{
					l.dotType = DotType.codeSection;
				}
				else if (line == ".section .sdata" || line.StartsWith(".section .rodata") || line == ".section .data" || line.StartsWith(".section .sbss") || line == ".section .sdata,\"aws\",@progbits")
				{
					l.dotType = DotType.dataSection;
				}
				else if (line.StartsWith(".align") || line.StartsWith(".global") || line.StartsWith(".type") || line.StartsWith(".size") || line.StartsWith(".ident") || line.StartsWith(".weak"))
				{
					l.dotType = DotType.other;
				}
				else if (line.StartsWith(".long") || line.StartsWith(".string") || line.StartsWith(".comm") || line.StartsWith(".zero 4") || line.StartsWith(".zero 8"))
				{
					l.dotType = DotType.other;
				}
				else
				{
					throw new Exception("unknown dot type:" + line);
				}
			}
			else if (line.StartsWith("#"))
			{
				l.type = LineType.sharp;
			}
			else
			{
				string[] temparr;
				try
				{
					l.type = LineType.cmd;
					int pos1 = line.IndexOf(" ");
					if (pos1 == -1)
					{
						l.cmd = line;
						temparr = new string[0];
					}
					else
					{
						l.cmd = line.Substring(0, pos1);
						string temp = line.Substring(pos1);
						temparr = temp.Split(',');
					}
				}
				catch (Exception ex)
				{
					throw new Exception("err" + line);
				}
				if (temparr.Length == 4)
				{
					l.op1 = Op.match(temparr[0].Trim(), filename);
					l.op2 = Op.match(temparr[1].Trim(), filename);
					l.op3 = Op.match(temparr[2].Trim(), filename);
					l.op4 = Op.match(temparr[3].Trim(), filename);
				}
				else if (temparr.Length == 3)
				{
					l.op1 = Op.match(temparr[0].Trim(), filename);
					l.op2 = Op.match(temparr[1].Trim(), filename);
					l.op3 = Op.match(temparr[2].Trim(), filename);
				}
				else if (temparr.Length == 2)
				{
					l.op1 = Op.match(temparr[0].Trim(), filename);
					l.op2 = Op.match(temparr[1].Trim(), filename);
				}
				else if (temparr.Length == 1)
				{
					if (l.cmd == "call" || l.cmd == "jmpi")
					{
						string sop = temparr[0].Trim();
						Op op = new Op();
						op.text = sop;
						int ins;
						if (int.TryParse(sop, out ins))
						{
							op.type = Op.OpType.ins;
							op.ins = ins;
						}
						else
						{
							string temp2 = Util.matchSym(sop, out sop);
							if (sop.Length == 0)
							{
								op.type = Op.OpType.sym;
								op.sym = temp2;
							}
						}
						l.op1 = op;
					}
					else
					{
						l.op1 = Op.match(temparr[0].Trim(), filename);
					}
				}
				else if (temparr.Length == 0)
				{
				}
				else
				{
					throw new Exception("err temparr.Length" + line);
				}
			}
			return l;
		}
	}

	public class Op
	{
		public enum OpType
		{
			reg,// rax 寄存器
			ins,// 123 立即数
			sym,//_xxx 符号引用
			regins,//0(sp)
			gprel
		}
		public bool hi;
		public bool hiadj;
		public bool lo;
		public OpType type;
		public int? reg;
		public int? ins;
		public string sym;
		public string filename;
		public int? symAdj;
		public string getSymAdj()
		{
			if (symAdj == null)
			{
				return "";
			}
			if (symAdj.Value >= 0)
			{
				return "+" + symAdj.Value;
			}
			if (symAdj.Value < 0)
			{
				return "-" + symAdj.Value;
			}
			return "";
		}
		public string text;


		public static Op match(string sop, string filename)
		{
			Op op = new Op();
			op.text = sop;
			int tempreg = Util.getReg(sop, true);
			int itemp;
			if (tempreg >= 0)
			{
				op.type = OpType.reg;
				op.reg = tempreg;
				return op;
			}

			if (sop.StartsWith("0x"))
			{
				string temp = sop.Substring(2);
				itemp = Convert.ToInt32(temp, 16);
				op.type = OpType.ins;
				op.ins = itemp;
				return op;
			}

			if (int.TryParse(sop, out itemp))
			{
				op.type = OpType.ins;
				op.ins = itemp;
				return op;
			}

			string s2;
			int? val;
			string stemp = Util.matchNum(sop, out s2, out val);
			if (stemp.Length > 0)
			{
				op.ins = val;
				sop = s2;
				if (sop.StartsWith("(") && sop.EndsWith(")"))
				{
					op.reg = Util.getReg(sop.Substring(1, sop.Length - 2), false);
					op.type = OpType.regins;
					return op;
				}

			}

			if (sop.StartsWith("%gprel"))//%gprel(c)(gp)
			{
				op.type = OpType.gprel;
				sop = sop.Substring("%gprel".Length);
				if (sop.EndsWith("(gp)"))
				{
					sop = sop.Substring(0, (sop.Length - "(gp)".Length));
					setop(op, sop);
					return op;
				}
				else
				{
					throw new Exception();
				}
			}
			if (sop.StartsWith("%hiadj"))//%hiadj(c)
			{
				op.type = OpType.ins;
				op.hiadj = true;
				sop = sop.Substring("%hiadj".Length);
				setop(op, sop);
				return op;
			}
			if (sop.StartsWith("%lo"))//%lo(.LC1)
			{
				op.type = OpType.ins;
				op.lo = true;
				sop = sop.Substring("%lo".Length);
				setop(op, sop);
				return op;
			}
			if (sop.StartsWith("%hi"))//%hi(.LC1)
			{
				op.type = OpType.ins;
				op.hi = true;
				sop = sop.Substring("%hi".Length);
				setop(op, sop);
				return op;
			}
			string temp2 = Util.matchSym(sop, out sop);
			if (sop.Length == 0)
			{
				op.type = OpType.sym;
				op.sym = temp2;
				op.filename = filename;
				return op;
			}

			throw new Exception("unknown op");
		}

		private static void setop(Op op, string sop)
		{
			if (sop.StartsWith("(") && sop.EndsWith(")"))
			{
				sop = sop.Substring(1, sop.Length - 2);
				string s2;
				int? val;
				string stemp = Util.matchNum(sop, out s2, out val);
				if (stemp.Length > 0)
				{
					op.ins = val;
					if (s2.Length > 0)
					{
						throw new Exception();
					}
				}
				else
				{
					string stemp2 = Util.matchSym(sop, out s2);
					op.sym = stemp2;
					sop = s2;
					if (sop.Length > 0)
					{
						op.symAdj = int.Parse(sop);
					}
				}

			}
		}

	}



	public class Config
	{
		public string text;
		public string scmd;
		public string op1;
		public string op2;
		public string op3;
		public string op4;
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
			if (temparr.Length == 4)
			{
				op1 = temparr[0].Trim();
				op2 = temparr[1].Trim();
				op3 = temparr[2].Trim();
				op4 = temparr[3].Trim();
			}
			if (temparr.Length == 3)
			{
				op1 = temparr[0].Trim();
				op2 = temparr[1].Trim();
				op3 = temparr[2].Trim();
			}
			else if (temparr.Length == 2)
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

		public bool match(Line line)
		{
			if (this.scmd != line.cmd)
			{
				return false;
			}
			if (this.op1 != null)
			{
				if (line.op1 == null)
				{
					return false;
				}
				if (this.op1 != line.op1.type.ToString())
				{
					return false;
				}
			}
			else
			{
				if (line.op1 != null)
				{
					return false;
				}
			}

			if (this.op2 != null)
			{
				if (line.op2 == null)
				{
					return false;
				}
				if (this.op2 != line.op2.type.ToString())
				{
					return false;
				}
			}
			else
			{
				if (line.op2 != null)
				{
					return false;
				}
			}

			if (this.op3 != null)
			{
				if (line.op3 == null)
				{
					return false;
				}
				if (this.op3 != line.op3.type.ToString())
				{
					return false;
				}
			}
			else
			{
				if (line.op3 != null)
				{
					return false;
				}
			}

			if (this.op4 != null)
			{
				if (line.op4 == null)
				{
					return false;
				}
				if (this.op4 != line.op4.type.ToString())
				{
					return false;
				}
			}
			else
			{
				if (line.op4 != null)
				{
					return false;
				}
			}


			return true;
		}
		public string ToString()
		{
			return text;
		}
	}

	public class DataIns
	{
		public Line line;
	}
	public class Ins : DataIns
	{
		public string name;
		public Op op;
		public int textformat;//ins format 0:i 1:r 2:j
		public int format;//ins format 0:i 1:r 2:j

		public int bitcmd;
		public int bitregA;
		public int bitregB;
		public int bitregC;
		public int IMM16;//16/26bit
		public int IMM26;//16/26bit
		public int CMD3;
		public int IMM6;
		public int pos;
		public string ToString()
		{
			return line.ToString();
		}
	}
	public class Data : DataIns
	{
		public Data(Line line)
		{
			this.line = line;
		}
		public byte[] data;
		public string sym;
		public string filename;
		public int len;
		public string ToString()
		{
			return line.ToString() + "," + sym + "," + len;
		}
	}


	public static int procAdj(Op op, int val)
	{
		if (op.hiadj)
		{
			//%hiadj(immed32) Extract bits [31..16] and adds bit 15 of immed32 ((immed32 >> 16) & 0xFFFF) + ((immed32 >> 15) & 0x1)
			return ((val >> 16) & 0xFFFF) + ((val >> 15) & 0x1);
		}
		else if (op.hi)
		{
			//%hi(immed32) Extract bits [31..16] of immed32 (immed32 >> 16) & 0xFFFF
			return (val >> 16) & 0xFFFF;
		}
		else if (op.lo)
		{
			//%lo(immed32) Extract bits [15..0] of immed32 immed32 & 0xFFFF
			return val & 0xFFFF;
		}
		return val;
	}


	static string[] convt = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f" };
	public static string getHex4(int val, ref int sum)//"ll"+"hh"
	{
		sum += ((val >> 8) & 0x0FF) + ((val >> 0) & 0x0FF);
		return convt[((val >> 12) & 0x0F)] + convt[((val >> 8) & 0x0F)] + convt[((val >> 4) & 0x0F)] + convt[((val >> 0) & 0x0F)];
	}
	public static string getHex8(int val, ref int sum)//"ll"+"hh"
	{
		sum += ((val >> 24) & 0x0FF) + ((val >> 16) & 0x0FF) + ((val >> 8) & 0x0FF) + ((val >> 0) & 0x0FF);
		return convt[((val >> 28) & 0x0F)] + convt[((val >> 24) & 0x0F)] + convt[((val >> 20) & 0x0F)] + convt[((val >> 16) & 0x0F)] + convt[((val >> 12) & 0x0F)] + convt[((val >> 8) & 0x0F)] + convt[((val >> 4) & 0x0F)] + convt[((val >> 0) & 0x0F)];
	}
	public static string getHex8(int val)//"ll"+"hh"
	{
		return convt[((val >> 28) & 0x0F)] + convt[((val >> 24) & 0x0F)] + convt[((val >> 20) & 0x0F)] + convt[((val >> 16) & 0x0F)] + convt[((val >> 12) & 0x0F)] + convt[((val >> 8) & 0x0F)] + convt[((val >> 4) & 0x0F)] + convt[((val >> 0) & 0x0F)];
	}

	public static string getHex2(int val)
	{
		return convt[((val & 0xF0) >> 4)] + convt[((val & 0x0F))];
	}

}
