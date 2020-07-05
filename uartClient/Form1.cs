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
            this.textBox1.KeyDown += textBox1_KeyDown;

            this.comboBox2.SelectedIndex = 2;
        }
        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            portName = (string)this.comboBox1.SelectedItem;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            connect(portName);
        }

        StringBuilder sb1 = new StringBuilder();
        StringBuilder sb2 = new StringBuilder();
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (fifo.Count > 0)
            {
                while (fifo.Count > 0)
                {
                    char c = fifo.Dequeue();
                    sb1.Append(c);
                    sb2.Append(" " + getHex2((byte)c));
                }
                textBox4.Text = sb1.ToString().Replace('\0', ' ');
                textBox3.Text = sb2.ToString().Replace('\0', ' ');
                this.textBox4.Select(this.textBox4.TextLength, 0);
                textBox4.ScrollToCaret();
            }
        }

        void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {

                foreach (var item in textBox1.Text)
                {
                    port.Write(new byte[] { (byte)item }, 0, 1);
                }

                if (this.comboBox2.Text == "\\0")
                {
                    port.Write(new byte[] { (byte)0 }, 0, 1);
                }
                if (this.comboBox2.Text == "\\n")
                {
                    port.Write(new byte[] { (byte)'\n' }, 0, 1);
                }
                if (this.comboBox2.Text == "\\r\\n")
                {
                    port.Write(new byte[] { (byte)'\r' }, 0, 1);
                    port.Write(new byte[] { (byte)'\n' }, 0, 1);
                }
                if (this.comboBox2.Text == "\\n\\0")
                {
                    port.Write(new byte[] { (byte)'\n' }, 0, 1);
                    port.Write(new byte[] { (byte)0 }, 0, 1);
                }
                if (this.comboBox2.Text == "\\r\\n\\0")
                {
                    port.Write(new byte[] { (byte)'\r' }, 0, 1);
                    port.Write(new byte[] { (byte)'\n' }, 0, 1);
                    port.Write(new byte[] { (byte)0 }, 0, 1);
                }
                textBox1.Text = "";
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            sb1 = new StringBuilder();
            sb2 = new StringBuilder();
            textBox4.Text = sb1.ToString().Replace('\0', ' ');
            textBox3.Text = sb2.ToString().Replace('\0', ' ');
        }

        void upload(string name)
        {
            FileStream fs = new FileStream("..\\mine.new\\" + name, FileMode.Open, FileAccess.Read);
            int len = (int)fs.Length;

            foreach (var item in "upload\n")
            {
                port.Write(new byte[] { (byte)item }, 0, 1);
            }
            Thread.Sleep(1000);
            Application.DoEvents();
            foreach (var item in name + "\n")
            {
                port.Write(new byte[] { (byte)item }, 0, 1);
            }
            Thread.Sleep(1000);
            Application.DoEvents();
            foreach (var item in len + "\n")
            {
                port.Write(new byte[] { (byte)item }, 0, 1);
            }
            Thread.Sleep(1000);
            Application.DoEvents();
            timer1_Tick(null, null);
            this.timer1.Enabled = false;

            for (int i = 0; i < len; i++)
            {
                int val = fs.ReadByte();
                port.Write(new byte[] { (byte)val }, 0, 1);
                char data;
                while (true)
                {
                    if (fifo.Count > 0)
                    {
                        data = fifo.Dequeue();
                        break;
                    }
                }
                if (data != val)
                {
                    throw new Exception();
                }
                this.Text = "" + i;
            }
            this.timer1.Enabled = true;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            upload("mbg.img");
            MessageBox.Show("ok");

        }


    }
}
