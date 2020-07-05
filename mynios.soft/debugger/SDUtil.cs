using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;
//Download by http://www.okbase.net
namespace SDcard
{
    class SDUtils
    {
        private const uint GENERIC_READ = 0x80000000;
        private const uint GENERIC_WRITE = 0x40000000;

        private const uint FILE_SHARE_READ = 0x00000001;
        private const uint FILE_SHARE_WRITE = 0x00000002;

        private const uint OPEN_EXISTING = 3;

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern SafeFileHandle CreateFileA(string lpFileName, uint dwDesiredAccess, uint dwShareMode, IntPtr lpSecurityAttributes, uint dwCreationDisposition, uint dwFlagsAndAttributes, IntPtr hTemplateFile);

        private System.IO.FileStream _DriverStream;
        private long _SectorLength = 0;
        private SafeFileHandle _DriverHandle;

        /// <summary>
        /// 扇区数
        /// </summary>
        public long SectorLength { get { return _SectorLength; } }


        /// <summary>
        /// 获取磁盘扇区信息
        /// </summary>
        /// <param name="DriverName">G:</param>
        public SDUtils(string DriverName)
        {
            try
            {
                if (DriverName == null && DriverName.Trim().Length == 0) return;
                _DriverHandle = CreateFileA("\\\\.\\" + DriverName.Trim(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero);
                _DriverStream = new System.IO.FileStream(_DriverHandle, System.IO.FileAccess.ReadWrite);
                GetSectorCount();

            }
            catch (Exception)
            {

            }
        }
        int number1;
        /// <summary>
        /// 扇区显示转换
        /// </summary>
        /// <param name="SectorBytes">扇区长度512</param>
        /// <returns>EB 52 90 ......55 AA</returns>
        public string Byte2String(byte[] SectorBytes)
        {
            if (SectorBytes == null || SectorBytes.Length != 512) return "Content is empty";
            StringBuilder ReturnText = new StringBuilder();
             
            for (number1 = 0; number1 < 16; number1++)
                ReturnText.Append(number1.ToString("X02") + " ");
            ReturnText.Append("\r\n");
            byte number=0;
            int RowCount = 0;
            ReturnText.Append(number.ToString("X02") + ":  ");
            number++;
            for (int i = 0; i != 512; i++)
            {
                ReturnText.Append(SectorBytes[i].ToString("X02") + " ");
                if (RowCount == 15 && number<=31)
                {
                    ReturnText.Append("\r\n");
                    ReturnText.Append(number.ToString("X02") + ":  ");
                    number++;
                    RowCount = -1;
                }
                RowCount++;
            }

            return ReturnText.ToString();

        }
        /// <summary>
        /// 获取扇区数
        /// </summary>
        public void GetSectorCount()
        {
            if (_DriverStream == null) return;
            _DriverStream.Position = 0;

            byte[] ReturnByte = new byte[512];
            _DriverStream.Read(ReturnByte, 0, 512); //获取第1扇区

            if (ReturnByte[0] == 0xEB && ReturnByte[1] == 0x58)          //DOS的好象都是32位
            {
                _SectorLength = (long)BitConverter.ToInt32(new byte[] { ReturnByte[32], ReturnByte[33], ReturnByte[34], ReturnByte[35] }, 0);
            }
            if (ReturnByte[0] == 0xEB && ReturnByte[1] == 0x52)          //NTFS好象是64位
            {
                _SectorLength = BitConverter.ToInt64(new byte[] { ReturnByte[40], ReturnByte[41], ReturnByte[42], ReturnByte[43], ReturnByte[44], ReturnByte[45], ReturnByte[46], ReturnByte[47] }, 0);
            }

        }


        /// <summary>
        /// 读一个扇区
        /// </summary>
        /// <param name="SectorIndex">扇区号</param>
        /// <returns>如果扇区数字大于分区信息的扇区数返回NULL</returns>
        public byte[] ReadSector(long SectorIndex)
        {
            //if (SectorIndex > _SectorLength) 
            //   return null;
            _DriverStream.Position = SectorIndex * 512;
            byte[] ReturnByte = new byte[512];
            _DriverStream.Read(ReturnByte, 0, 512); //获取扇区
            return ReturnByte;
        }
        /// <summary>
        /// 写入数据
        /// </summary>
        /// <param name="SectorBytes">扇区长度512</param>
        /// <param name="SectorIndex">扇区位置</param>
        public void WriteSector(byte[] SectorBytes, long SectorIndex)
        {
            if (SectorBytes.Length != 512) return;
            if (SectorIndex > _SectorLength) return;
            _DriverStream.Position = SectorIndex * 512;
            _DriverStream.Write(SectorBytes, 0, 512); //写入扇区 
        }
        /// <summary>
        /// 关闭
        /// </summary>
        public void Close()
        {
            _DriverStream.Close();
        }
    }
}
﻿