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
                port.DataReceived += port_DataReceived;
            }
            else
            {
                port.Close();
                //COM4为Arduino使用的串口号，需根据实际情况调整  115200
                port = new SerialPort(portName, 2000000, Parity.None, 8, StopBits.One);
                port.Open();
                port.DataReceived += port_DataReceived;
            }
        }
        public Queue<char> fifo = new Queue<char>(1024 * 1024);
        public void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            while (port.BytesToRead > 0)
            {
                fifo.Enqueue((char)port.ReadByte());
            }
        }

        string[] convt = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f" };
        public string getHex4(int val)//"ll"+"hh"
        {
            return convt[((val & 0xF0) >> 4)] + convt[((val & 0x0F))] + convt[((val & 0xF000) >> 12)] + convt[((val & 0x0F00) >> 8)];
        }
        public string getHex2(int val)//"ll"+"hh"
        {
            return convt[((val & 0xF0) >> 4)] + convt[((val & 0x0F))];
        }
        public string getHex(byte[] buff, int idx, int len)//"ll"+"hh"
        {
            StringBuilder sb = new StringBuilder();
            for (int i = idx; i < idx + len; i++)
            {
                byte val = buff[i];
                sb.Append(convt[((val & 0xF0) >> 4)] + convt[((val & 0x0F))]);
            }
            return sb.ToString();
        }

    }
}
