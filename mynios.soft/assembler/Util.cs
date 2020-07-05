using System;
namespace Assembler
{
	class Util
	{

		public static int getReg(string reg, bool noerr)
		{
			if (reg == "zero" || reg == "r0")
			{
				return 0;
			}
			else if (reg == "r1")
			{
				return 1;
			}
			else if (reg == "r2")
			{
				return 2;
			}
			else if (reg == "r3")
			{
				return 3;
			}
			else if (reg == "r4")
			{
				return 4;
			}
			else if (reg == "r5")
			{
				return 5;
			}
			else if (reg == "r6")
			{
				return 6;
			}
			else if (reg == "r7")
			{
				return 7;
			}
			else if (reg == "r8")
			{
				return 8;
			}
			else if (reg == "r9")
			{
				return 9;
			}
			else if (reg == "r10")
			{
				return 10;
			}
			else if (reg == "r11")
			{
				return 11;
			}
			else if (reg == "r12")
			{
				return 12;
			}
			else if (reg == "r13")
			{
				return 13;
			}
			else if (reg == "r14")
			{
				return 14;
			}
			else if (reg == "r15")
			{
				return 15;
			}
			else if (reg == "r16")
			{
				return 16;
			}
			else if (reg == "r17")
			{
				return 17;
			}
			else if (reg == "r18")
			{
				return 18;
			}
			else if (reg == "r19")
			{
				return 19;
			}
			else if (reg == "r20")
			{
				return 20;
			}
			else if (reg == "r21")
			{
				return 21;
			}
			else if (reg == "r22")
			{
				return 22;
			}
			else if (reg == "r23")
			{
				return 23;
			}
			else if (reg == "r24")
			{
				return 24;
			}
			else if (reg == "r25")
			{
				return 25;
			}
			else if (reg == "gp")
			{
				return 26;
			}
			else if (reg == "sp")
			{
				return 27;
			}
			else if (reg == "fp")
			{
				return 28;
			}
			else if (reg == "r29")
			{
				return 29;
			}
			else if (reg == "r30")
			{
				return 30;
			}
			else if (reg == "ra")
			{
				return 31;
			}

			if (noerr)
			{
				return -1;
			}
			else
			{
				throw new Exception("reg not found");
			}
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
				case 13: return "rbase";
			}
			return "reg(" + val + ")";
		}

		public static string matchReg(string s, out string s2)
		{
			string word = "";
			for (int i = 0; i < s.Length; i++)
			{
				var c = s[i];
				if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
				{
					word += c;
				}
				else
				{
					break;
				}
			}
			s2 = s.Substring(word.Length);
			return word;
		}
		public static string matchSym(string s, out string s2)
		{
			string word = "";
			for (int i = 0; i < s.Length; i++)
			{
				var c = s[i];
				bool match = false;
				if (i == 0)
				{
					if (c == '.' || c == '_' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
					{
						match = true;
					}
				}
				else
				{
					if (c == '_' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
					{
						match = true;
					}
				}
				if (match)
				{
					word += c;
				}
				else
				{
					break;
				}
			}
			s2 = s.Substring(word.Length);
			return word;
		}
		public static string matchNum(string s, out string s2, out int? val)
		{
			string word = "";
			for (int i = 0; i < s.Length; i++)
			{
				var c = s[i];
				bool match = false;
				if (i == 0)
				{
					if (c == '+' || c == '-' || c >= '0' && c <= '9')
					{
						match = true;
					}
				}
				else
				{
					if (c >= '0' && c <= '9')
					{
						match = true;
					}
				}
				if (match)
				{
					word += c;
				}
				else
				{
					break;
				}
			}
			val = null;
			if (word.Length > 0)
			{
				val = int.Parse(word);
			}
			s2 = s.Substring(word.Length);
			return word;
		}

		public static string tostr(int val)
		{
			if (val >= 0)
			{
				return "+" + val.ToString();
			}
			else
			{
				return val.ToString();
			}
		}

		public static int align4(int val)
		{
			if ((val & 0x03) != 0)
			{
				return (int)((val & 0xFFFFFFFC) + 4);
			}
			return val;
		}


		public static string check(string val)
		{
			if (val == null)
			{
				throw new Exception();
			}
			return val;
		}

	}
}
