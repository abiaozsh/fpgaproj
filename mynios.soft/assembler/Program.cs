using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.IO;
using System.Globalization;
using System.Threading;
using System.Text.RegularExpressions;

namespace Assembler
{
	class Program : Base
	{

		static void Main(string[] args)
		{
			string filein;
			string fileout;
			string filetemp;
			string test = "e";
			string type = "dos";
			string fileoutBin;
			if (args.Length > 0 && !String.IsNullOrEmpty(args[0]))
			{
				filein = args[0];
			}
			else
			{
				filein = test + ".s,inc\\irq.asm";
			}
			if (args.Length > 1 && !String.IsNullOrEmpty(args[1]))
			{
				fileout = args[1];
			}
			else
			{
				fileout = test + ".hex";
			}
			if (args.Length > 2 && !String.IsNullOrEmpty(args[2]))
			{
				filetemp = args[2];
			}
			else
			{
				filetemp = test + ".temp.s";
			}
			if (args.Length > 3 && !String.IsNullOrEmpty(args[3]))
			{
				type = args[3];
			}
			if (args.Length > 4 && !String.IsNullOrEmpty(args[4]))
			{
				fileoutBin = args[4];
			}
			else
			{
				fileoutBin = test + ".bin";
			}
			Console.WriteLine(filein);
			Console.WriteLine(fileout);
			Console.WriteLine(filetemp);

			try
			{
				compile(filein, fileout, fileoutBin, filetemp, type);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex);
				Console.ReadLine();
			}
		}

		public class RawLine
		{
			public string line;
			public string filename;
		}

		public static void compile(string filein, string fileout, string fileoutBin, string filetemp, string type)
		{
			List<Config> cfgs = Config.loadConfig(@"assembler\config.txt");
			List<RawLine> linesraw = new List<RawLine>();
			foreach (string f in filein.Split(','))
			{
				FileStream fs = new FileStream(f, FileMode.Open, FileAccess.Read);
				StreamReader sr = new StreamReader(fs);
				var temp = sr.ReadToEnd();
				foreach (var item in temp.Split('\n'))
				{
					RawLine rl = new RawLine();
					rl.filename = f;
					rl.line = item;
					linesraw.Add(rl);
				}
				fs.Close();
			}

			int basePos;

			List<Line> lines = new List<Line>();

			lines.Add(Line.match(".section .text", null));
			if (type == "bios")
			{
				Console.WriteLine("type=bios");
				int sp = 0x02000000;//0x0200C000
				basePos = 0x02000000;
				lines.Add(Line.match("movhi sp, %hiadj(" + sp + ")", null));
				lines.Add(Line.match("addi sp, sp, %lo(" + sp + ")", null));
				lines.Add(Line.match("jmpi main", null));
			}
			else if (type == "dos")
			{
				Console.WriteLine("type=dos");
				int sp = 0x02000000;//0x0200C000
				basePos = 0x00000000;
				lines.Add(Line.match("movhi sp, %hiadj(" + sp + ")", null));
				lines.Add(Line.match("addi sp, sp, %lo(" + sp + ")", null));
				lines.Add(Line.match("jmpi main", null));
			}
			else if (type == "app")
			{
				Console.WriteLine("type=app");
				uint h = 0x80000000;
				basePos = (int)(h);
				lines.Add(Line.match("call main", null));
				lines.Add(Line.match("ret", null));
			}
			else if (type == "debug")
			{
				Console.WriteLine("type=debug");
				basePos = 0x00000000;
			}
			else
			{
				throw new Exception();
			}

			foreach (var lineraw in linesraw)
			{
				string raw = lineraw.line;
				if (raw.Trim().Length > 0)
				{
					string temp = raw.Trim();
					if (temp.StartsWith(".string"))
					{
						temp = temp.Substring(".string ".Length);
						temp = ".string " + temp;
					}
					else
					{
						temp = temp.Replace('\t', ' ');
						while (true)
						{
							if (temp.Contains("  "))
							{
								temp = temp.Replace("  ", " ");
							}
							else
							{
								break;
							}
						}
					}

					Line line = Line.match(temp, lineraw.filename);
					if (line == null)
					{
						throw new Exception("unknow line:" + raw);
					}
					lines.Add(line);
				}
			}
			lines.Add(Line.match(".section .text", null));
			lines.Add(Line.match("_Z7__eof__v:", null));
			lines.Add(Line.match("movhi r2, %hiadj(eof)", null));
			lines.Add(Line.match("addi r2, r2, %lo(eof)", null));
			lines.Add(Line.match("ret", null));
			lines.Add(Line.match(".section .data", null));
			lines.Add(Line.match("eof:", null));
			lines.Add(Line.match(".zero 4", null));

			List<Line> linespass2 = new List<Line>();
			foreach (var line in lines)
			{

				//bgtu rA, rB, label bltu rB, rA, label

				//bleu rA, rB, label bgeu rB, rA, label
				//cmpgt rC, rA, rB cmplt rC, rB, rA
				//cmpgti rB, rA, IMMED cmpgei rB, rA, (IMMED+1)
				//cmpgtu rC, rA, rB cmpltu rC, rB, rA
				//cmpgtui rB, rA, IMMED cmpgeui rB, rA, (IMMED+1)
				//cmple rC, rA, rB cmpge rC, rB, rA
				//cmplei rB, rA, IMMED cmplti rB, rA, (IMMED+1)
				//cmpleu rC, rA, rB cmpgeu rC, rB, rA
				//cmpleui rB, rA, IMMED cmpltui rB, rA, (IMMED+1)

				//movia rB, label orhi rB, r0, %hiadj(label)
				//addi, rB, r0, %lo(label)

				//subi rB, rA, IMMED addi rB, rA, (-IMMED)

				if ((new Config("nop")).match(line))//nop add r0, r0, r0
				{
					linespass2.Add(Line.match("add r0, r0, r0", line.filename));
				}
				//br sym                       @          20 @                      0 @   6 @          000110      0x06
				else if ((new Config("br sym")).match(line))//ble rA, rB, label bge rB, rA, label
				{
					linespass2.Add(Line.match("beq r0, r0, " + line.op1.text, line.filename));
				}
				else if ((new Config("ble reg, reg, sym")).match(line))//ble rA, rB, label bge rB, rA, label
				{
					linespass2.Add(Line.match("bge " + line.op2.text + ", " + line.op1.text + ", " + line.op3.text, line.filename));
				}
				else if ((new Config("bgt reg, reg, sym")).match(line))//bgt rA, rB, label blt rB, rA, label
				{
					linespass2.Add(Line.match("blt " + line.op2.text + ", " + line.op1.text + ", " + line.op3.text, line.filename));
				}
				else if ((new Config("movi reg, ins")).match(line))//movi rB, IMMED addi, rB, r0, IMMED
				{
					linespass2.Add(Line.match("addi " + line.op1.text + ", r0, " + line.op2.text, line.filename));
				}
				else if ((new Config("movui reg, ins")).match(line))//movui rB, IMMED ori rB, r0, IMMED
				{
					linespass2.Add(Line.match("ori " + line.op1.text + ", r0, " + line.op2.text, line.filename));
				}
				else if ((new Config("mov reg, reg")).match(line))//mov rC, rA add rC, rA, r0
				{
					linespass2.Add(Line.match("add " + line.op1.text + ", " + line.op2.text + ", r0", line.filename));
				}
				else if ((new Config("movhi reg, ins")).match(line))//movhi rB, IMMED orhi rB, r0, IMMED
				{
					linespass2.Add(Line.match("orhi " + line.op1.text + ", r0, " + line.op2.text, line.filename));
				}
				else if ((new Config("stw reg, gprel")).match(line))//stw r2, %gprel(Sd2Card_status_)(gp)
				{
					//%gprel(immed32) Replace the immed32 address with an offset from the global pointer immed32 –_gp
					//movhi	r1, %hiadj(c)
					linespass2.Add(Line.match("orhi r1, r0, %hiadj(" + line.op2.sym + line.op2.getSymAdj() + ")", line.filename));
					//addi	r1, r1, %lo(c)
					linespass2.Add(Line.match("addi r1, r1, %lo(" + line.op2.sym + line.op2.getSymAdj() + ")", line.filename));
					//ldw	r4, 0(r1)
					linespass2.Add(Line.match("stw " + line.op1.text + ", 0(r1)", line.filename));
				}
				else if ((new Config("ldw reg, gprel")).match(line))//ldw r4, %gprel(c)(gp)
				{
					//%gprel(immed32) Replace the immed32 address with an offset from the global pointer immed32 –_gp
					//movhi	r1, %hiadj(c)
					linespass2.Add(Line.match("orhi r1, r0, %hiadj(" + line.op2.sym + line.op2.getSymAdj() + ")", line.filename));
					//addi	r1, r1, %lo(c)
					linespass2.Add(Line.match("addi r1, r1, %lo(" + line.op2.sym + line.op2.getSymAdj() + ")", line.filename));
					//ldw	r4, 0(r1)
					linespass2.Add(Line.match("ldw " + line.op1.text + ", 0(r1)", line.filename));
				}
				else if ((new Config("ldwio reg, regins")).match(line))//ldw r4, %gprel(c)(gp)
				{
					//ldw	r4, 0(r1)
					linespass2.Add(Line.match("ldw " + line.op1.text + ", " + line.op2.text, line.filename));
				}
				else if ((new Config("stwio reg, regins")).match(line))//ldw r4, %gprel(c)(gp)
				{
					//ldw	r4, 0(r1)
					linespass2.Add(Line.match("stw " + line.op1.text + ", " + line.op2.text, line.filename));
				}
				else if ((new Config("jmpi sym")).match(line))//jmpi
				{
					//movhi	r1, %hiadj(c)
					linespass2.Add(Line.match("orhi r1, r0, %hiadj(" + line.op1.sym + ")", line.filename));
					//addi	r1, r1, %lo(c)
					linespass2.Add(Line.match("addi r1, r1, %lo(" + line.op1.sym + ")", line.filename));
					//ldw	r4, 0(r1)
					linespass2.Add(Line.match("jmp r1", line.filename));
				}
				else if ((new Config("jmpi ins")).match(line))//jmpi
				{
					//movhi	r1, %hiadj(c)
					linespass2.Add(Line.match("orhi r1, r0, %hiadj(" + line.op1.ins + ")", line.filename));
					//addi	r1, r1, %lo(c)
					linespass2.Add(Line.match("addi r1, r1, %lo(" + line.op1.ins + ")", line.filename));
					//ldw	r4, 0(r1)
					linespass2.Add(Line.match("jmp r1", line.filename));
				}

				else
				{
					linespass2.Add(line);
				}
			}
			lines = linespass2;


			int pos;

			List<Sym> syms = new List<Sym>();

			List<DataIns> dataInsList = new List<DataIns>();

			pos = basePos;
			{
				foreach (var item in lines)
				{
					var line = item;
					if (line.type == Line.LineType.sym)
					{
						Sym sym = new Sym();
						sym.name = line.sym;
						sym.filename = line.filename;
						sym.pos = pos;
						syms.Add(sym);
					}
					else if (line.type == Line.LineType.cmd)
					{
						addCmd(cfgs, line, ref pos, dataInsList);
					}
					else
					{
						addData(line, ref pos, syms, dataInsList);
					}
				}
			}

			foreach (var item in dataInsList)
			{
				if (item is Ins)
				{
					var ins = item as Ins;
					if (ins.op != null)
					{
						if (ins.op.sym != null)
						{
							Sym found = null;
							foreach (var sym in syms)
							{
								if (ins.op.sym == sym.name && ins.op.filename == sym.filename)
								{
									found = sym; break;
								}
							}
							if (found == null)
							{
								foreach (var sym in syms)
								{
									if (ins.op.sym == sym.name)
									{
										found = sym; break;
									}
								}
							}
							if (found == null)
							{
								throw new Exception("not found:" + ins.op.sym);
							}
							if (ins.format == 0)
							{
								if (ins.textformat == 15 || ins.textformat == 20)
								{
									ins.IMM16 = found.pos - ins.pos - 4;
									if (ins.IMM16 > 32768 || ins.IMM16 < -30000)
									{
										throw new Exception();
									}
								}
								else if (ins.textformat == 10)
								{
									int temppos = found.pos;
									if (ins.op.symAdj != null)
									{
										temppos += ins.op.symAdj.Value;
									}
									ins.IMM16 = procAdj(ins.op, temppos);
									if (ins.IMM16 > 32768 || ins.IMM16 < -30000)
									{
										throw new Exception();
									}
								}
								else
								{
									throw new Exception();
								}
							}
							else if (ins.format == 2)
							{
								if (found.pos > 35000000)
								{
									throw new Exception();
								}
								ins.IMM26 = found.pos >> 2;
							}
						}
						else
						{
							ins.IMM16 = procAdj(ins.op, ins.op.ins.Value);
							if (ins.IMM16 > 70000 || ins.IMM16 < -70000)
							{
								throw new Exception();
							}
						}
					}
				}

				if (item is Data)
				{
					var data = item as Data;
					if (data.sym != null)
					{
						Sym found = null;
						int val = 0;
						foreach (var sym in syms)
						{
							if (data.sym == sym.name && data.filename == sym.filename)
							{
								found = sym;
								val = found.pos;
								break;
							}
						}
						if (found == null)
						{
							foreach (var sym in syms)
							{
								if (data.sym == sym.name)
								{
									found = sym;
									val = found.pos;
									break;
								}
							}
						}
						if (found == null)
						{
							throw new Exception("not found:" + data.sym);
						}
						byte[] src = new byte[4];
						src[3] = (byte)((val >> 24) & 0xFF);
						src[2] = (byte)((val >> 16) & 0xFF);
						src[1] = (byte)((val >> 8) & 0xFF);//高8位
						src[0] = (byte)(val & 0xFF);//低位
						data.data = src;
					}
				}
			}

			{
				FileStream fs2 = new FileStream("temp.sym", FileMode.Create, FileAccess.Write);
				StreamWriter sw = new StreamWriter(fs2);
				foreach (var sym in syms)
				{
					sw.WriteLine(sym.pos + "," + sym.name);
				}
				sw.Flush();
				fs2.Close();
			}

			{
				int len = pos;
				FileStream fs = new FileStream(fileoutBin, FileMode.Create, FileAccess.Write);
				FileStream fs2 = new FileStream(fileout, FileMode.Create, FileAccess.Write);
				FileStream fs3 = new FileStream(filetemp, FileMode.Create, FileAccess.Write);

				BinaryWriter bw = new BinaryWriter(fs);
				StreamWriter sw = new StreamWriter(fs2);
				StreamWriter sw3 = new StreamWriter(fs3);
				//bw.Write(len);
				//bw.Write(entry);

				int posx = 0;
				foreach (var item in dataInsList)
				{
					string ssym = "";
					foreach (var sym in syms)
					{
						if (((posx) * 4) == sym.pos)
						{
							ssym += sym.name + ",";
						}
					}

					if (item is Ins)
					{
						var ins = item as Ins;
						//[31]1bit:4 byte/8 byte
						//[30:24]7bit:cmd
						//[23:20]4bit:reg1
						//[19:16]4bit:reg2
						//[15:0]16bit:ins1
						//[31:0]32bit:ins2
						int insbin = 0;
						if (ins.format == 0)//andi reg, reg, ins
						{
							insbin = (ins.bitregA << (31 - 5)) | (ins.bitregB << (31 - 10)) | ((ins.IMM16 & 0x0000FFFF) << (5)) | (ins.bitcmd);
						}
						else if (ins.format == 1)
						{
							insbin = (ins.bitregA << (31 - 5)) | (ins.bitregB << (31 - 10)) | (ins.bitregC << (31 - 15)) | (ins.IMM6 << (5 + 5)) | (ins.bitcmd << (5)) | 0;
						}
						else if (ins.format == 2)
						{
							insbin = (ins.IMM26 << 5) | (ins.bitcmd);
						}
						else if (ins.format == 3)
						{
							insbin = (ins.bitregA << (31 - 5)) | (ins.bitregB << (31 - 10)) | (ins.bitregC << (31 - 15)) | (ins.IMM6 << (5 + 5)) | (ins.bitcmd << (5)) | 31;
						}

						int paritycheck = 0;
						for (int i = 0; i < 31; i++)
						{
							paritycheck ^= (insbin >> i) & 1;
						}

						insbin |= (~paritycheck) << 31;

						bw.Write(insbin);
						writehex(insbin, posx++, sw);

						sw3.WriteLine(getHex8((posx - 1) * 4) + " [" + getHex8(insbin) + "] (" + ssym + ") " + ins.line.text);

					}

					if (item is Data)
					{
						var data = item as Data;
						for (int i = 0; i < data.len; i += 4)
						{
							int val = 0;
							if (i + 0 < data.data.Length)
							{
								val |= (data.data[i + 0]);
							}
							if (i + 1 < data.data.Length)
							{
								val |= (data.data[i + 1] << 8);
							}
							if (i + 2 < data.data.Length)
							{
								val |= (data.data[i + 2] << 16);
							}
							if (i + 3 < data.data.Length)
							{
								val |= (data.data[i + 3] << 24);
							}
							bw.Write(val);
							writehex(val, posx++, sw);
							sw3.WriteLine(getHex8((posx - 1) * 4) + " [" + getHex8(val) + "] (" + ssym + ")");
						}
					}
				}

				Console.WriteLine("size:" + posx * 4);
				sw.WriteLine(":00000001FF");

				sw.Flush();
				fs2.Close();

				sw3.Flush();
				fs3.Close();

				bw.Flush();
				fs.Close();

			}
			//Console.ReadLine();
		}

		public static void addCmd(List<Config> cfgs, Line line, ref int pos, List<DataIns> dataInsList)
		{
			Ins ins = new Ins();
			Config matchCfg = null;
			foreach (var cfg in cfgs)
			{
				if (cfg.match(line))
				{
					matchCfg = cfg;
					break;
				}
			}
			if (matchCfg == null)
			{
				throw new Exception("unknow cmd:" + line.text);
			}
			else
			{
				try
				{
					switch (matchCfg.textformat)
					{
						case 10://andhi rB, rA, IMM16
							ins.bitregB = line.op1.reg.Value;
							ins.bitregA = line.op2.reg.Value;
							ins.op = line.op3;
							break;
						case 11://stw rB, byte_offset(rA)
							ins.bitregB = line.op1.reg.Value;
							ins.bitregA = line.op2.reg.Value;
							ins.IMM16 = line.op2.ins.Value;
							break;
						case 15://bne rA, rB, label
							ins.bitregA = line.op1.reg.Value;
							ins.bitregB = line.op2.reg.Value;
							ins.op = line.op3;
							break;
						case 22://call sym
							ins.op = line.op1;
							break;
						case 30://add rC, rA, rB
							ins.bitregC = line.op1.reg.Value;
							ins.bitregA = line.op2.reg.Value;
							ins.bitregB = line.op3.reg.Value;
							break;
						case 31://slli reg, reg, ins
							ins.bitregC = line.op1.reg.Value;
							ins.bitregA = line.op2.reg.Value;
							ins.IMM6 = line.op3.ins.Value;
							break;

						case 50://callr reg
							ins.bitregC = 31;
							ins.bitregA = line.op1.reg.Value;
							ins.CMD3 = 0;
							break;

						case 51://jmp reg
							ins.bitregA = line.op1.reg.Value;
							ins.CMD3 = 1;
							break;

						case 52://setirq reg, reg, reg, ins   @ addr, flg, result, isSetAddr
							ins.bitregA = line.op1.reg.Value;
							ins.bitregB = line.op2.reg.Value;
							ins.bitregC = line.op3.reg.Value;
							ins.CMD3 = 2;
							ins.IMM6 = line.op4.ins.Value;
							break;

						case 53://stoff reg
							ins.bitregA = line.op1.reg.Value;
							ins.CMD3 = 3;
							break;

						case 54://hlt ins
							ins.IMM6 = line.op1.ins.Value;
							ins.CMD3 = 4;
							break;

						case 55://ret
							ins.bitregA = 31;
							ins.CMD3 = 5;
							break;

						case 56://reti
							ins.bitregA = 29;
							ins.CMD3 = 6;
							break;

						case 57://irqcall
							ins.bitregC = 31;
							ins.CMD3 = 7;
							break;

						default:
							throw new Exception("unknown text format:" + matchCfg.textformat);
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.ToString());
					throw new Exception("error:" + line.text);
				}
				ins.line = line;
				ins.textformat = matchCfg.textformat;
				ins.format = matchCfg.insformat;
				ins.bitcmd = matchCfg.cmd;
				ins.pos = pos;
				dataInsList.Add(ins);
				pos += 4;
			}


		}

		public static void addData(Line line, ref int pos, List<Sym> syms, List<DataIns> dataInsList)
		{

			//.comm	_gbuff, 64	 # 51
			//.comm	_tempstr, 16	 # 4
			if (line.text.Trim().StartsWith(".comm "))
			{
				string txt = line.text.Trim().Substring(".comm ".Length);
				string[] arr = txt.Split(',');
				Sym dataSym = new Sym();
				dataSym.name = arr[0];
				dataSym.filename = line.filename;
				dataSym.pos = pos;
				syms.Add(dataSym);

				int len = int.Parse(arr[1]);
				Data data = new Data(line);
				data.data = new byte[len];
				data.len = len;
				dataInsList.Add(data);
				pos += len;
			}
			else
			{
				if (line.text.StartsWith(".string \""))
				{
					Data data = new Data(line);
					string txt = line.text.Substring(".string \"".Length, line.text.Length - 1 - ".string \"".Length) + "\0";
					txt = System.Text.RegularExpressions.Regex.Unescape(txt);
					data.data = System.Text.Encoding.Default.GetBytes(txt);
					dataInsList.Add(data);
					data.len = Util.align4(data.data.Length);
					pos += data.len;
				}
				if (line.text.StartsWith(".long"))
				{
					Data data = new Data(line);
					string txt = line.text.Substring(".long ".Length);
					int val;
					if (int.TryParse(txt, out val))
					{
						byte[] src = new byte[4];
						src[3] = (byte)((val >> 24) & 0xFF);
						src[2] = (byte)((val >> 16) & 0xFF);
						src[1] = (byte)((val >> 8) & 0xFF);//高8位
						src[0] = (byte)(val & 0xFF);//低位
						data.data = src;
					}
					else
					{
						data.sym = txt;
						data.filename = line.filename;
					}
					data.len = 4;
					pos += data.len;
					dataInsList.Add(data);
				}
				if (line.text == ".zero 4")
				{
					Data data = new Data(line);
					byte[] src = new byte[4];
					data.data = src;
					data.len = 4;
					pos += data.len;
					dataInsList.Add(data);
				}
				if (line.text == ".zero 8")
				{
					Data data = new Data(line);
					byte[] src = new byte[4];
					data.data = src;
					data.len = 8;
					pos += data.len;
					dataInsList.Add(data);
				}

				if (line.text.StartsWith(".byte"))
				{
					Data data = new Data(line);
					string txt = line.text.Substring(".byte ".Length);
					int val;
					if (int.TryParse(txt, out val))
					{
						byte[] src = new byte[4];
						src[3] = 0;
						src[2] = 0;
						src[1] = 0;//高8位
						src[0] = (byte)(val & 0xFF);//低位
						data.data = src;
					}
					else
					{
						throw new Exception("byte:" + txt);
					}
					data.len = 4;
					pos += data.len;
					dataInsList.Add(data);
				}
				if (line.text.StartsWith(".short"))
				{
					Data data = new Data(line);
					string txt = line.text.Substring(".short ".Length);
					int val;
					if (int.TryParse(txt, out val))
					{
						byte[] src = new byte[4];
						src[3] = 0;
						src[2] = 0;
						src[1] = (byte)((val >> 8) & 0xFF);//高8位
						src[0] = (byte)(val & 0xFF);//低位
						data.data = src;
					}
					else
					{
						throw new Exception("byte:" + txt);
					}
					data.len = 4;
					pos += data.len;
					dataInsList.Add(data);
				}
				//word??
			}
		}

		public static void writehex(int data, int posx, StreamWriter sw)
		{
			//:04 0000 00 0302 0100 51
			int sum = 4;
			sw.Write(":04" + getHex4(posx, ref sum) + "00" + getHex8(data, ref sum));
			sw.Write(getHex2(0x100 - (sum & 0xFF)));
			sw.WriteLine();
		}
	}
}
